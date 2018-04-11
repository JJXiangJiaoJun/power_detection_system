#include "bdgps.h"
#include "totaltask.h"
#include "realtime.h"
#include "usart3.h"
/****************任务控制块与任务堆栈定义**************/
//任务8
OS_TCB BDGPSTastTCB;
CPU_STK BDGPS_TASK_STK[BDGPS_STK_SIZE];
#define TIMEZONE_FLAG 8
/**********************内部函数*************************/

//从buf里面得到第cx个逗号所在的位置
//返回值:0~0XFE,代表逗号所在位置的偏移.
//       0XFF,代表不存在第cx个逗号							  
u8 NMEA_Comma_Pos(u8 *buf,u8 cx)
{	 		    
	u8 *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//遇到'*'或者非法字符,则不存在第cx个逗号
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}

//m^n函数
//返回值:m^n次方.
u32 NMEA_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//str转换为数字,以','或者'*'结束
//buf:数字存储区
//dx:小数点位数,返回给调用函数
//返回值:转换后的数值
int NMEA_Str2num(u8 *buf,u8*dx)
{
	u8 *p=buf;
	u32 ires=0,fres=0;
	u8 ilen=0,flen=0,i;
	u8 mask=0;
	int res;
	while(1) //得到整数和小数的长度
	{
		if(*p=='-'){mask|=0X02;p++;}//是负数
		if(*p==','||(*p=='*'))break;//遇到结束了
		if(*p=='.'){mask|=0X01;p++;}//遇到小数点了
		else if(*p>'9'||(*p<'0'))	//有非法字符
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;
		else ilen++;
		p++;
	}
	if(mask&0X02)buf++;	//去掉负号
	for(i=0;i<ilen;i++)	//得到整数部分数据
	{  
		ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5)flen=5;	//最多取5位小数
	*dx=flen;	 		//小数点位数
	for(i=0;i<flen;i++)	//得到小数部分数据
	{  
		fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	} 
	res=ires*NMEA_Pow(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
}

//提取GNRMC信息
//buf:接收到的GPS数据缓冲区首地址
void GNRMC_Analysis(u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;     
	u32 temp;
	utc *gpsx;
	p1=(u8*)strstr((const char *)buf,"GNRMC");//"$GNRMC",经常有&和GNRMC分开的情况,故只判断GNRMC.
	posx=NMEA_Comma_Pos(p1,1);								//得到UTC时间
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);	 	//得到UTC时间,去掉ms
		gpsx->hour=temp/10000;
		gpsx->min=(temp/100)%100;
		gpsx->sec=temp%100;	 	 
	}
	
	posx=NMEA_Comma_Pos(p1,9);								//得到UTC日期
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 				//得到UTC日期
		gpsx->date=temp/10000;
		gpsx->month=(temp/100)%100;
		gpsx->year=temp%100;	 	 
	}
	gpsx->hour+=TIMEZONE_FLAG;
	if(gpsx->hour>=24)
	{
		gpsx->hour-=24;
		gpsx->date++;
	}
	if(gpsx->date!=0)
		setrealtime(gpsx->year,gpsx->month,gpsx->date,gpsx->hour,gpsx->min,gpsx->sec,0);
	
}

/**********************************外部函数*************************************/

//gps初始化函数
char bdgps_task_init(void* p_arg)
{
	OS_ERR err;
	
	OSTaskCreate((OS_TCB 	* )&BDGPSTastTCB,		//任务控制块
				 (CPU_CHAR	* )"bdgps task", 		//任务名字
                 (OS_TASK_PTR )bdgps_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )BDGPS_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&BDGPS_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)BDGPS_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)BDGPS_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )10,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP, //任务选项,为了保险起见，所有任务都保存浮点寄存器的值
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
								 
	if(err != OS_ERR_NONE)
		return 0;//失败返回0
	return 1;//成功返回1
}

void bdgps_task(void* p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE size;
	int buffernum = 0;
	char* p;
	char buffer[100];
	char temp;
	uart3_init(38400);
	//__HAL_UART_ENABLE_IT(&UART3_Handler,USART_IT_RXNE);
	memset(buffer,0,sizeof(buffer));
	///////////////////////
	while(1)
	{
		p=OSTaskQPend((OS_TICK		)0,
                      (OS_OPT		)OS_OPT_PEND_BLOCKING,
                      (OS_MSG_SIZE*	)&size,
                      (CPU_TS*		)0,
                      (OS_ERR*      )&err);
		temp = *p;
		myfree(1,p);
		if(temp == '\n')
		{
			if((strncmp(buffer,"$GNRMC",6))==0)
			GNRMC_Analysis((u8*)buffer);
			memset(buffer,0,sizeof(buffer));
			buffernum = 0;
		}
		else if(temp == '\r')
			continue;
		else
			{
			buffer[buffernum]=temp;
			buffernum++;
			}
	}
}
char bdgps_task_deinit(void* p_arg)
{
	/************错误处理需改进*********/
	OS_ERR err;
	HAL_UART_DeInit(&UART3_Handler);
	__HAL_UART_DISABLE_IT(&UART3_Handler,USART_IT_RXNE);
	HAL_NVIC_DisableIRQ(USART3_IRQn);
	HAL_GPIO_DeInit(GPIOB,GPIO_PIN_10|GPIO_PIN_11);
	__HAL_RCC_USART3_CLK_DISABLE();	
	// INTX_DISABLE();
	OSTaskDel((OS_TCB*)&BDGPSTastTCB,&err);
	if(err != OS_ERR_NONE)
		return 0;//失败返回0
	return 1;//成功返回1
}
