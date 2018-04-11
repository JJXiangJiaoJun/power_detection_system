#include "gprs.h"
#include <string.h>
#include <stdlib.h>
#include "usart2.h"
#include "totaltask.h"
/****************任务控制块与任务堆栈定义**************/
//任务4
OS_TCB GPRSTastTCB;
CPU_STK GPRS_TASK_STK[GPRS_STK_SIZE];
//任务5
OS_TCB GPRSRUNTastTCB;
CPU_STK GPRSRUN_TASK_STK[GPRSRUN_STK_SIZE];
//任务6
OS_TCB GPRSSENDTastTCB;
CPU_STK GPRSSEND_TASK_STK[GPRSSEND_STK_SIZE];
struct test
{
	int32_t a;
	int32_t b;
	float aa;
	float ab;
	float ac;
	float ad;
	float ae;
	float af;
	float ag;
	float ah;
};
extern UART_HandleTypeDef UART2_Handler;  
/*************内部结构体**********/
struct sendinit
{
	char mess[50];
	int times;
	int pertime;
};
char ATtemp[80];
//#define FINDSERCERID(S) 
//#define SERVER 1728x3v189.iok.la
/**********内部函数*******/
//调用该函数发送指定的命令，times为发送次数，每次发送间隔1s
static void send(char* mess,int times,int pertime)
{
	OS_ERR err;
	struct sendinit *temp;
	temp = mymalloc(1,sizeof(struct sendinit));
	mymemcpy(temp->mess,mess,50);
	temp->times = times;
	temp->pertime = pertime;
	OSTaskCreate((OS_TCB 	* )&GPRSSENDTastTCB,		//任务控制块
				 (CPU_CHAR	* )"gprsend task", 		//任务名字
                 (OS_TASK_PTR )gprs_send_task, 			//任务函数
                 (void		* )temp,					//传递给任务函数的参数
                 (OS_PRIO	  )GPRSSEND_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&GPRSSEND_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)GPRSSEND_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)GPRSSEND_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP, //任务选项,为了保险起见，所有任务都保存浮点寄存器的值
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
}
//用于send任务的关闭
static void sendoff()
{
	OS_ERR err;
	OSTaskDel((OS_TCB*)&GPRSSENDTastTCB,&err);
}
//发送任务并且检查回应
static int send_and_checkack(char* mess,char *ack,int times,int persendtime)
{
	char temp[100];
	OS_ERR err;
	OS_MSG_SIZE size;
	char *a=NULL;
	strcpy (temp,mess);
	strcat (temp,"\r\n");
	printf("%s",temp);
	send(temp,times,persendtime);
	while(1)
	{
	a = OSTaskQPend((OS_TICK		)persendtime*times,
                      (OS_OPT		)OS_OPT_PEND_BLOCKING,
                      (OS_MSG_SIZE*	)&size,
                      (CPU_TS*		)0,
                      (OS_ERR*      )&err);
		//printlcd("sendat");
		if(err ==OS_ERR_NONE && a!=NULL)
		{
			//printlcd("proget = %s",a);
			//printlcd("check = %s",ack);
			if(strstr(a,ack))
			{
				mymemcpy(ATtemp,a,50);
				myfree(1,a);
				a = NULL;
				sendoff();
				return 1;//成功返回1
			}
			else
			{
				myfree(1,a);
				a = NULL;
			}
		}
		else if(err == OS_ERR_TIMEOUT)
		{
			return 0;//失败返回0
		}
		if(a!=NULL)
		{
			myfree(1,a);
			a = NULL;
		}
	}
		
}

/**************************/
char gprs_task_init(void* p_arg)
{
	/********函数待改进，加入异常处理机制**********/
	OS_ERR err;
	OSTaskCreate((OS_TCB 	* )&GPRSTastTCB,		//任务控制块
				 (CPU_CHAR	* )"gprs task", 		//任务名字
                 (OS_TASK_PTR )gprs_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )GPRS_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&GPRS_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)GPRS_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)GPRS_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )10,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP, //任务选项,为了保险起见，所有任务都保存浮点寄存器的值
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
		OSTaskCreate((OS_TCB 	* )&GPRSRUNTastTCB,		//任务控制块
				 (CPU_CHAR	* )"gprsrun task", 		//任务名字
                 (OS_TASK_PTR )gprs_run_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )GPRSRUN_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&GPRSRUN_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)GPRSRUN_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)GPRSRUN_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )10,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP, //任务选项,为了保险起见，所有任务都保存浮点寄存器的值
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	if(err != OS_ERR_NONE)
		return 0;//失败返回0
	return 1;//成功返回1
}


/****************************************/
//函数作用:流程控制
void gprs_run_task(void* p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE size;
	struct test a;
	uart2_init(115200);		        //串口初始化
	a.a =1;
	a.b = 2;
	a.aa = 3.3;
	a.ab = 4.4;
	a.ac = 5.5;
	a.ad = 6.6;
	a.ae = 7.7;
	a.af = 8.8;
	a.ag = 9.9;
	a.ah = 10.10;
	while(1)
	{
		//给模块一个开机的时间
		//OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_HMSM_STRICT,&err);
		//检测是否开机成功
		while(!send_and_checkack("AT","OK",1,1000))
		{
			printlcd("Please Check Gprs Connetion");
		}
		printlcd("GPRS OK");
		OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_HMSM_STRICT,&err);
		printlcd("GPRS START");
		//send("ATE0\r\n",1,100);
		//send_and_checkack("ATE0","OK",1,1000);
		if(!send_and_checkack("ATE0","OK",1,1000))
		{
			printlcd("WRONG");
		}
		printlcd("ATE0OK");
		OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err);
		//send("AT+CIPSHUT\r\n",1,100);
		OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err);
		//send("AT+CIPSTART=\"TCP\",\"119.29.202.238\",\"1256\"\r\n",1,1000);
		OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err);
		while(!send_and_checkack("AT+CIPSTART=\"TCP\",\"119.29.202.238\",\"1256\"","CONNECT OK",1,5000))
		{
				printlcd("CONNECT WRONG");
			send("AT+CIPSHUT\r\n",1,100);
			OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_HMSM_STRICT,&err);
		}
		printlcd("CONNECT OK");
		send("AT+CIPSEND\r\n",1,100);
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
		u2_printf("send\n");
		printlcd("size = %d",sizeof(a));
		HAL_UART_Transmit(&UART2_Handler,(u8*)&a,sizeof(a),1000);
//		for(int j=0;j<sizeof(struct test);j++)                     //循环发送数据
//	{
//		while((USART3->ISR&0X40)==0);     //循环发送,直到发送完毕   
//		USART3->TDR=p[sizeof(struct test) - j -1];  
//	}
		u2_printf("\n");
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
		u2_printf("%c",0x1a);
		send("AT+CIPSHUT\r\n",1,100);
		while(1)
		{
			printlcd("SEND OVER");
			OSTimeDlyHMSM(1,0,0,0,OS_OPT_TIME_HMSM_STRICT,&err);
		}
//		if(!send_and_checkack("AT","OK",10,1000))
//		{
//			printlcd("WRONG");
//		}
//		printlcd("ATOK");
//		//检测是否关闭回显
//		if(!send_and_checkack("ATE0","OK",2,1000))
//		{
//			printlcd("WRONG");
//		}
//		printlcd("ATE0OK");
//		//检测信号强度
//		if(!send_and_checkack("AT+CSQ","+CSQ:",2,1000))
//		{
//			printlcd("WRONG");
//		}
//		int csq;
//		sscanf(ATtemp,"+CSQ: %d,0",&csq);
//		printlcd("CSQOK,csq = %d",csq);
//		
//		//检测是否附着gprs网络
//		if(!send_and_checkack("AT+CGATT?","+CGATT: 1",2,1000))
//		{
//			printlcd("WRONG");
//		}
//		printlcd("CGATTOK");
//		//设置APN
//		if(!send_and_checkack("AT+CSTT","OK",2,1000))
//		{
//			printlcd("WRONG");
//		}
//		printlcd("APNOK");
//				//激活移动场景
//		if(!send_and_checkack("AT+CIICR","OK",2,1000))
//		{
//			printlcd("WRONG");
//		}
//		printlcd("MOVEOK");
//		//获取本机ip地址
////		if(!send_and_checkack("AT+CIFSR",".",2,5000))
////		{
////			printlcd("WRONG");
////		}
////		char ip1,ip2,ip3,ip4;
////		sscanf(ATtemp,"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);
////		printlcd("ip = %d.%d.%d.%d",ip1,ip2,ip3,ip4);
//		
//		//获取目标服务器ip地址
////		if(!send_and_checkack("AT+CDNSGIP=1728x3v189.iok.la","1728x3v189.iok.la",2,10000))
////		{
////			printlcd("WRONG");
////		}
////		char serip1,serip2,serip3,serip4;
////		printlcd("%s",ATtemp);
////		sscanf(ATtemp,"%*[^,],%*[^,],\"%d.%d.%d.%d%*[^\"]",&serip1,&serip2,&serip3,&serip4);
////		printlcd("serip = %d.%d.%d.%d",serip1,serip2,serip3,serip4);
//		
//		//该注释往上的gprs操作代码应改变为初始化操作。
//		char *serverip = "119.29.202.238";
//		//打开往服务器的链接,echo测试
//		
//		if(!send_and_checkack("AT+CIPSTART=\"TCP\",\"119.29.202.238\",\"1256\"","OK",1,20000))
//		{
//			printlcd("WRONG");
//		}
//		printlcd("connect ok");
//		while(1)
//		{
//			OSTimeDlyHMSM(0,0,5,0,OS_OPT_TIME_HMSM_STRICT,&err);
//		//开始写入字符串
//		if(!send_and_checkack("AT+CIPSEND",">",1,50000))
//		{
//			printlcd("WRONG");
//		}
//		printlcd("start send");
//		//字符串的写入与读取需要另外的函数，待写
//		u3_printf("%s\r\n",test);
//		//u3_printf("hello");
//		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
//		u3_printf("%c",0x1a);
//	}
//		OSTimeDlyHMSM(0,0,100,0,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}

void gprs_send_task(void* p_arg)
{
	OS_ERR err;
	struct sendinit a;
	int times;
	memcpy(&a,p_arg,sizeof(struct sendinit));
	myfree(1,p_arg);
	times = a.times;
	int pertime = a.pertime;
	while(times--)
	{
		u2_printf("%s",a.mess); 
		//printlcd("times = %d",times);
		OSTimeDlyHMSM(0,0,0,pertime,OS_OPT_TIME_HMSM_STRICT,&err);
	}
	sendoff();
}

void gprs_task(void* p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE size;
	int buffernum = 0;
	char* p;
	char buffer[100];
	char temp;
	char *a = NULL;
	memset(buffer,0,sizeof(buffer));
	while(1)
	{
		p=OSTaskQPend((OS_TICK		)0,
                      (OS_OPT		)OS_OPT_PEND_BLOCKING,
                      (OS_MSG_SIZE*	)&size,
                      (CPU_TS*		)0,
                      (OS_ERR*      )&err);
		temp = *p;
		myfree(1,p);
		//printlcd("%c",temp);
		if(temp == '>')
		{
			a = mymalloc(1,sizeof(char)*50);
			*a = '>';
			OSTaskQPost((OS_TCB*	)&GPRSRUNTastTCB,	//向任务GPRSTast发送消息
                    (void*		)a,
                    (OS_MSG_SIZE)100,
                    (OS_OPT		)OS_OPT_POST_FIFO,(OS_ERR*	)&err);
			memset(buffer,0,sizeof(buffer));
			buffernum = 0;
		}
		else if(temp == '\n')
		{
			//printlcd("gprsget = %s",buffer);
			a = mymalloc(1,sizeof(char)*100);
			mymemcpy(a,buffer,sizeof(char)*100);
			OSTaskQPost((OS_TCB*	)&GPRSRUNTastTCB,	//向任务GPRSTast发送消息
                    (void*		)a,
                    (OS_MSG_SIZE)100,
                    (OS_OPT		)OS_OPT_POST_FIFO,(OS_ERR*	)&err);
			memset(buffer,0,sizeof(buffer));
			buffernum = 0;
		}
		else if(temp == '\r')
			continue;
		else
			{
			buffer[buffernum]=temp;
			buffernum++;
				if(buffernum>=50)
					buffernum = 0;
			}
	}
}

char gprs_task_deinit(void* p_arg)
{
	/************错误处理需改进*********/
	OS_ERR err;
	OSTaskDel((OS_TCB*)&GPRSTastTCB,&err);
	OSTaskDel((OS_TCB*)&GPRSRUNTastTCB,&err);
	if(err != OS_ERR_NONE)
		return 0;//失败返回0
	return 1;//成功返回1
}
