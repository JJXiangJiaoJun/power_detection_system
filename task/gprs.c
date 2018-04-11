#include "gprs.h"
#include <string.h>
#include <stdlib.h>
#include "usart2.h"
#include "totaltask.h"
/****************������ƿ��������ջ����**************/
//����4
OS_TCB GPRSTastTCB;
CPU_STK GPRS_TASK_STK[GPRS_STK_SIZE];
//����5
OS_TCB GPRSRUNTastTCB;
CPU_STK GPRSRUN_TASK_STK[GPRSRUN_STK_SIZE];
//����6
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
/*************�ڲ��ṹ��**********/
struct sendinit
{
	char mess[50];
	int times;
	int pertime;
};
char ATtemp[80];
//#define FINDSERCERID(S) 
//#define SERVER 1728x3v189.iok.la
/**********�ڲ�����*******/
//���øú�������ָ�������timesΪ���ʹ�����ÿ�η��ͼ��1s
static void send(char* mess,int times,int pertime)
{
	OS_ERR err;
	struct sendinit *temp;
	temp = mymalloc(1,sizeof(struct sendinit));
	mymemcpy(temp->mess,mess,50);
	temp->times = times;
	temp->pertime = pertime;
	OSTaskCreate((OS_TCB 	* )&GPRSSENDTastTCB,		//������ƿ�
				 (CPU_CHAR	* )"gprsend task", 		//��������
                 (OS_TASK_PTR )gprs_send_task, 			//������
                 (void		* )temp,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )GPRSSEND_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&GPRSSEND_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)GPRSSEND_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)GPRSSEND_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP, //����ѡ��,Ϊ�˱���������������񶼱��渡��Ĵ�����ֵ
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
}
//����send����Ĺر�
static void sendoff()
{
	OS_ERR err;
	OSTaskDel((OS_TCB*)&GPRSSENDTastTCB,&err);
}
//���������Ҽ���Ӧ
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
				return 1;//�ɹ�����1
			}
			else
			{
				myfree(1,a);
				a = NULL;
			}
		}
		else if(err == OS_ERR_TIMEOUT)
		{
			return 0;//ʧ�ܷ���0
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
	/********�������Ľ��������쳣�������**********/
	OS_ERR err;
	OSTaskCreate((OS_TCB 	* )&GPRSTastTCB,		//������ƿ�
				 (CPU_CHAR	* )"gprs task", 		//��������
                 (OS_TASK_PTR )gprs_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )GPRS_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&GPRS_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)GPRS_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)GPRS_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )10,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP, //����ѡ��,Ϊ�˱���������������񶼱��渡��Ĵ�����ֵ
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
		OSTaskCreate((OS_TCB 	* )&GPRSRUNTastTCB,		//������ƿ�
				 (CPU_CHAR	* )"gprsrun task", 		//��������
                 (OS_TASK_PTR )gprs_run_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )GPRSRUN_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&GPRSRUN_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)GPRSRUN_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)GPRSRUN_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )10,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP, //����ѡ��,Ϊ�˱���������������񶼱��渡��Ĵ�����ֵ
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	if(err != OS_ERR_NONE)
		return 0;//ʧ�ܷ���0
	return 1;//�ɹ�����1
}


/****************************************/
//��������:���̿���
void gprs_run_task(void* p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE size;
	struct test a;
	uart2_init(115200);		        //���ڳ�ʼ��
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
		//��ģ��һ��������ʱ��
		//OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_HMSM_STRICT,&err);
		//����Ƿ񿪻��ɹ�
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
//		for(int j=0;j<sizeof(struct test);j++)                     //ѭ����������
//	{
//		while((USART3->ISR&0X40)==0);     //ѭ������,ֱ���������   
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
//		//����Ƿ�رջ���
//		if(!send_and_checkack("ATE0","OK",2,1000))
//		{
//			printlcd("WRONG");
//		}
//		printlcd("ATE0OK");
//		//����ź�ǿ��
//		if(!send_and_checkack("AT+CSQ","+CSQ:",2,1000))
//		{
//			printlcd("WRONG");
//		}
//		int csq;
//		sscanf(ATtemp,"+CSQ: %d,0",&csq);
//		printlcd("CSQOK,csq = %d",csq);
//		
//		//����Ƿ���gprs����
//		if(!send_and_checkack("AT+CGATT?","+CGATT: 1",2,1000))
//		{
//			printlcd("WRONG");
//		}
//		printlcd("CGATTOK");
//		//����APN
//		if(!send_and_checkack("AT+CSTT","OK",2,1000))
//		{
//			printlcd("WRONG");
//		}
//		printlcd("APNOK");
//				//�����ƶ�����
//		if(!send_and_checkack("AT+CIICR","OK",2,1000))
//		{
//			printlcd("WRONG");
//		}
//		printlcd("MOVEOK");
//		//��ȡ����ip��ַ
////		if(!send_and_checkack("AT+CIFSR",".",2,5000))
////		{
////			printlcd("WRONG");
////		}
////		char ip1,ip2,ip3,ip4;
////		sscanf(ATtemp,"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);
////		printlcd("ip = %d.%d.%d.%d",ip1,ip2,ip3,ip4);
//		
//		//��ȡĿ�������ip��ַ
////		if(!send_and_checkack("AT+CDNSGIP=1728x3v189.iok.la","1728x3v189.iok.la",2,10000))
////		{
////			printlcd("WRONG");
////		}
////		char serip1,serip2,serip3,serip4;
////		printlcd("%s",ATtemp);
////		sscanf(ATtemp,"%*[^,],%*[^,],\"%d.%d.%d.%d%*[^\"]",&serip1,&serip2,&serip3,&serip4);
////		printlcd("serip = %d.%d.%d.%d",serip1,serip2,serip3,serip4);
//		
//		//��ע�����ϵ�gprs��������Ӧ�ı�Ϊ��ʼ��������
//		char *serverip = "119.29.202.238";
//		//����������������,echo����
//		
//		if(!send_and_checkack("AT+CIPSTART=\"TCP\",\"119.29.202.238\",\"1256\"","OK",1,20000))
//		{
//			printlcd("WRONG");
//		}
//		printlcd("connect ok");
//		while(1)
//		{
//			OSTimeDlyHMSM(0,0,5,0,OS_OPT_TIME_HMSM_STRICT,&err);
//		//��ʼд���ַ���
//		if(!send_and_checkack("AT+CIPSEND",">",1,50000))
//		{
//			printlcd("WRONG");
//		}
//		printlcd("start send");
//		//�ַ�����д�����ȡ��Ҫ����ĺ�������д
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
			OSTaskQPost((OS_TCB*	)&GPRSRUNTastTCB,	//������GPRSTast������Ϣ
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
			OSTaskQPost((OS_TCB*	)&GPRSRUNTastTCB,	//������GPRSTast������Ϣ
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
	/************��������Ľ�*********/
	OS_ERR err;
	OSTaskDel((OS_TCB*)&GPRSTastTCB,&err);
	OSTaskDel((OS_TCB*)&GPRSRUNTastTCB,&err);
	if(err != OS_ERR_NONE)
		return 0;//ʧ�ܷ���0
	return 1;//�ɹ�����1
}
