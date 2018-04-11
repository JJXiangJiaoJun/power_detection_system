#include "test.h"
#include "led.h"
#include "totaltask.h"
#include "usart2.h"
#include "realtime.h"
/****************������ƿ��������ջ����**************/
//�������:1
OS_TCB LEDTastTCB;
CPU_STK LED_TASK_STK[LED_STK_SIZE];

DATAPACKET_TypeDef DEMO;

void DEMO_init()
{
    DEMO.circuit=100.00234;
	DEMO.day=9;
	DEMO.hour=13;
	DEMO.min =30;
	DEMO.month=2;
	DEMO.sec=47;
	DEMO.voltage=220.20345;
	DEMO.year=17;
	DEMO.activepower=90.56342;
	DEMO.apparentoutput=28.72454;
	DEMO.distortion_voltage=34.83787;
	DEMO.frequency=50.99345;
	DEMO.powerfactor=40.29565;
	DEMO.reactivepower=123.92234;
}

char led_task_init(void* p_arg)
{
	OS_ERR err;
	OSTaskCreate((OS_TCB 	* )&LEDTastTCB,		//������ƿ�
				 (CPU_CHAR	* )"led task", 		//��������
                 (OS_TASK_PTR )led_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )LED_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&LED_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)LED_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)LED_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP, //����ѡ��,Ϊ�˱���������������񶼱��渡��Ĵ�����ֵ
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	if(err != OS_ERR_NONE)
		return 0;//ʧ�ܷ���0
	return 1;//�ɹ�����1
}

void led_task(void* p_arg)
{
	OS_ERR err;
//	char a = 0;
	DEMO_init();
	Time_Struct ttt;
	char tbuf[100];
	while(1)
	{
		LED1_Toggle;
		ttt=getrealtime();
		sprintf((char *)tbuf,"20%02d-%02d-%02d  %02d:%02d:%02d:%03d",ttt.year,ttt.month,ttt.date,ttt.hour,ttt.minute,ttt.second,ttt.musecond);
		printlcd("%s",tbuf);
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
		/*a++;
		LED1_Toggle;
			OSQPost((OS_Q*		)&DATAPACK_Msg,		
					(void*		)&DEMO,
					(OS_MSG_SIZE)sizeof(DEMO),
					(OS_OPT		)OS_OPT_POST_FIFO,
					(OS_ERR*	)&err);
					DEMO.sec+=1;
		//u3_printf("AT\r\n"); 
		printlcd("hello,%d",a);
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);*/
		
		
	}
}

char led_task_deinit(void* p_arg)
{
	OS_ERR err;
	OSTaskDel((OS_TCB*)&LEDTastTCB,&err);
	if(err != OS_ERR_NONE)
		return 0;//ʧ�ܷ���0
	return 1;//�ɹ�����1
}
