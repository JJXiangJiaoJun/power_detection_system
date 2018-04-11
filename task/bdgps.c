#include "bdgps.h"
#include "totaltask.h"
#include "realtime.h"
#include "usart3.h"
/****************������ƿ��������ջ����**************/
//����8
OS_TCB BDGPSTastTCB;
CPU_STK BDGPS_TASK_STK[BDGPS_STK_SIZE];
#define TIMEZONE_FLAG 8
/**********************�ڲ�����*************************/

//��buf����õ���cx���������ڵ�λ��
//����ֵ:0~0XFE,����������λ�õ�ƫ��.
//       0XFF,�������ڵ�cx������							  
u8 NMEA_Comma_Pos(u8 *buf,u8 cx)
{	 		    
	u8 *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//����'*'���߷Ƿ��ַ�,�򲻴��ڵ�cx������
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}

//m^n����
//����ֵ:m^n�η�.
u32 NMEA_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//strת��Ϊ����,��','����'*'����
//buf:���ִ洢��
//dx:С����λ��,���ظ����ú���
//����ֵ:ת�������ֵ
int NMEA_Str2num(u8 *buf,u8*dx)
{
	u8 *p=buf;
	u32 ires=0,fres=0;
	u8 ilen=0,flen=0,i;
	u8 mask=0;
	int res;
	while(1) //�õ�������С���ĳ���
	{
		if(*p=='-'){mask|=0X02;p++;}//�Ǹ���
		if(*p==','||(*p=='*'))break;//����������
		if(*p=='.'){mask|=0X01;p++;}//����С������
		else if(*p>'9'||(*p<'0'))	//�зǷ��ַ�
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;
		else ilen++;
		p++;
	}
	if(mask&0X02)buf++;	//ȥ������
	for(i=0;i<ilen;i++)	//�õ�������������
	{  
		ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5)flen=5;	//���ȡ5λС��
	*dx=flen;	 		//С����λ��
	for(i=0;i<flen;i++)	//�õ�С����������
	{  
		fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	} 
	res=ires*NMEA_Pow(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
}

//��ȡGNRMC��Ϣ
//buf:���յ���GPS���ݻ������׵�ַ
void GNRMC_Analysis(u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;     
	u32 temp;
	utc *gpsx;
	p1=(u8*)strstr((const char *)buf,"GNRMC");//"$GNRMC",������&��GNRMC�ֿ������,��ֻ�ж�GNRMC.
	posx=NMEA_Comma_Pos(p1,1);								//�õ�UTCʱ��
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);	 	//�õ�UTCʱ��,ȥ��ms
		gpsx->hour=temp/10000;
		gpsx->min=(temp/100)%100;
		gpsx->sec=temp%100;	 	 
	}
	
	posx=NMEA_Comma_Pos(p1,9);								//�õ�UTC����
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 				//�õ�UTC����
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

/**********************************�ⲿ����*************************************/

//gps��ʼ������
char bdgps_task_init(void* p_arg)
{
	OS_ERR err;
	
	OSTaskCreate((OS_TCB 	* )&BDGPSTastTCB,		//������ƿ�
				 (CPU_CHAR	* )"bdgps task", 		//��������
                 (OS_TASK_PTR )bdgps_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )BDGPS_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&BDGPS_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)BDGPS_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)BDGPS_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )10,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP, //����ѡ��,Ϊ�˱���������������񶼱��渡��Ĵ�����ֵ
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
								 
	if(err != OS_ERR_NONE)
		return 0;//ʧ�ܷ���0
	return 1;//�ɹ�����1
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
	/************��������Ľ�*********/
	OS_ERR err;
	HAL_UART_DeInit(&UART3_Handler);
	__HAL_UART_DISABLE_IT(&UART3_Handler,USART_IT_RXNE);
	HAL_NVIC_DisableIRQ(USART3_IRQn);
	HAL_GPIO_DeInit(GPIOB,GPIO_PIN_10|GPIO_PIN_11);
	__HAL_RCC_USART3_CLK_DISABLE();	
	// INTX_DISABLE();
	OSTaskDel((OS_TCB*)&BDGPSTastTCB,&err);
	if(err != OS_ERR_NONE)
		return 0;//ʧ�ܷ���0
	return 1;//�ɹ�����1
}
