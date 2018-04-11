#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "ltdc.h"
#include "sdram.h"
#include "usart2.h"
#include "mpu.h"
#include "includes.h"
#include "totaltask.h"

/************�����*************/
#include "exfuns.h"
#include "ff.h"
#include "ftl.h"
/***********ë����*************/
#include "bdgps.h"
#include "rtc.h"
#include "timer.h"
/*****************������ͷ�ļ�******************/
#include "test.h"
#include "lcdshow.h"
/************************************************
 ���ȼ�0���жϷ������������� OS_IntQTask()
 ���ȼ�1��ʱ�ӽ������� OS_TickTask()
 ���ȼ�2����ʱ���� OS_TmrTask()
 ���ȼ�OS_CFG_PRIO_MAX-2��ͳ������ OS_StatTask()
 ���ȼ�OS_CFG_PRIO_MAX-1���������� OS_IdleTask()

 ����64�����ȼ�
************************************************/

//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С	
#define START_STK_SIZE 		128
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

////////////////////////////////////////////////////////

//LCDˢ��ʱʹ�õ���ɫ
int lcd_discolor[14]={	WHITE, RED,   BLUE,  BRED,      
						GRED,  GBLUE, BLACK,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };


int main(void)
{
    
	 OS_ERR err;
	/************�����*************/
	//˵����Ӧ��IAP���ʱ��Ҫ�����ж�������ƫ������ͬʱҲ��Ҫ����Target�е�FLASH��ʼ��ַ��
	//������תǰ���뵽�߳�ģʽ
	//SCB->VTOR = FLASH_BASE | 0x10000;//����ƫ����
	/************�����*************/
	
	CPU_SR_ALLOC();
    
    Write_Through();                //͸д
	MPU_Memory_Protection();		//������ش洢����
    Cache_Enable();                 //��L1-Cache
    HAL_Init();				        //��ʼ��HAL��
    Stm32_Clock_Init(432,25,2,9);   //����ʱ��,216Mhz 
    delay_init(216);                //��ʱ��ʼ��
	uart_init(115200);		        //���ڳ�ʼ��
	//uart2_init(115200);
    LED_Init();                     //��ʼ��LED
    KEY_Init();                     //��ʼ������
    SDRAM_Init();                   //��ʼ��SDRAM
	my_mem_init(SRAMIN);		    //��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);		    //��ʼ���ⲿ�ڴ��
	my_mem_init(SRAMDTCM);		    //��ʼ��DTCM�ڴ��
	LCD_Init();                     //��ʼ��LCD
	OSInit(&err);		    	        //��ʼ��UCOSIII
		/**********�����***********/
	FTL_Init();                     //�ļ�ϵͳ��ʼ��
 	exfuns_init();                  //�ļ�ϵͳ����������ʼ��
	/*****************ë����******************/
	RTC_Init();
	TIM3_Init(100-1,1080-1);//�޷���֪����
	//uart3_init(38400);
	OS_CRITICAL_ENTER();	            //�����ٽ���	
	//������ʼ����
	LCD_ShowString(30,0,500,100,32,"Project Title");
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP, //����ѡ��,Ϊ�˱���������������񶼱��渡��Ĵ�����ֵ
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);      //����UCOSIII
    while(1)
    {
	} 
}

//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	
	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	  //ʹ��ʱ��Ƭ��ת���ȹ���,����Ĭ�ϵ�ʱ��Ƭ����
	OSSchedRoundRobinCfg(DEF_ENABLED,10,&err);  
#endif	
/*********************������Ϣ����**********************/
		/***********�����******************/
	//������Ϣ����DATAPACK_Msg�����ݰ�
//	OSQCreate ((OS_Q*		)&DATAPACK_Msg,	     //��Ϣ����
//                (CPU_CHAR*	)"DATAPACK Msg",	//��Ϣ��������
//                (OS_MSG_QTY	)SDCARD_TASK_Q_NUM,	//��Ϣ���г��ȣ���������Ϊ1
//                (OS_ERR*	)&err);		        //������
//	/************�����*****************/

/********************��ʱ����ʼ��*********************/
	
/*********************�����ʼ��************************/
	led_task_init(0);
	lcd_task_init(0);
	uart_task_init(0);
	gprs_task_init(0);
	/***************ë����******************************/
	//bdgps_task_init(0);
	/***********����꣨SD����ʼ��)******************/

    sdcard_task_init(0);
	/************���ݲɼ���ʼ��*********************/
		dataget_task_init(0);

/************************************************/
	OS_CRITICAL_EXIT();	//�˳��ٽ���
	OSTaskDel((OS_TCB*)0,&err);	//ɾ��start_task��������
}
