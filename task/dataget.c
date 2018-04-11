#include "dataget.h"
#include "totaltask.h"
#include "ATT7022E.h"
#include "bdgps.h"
/****************������ƿ��������ջ����**************/
//������ƿ�
OS_TCB DATAGETTastTCB;
//�����ջ	
CPU_STK DATAGET_TASK_STK[DATAGET_STK_SIZE];
/**********************�ڲ�����*************************/



/**********************************�ⲿ����*************************************/

//dataget��ʼ������
static void init()
{
	GPIO_InitTypeDef GPIO_Initure;
	GPIO_Initure.Pin=GPIO_PIN_1;			  //PB0
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_OD;		              //�����������
	GPIO_Initure.Pull=GPIO_NOPULL;			              //����
	GPIO_Initure.Speed=GPIO_SPEED_FAST;		              //����
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);	            	  //��ʼ��PB1
}
static void open()
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,1);
}
static void close()
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,0);
}
char dataget_task_init(void* p_arg)
{
	OS_ERR err;
	
	OSTaskCreate((OS_TCB 	* )&DATAGETTastTCB,		//������ƿ�
				 (CPU_CHAR	* )"dataget task", 		//��������
                 (OS_TASK_PTR )dataget_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )DATAGET_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&DATAGET_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)DATAGET_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)DATAGET_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )10,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP, //����ѡ��,Ϊ�˱���������������񶼱��渡��Ĵ�����ֵ
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	if(err != OS_ERR_NONE)
		return 0;//ʧ�ܷ���0
	return 1;//�ɹ�����1
}


static void sendToSdcard()
{
	OS_ERR err;
	OSSemPost(&SDcardWrite,OS_OPT_POST_1,&err);
}
void dataget_task(void* p_arg)
{
	OS_ERR err;
	init();
	do
	{
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
		//����ʼ���Ƿ�ɹ�
		InitAmmeter();
		printlcd("try init att7022e");
	}
	while(!ATT7022E_Check());
	printlcd("att7022e ok");
	while(1)
	{
		bdgps_task_deinit(0);
		//close();
		//OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
		//OSSchedLock(&err);
		if(ReadAmmeterData())
		//if(ReadSend_ALL_AmmeterData())
		{
			//OSSchedUnlock(&err);	
			ReadSend_ALL_AmmeterData();
			printlcd("att7022e read ok");
			//OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err);
			printlcd("send to SDCard");
			//OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);
			//ReadSend_ALL_AmmeterData();
			sendToSdcard();
		}
		else
		{
			//OSSchedUnlock(&err);	
			printlcd("att7022e read wrong");
		}
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
		bdgps_task_init(0);
		OSTimeDlyHMSM(0,0,19,0,OS_OPT_TIME_HMSM_STRICT,&err);
		//open();	
	}
}
char dataget_task_deinit(void* p_arg)
{
	/************��������Ľ�*********/
	OS_ERR err;
	OSTaskDel((OS_TCB*)&DATAGETTastTCB,&err);
	if(err != OS_ERR_NONE)
		return 0;//ʧ�ܷ���0
	return 1;//�ɹ�����1
}
