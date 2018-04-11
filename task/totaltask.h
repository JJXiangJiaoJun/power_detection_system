#ifndef __TOTAL_H
#define __TOTAL_H
/******************ͷ�ļ�����*************/
#include "includes.h"
#include "malloc.h"
/*****************������ͷ�ļ�************/
#include "lcdshow.h"
#include "uart.h"
#include "gprs.h"
#include "dataget.h"
/****************�����************************/
#include "sdcard.h"
/******************����鶨��*************/

//�������:1
//�����ܽ���:LED��ʱ��˸����ʾϵͳδ����
//�������ȼ�:60
//��������Ϣ:��
//���������Ϣ����
//��������(ʱ��������/��Ϣ������):ʱ������

//�������ȼ�
#define LED_TASK_PRIO		60
//�����ջ��С	
#define LED_STK_SIZE 		512
//������ƿ�
extern OS_TCB LEDTastTCB;
//�����ջ	
extern CPU_STK LED_TASK_STK[LED_STK_SIZE];

//�������:2
//�����ܽ���:LCD��ʾģ��
//�������ȼ�:59
//��������Ϣ:��
//���������Ϣ������LCD��ʾ��Ϣ
//��������(ʱ��������/��Ϣ������):��Ϣ�����������Դ���Ϣ���У�

//�������ȼ�
#define LCD_TASK_PRIO		59
//�����ջ��С	
#define LCD_STK_SIZE 		512
//������ƿ�
extern OS_TCB LCDTastTCB;
//�����ջ	
extern CPU_STK LCD_TASK_STK[LCD_STK_SIZE];

//�������:3
//�����ܽ���:�������ݴ���ģ��
//�������ȼ�:58
//��������Ϣ:��
//���������Ϣ�������жϷ�����char�ַ�
//��������(ʱ��������/��Ϣ������):��Ϣ�����������Դ���Ϣ���У�

//�������ȼ�
#define UART_TASK_PRIO		58
//�����ջ��С	
#define UART_STK_SIZE 		512
//������ƿ�
extern OS_TCB UARTTastTCB;
//�����ջ	
extern CPU_STK UART_TASK_STK[UART_STK_SIZE];

//�������:4
//�����ܽ���:GPRSģ�鴮�ڽ��մ���
//�������ȼ�:47
//��������Ϣ:����õĻظ����൱��rfile.getline()
//���������Ϣ������3�жϷ�����char�ַ�
//��������(ʱ��������/��Ϣ������):��Ϣ�����������Դ���Ϣ���У�

//�������ȼ�
#define GPRS_TASK_PRIO		47
//�����ջ��С	
#define GPRS_STK_SIZE 		1024
//������ƿ�
extern OS_TCB GPRSTastTCB;
//�����ջ	
extern CPU_STK GPRS_TASK_STK[GPRS_STK_SIZE];

//�������:5
//�����ܽ���:GPRSģ�����̹���
//�������ȼ�:50
//��������Ϣ:����gprs���ݷ���ģ��
//���������Ϣ������4����������õĵ�����Ϣ
//��������(ʱ��������/��Ϣ������):ʱ��/��Ϣ�������

//�������ȼ�
#define GPRSRUN_TASK_PRIO		49
//�����ջ��С	
#define GPRSRUN_STK_SIZE 		2048
//������ƿ�
extern OS_TCB GPRSRUNTastTCB;
//�����ջ	
extern CPU_STK GPRSRUN_TASK_STK[GPRSRUN_STK_SIZE];

//�������:6
//�����ܽ���:GPRSģ�����ݷ���
//�������ȼ�:48
//��������Ϣ:���ڷ��͸�gprsģ�顣
//���������Ϣ��ͨ����ʼ�����ղ�ͬ�ķ����ַ���
//��������(ʱ��������/��Ϣ������):ʱ������

//�������ȼ�
#define GPRSSEND_TASK_PRIO		48
//�����ջ��С	
#define GPRSSEND_STK_SIZE 		1024
//������ƿ�
extern OS_TCB GPRSSENDTastTCB;
//�����ջ	
extern CPU_STK GPRSSEND_TASK_STK[GPRSSEND_STK_SIZE];


/****************�����************************/


/**********SD����ʼ������*****************/
//�������:7
//�����ܽ���:SD��
//�������ȼ�:50
//��������Ϣ:���ڷ��͸�gprsģ�顣
//���������Ϣ��ͨ����ʼ�����ղ�ͬ�ķ����ַ���
//��������(ʱ��������/��Ϣ������):ʱ������

//�������ȼ�
#define SDCARD_TASK_PRIO  40
//�����ջ��С
#define  SDCARD_STK_SIZE  4096
//������ƿ�
extern OS_TCB   SDcardTask_TCB;
//�����ջ
extern CPU_STK  SDCARD_TASK_STK[SDCARD_STK_SIZE];
//��Ϣ���еĳ���
#define SDCARD_TASK_Q_NUM  4
//extern OS_Q DATAPACK_Msg;   //����һ����Ϣ���У����ڰ�����Ϣ���ݣ�ģ����Ϣ���


/************************ë����****************************/
//�������:8
//�����ܽ���:GPRSģ�����ݷ���
//�������ȼ�:51
//��������Ϣ:���ڷ��͸�gprsģ�顣
//���������Ϣ��ͨ����ʼ�����ղ�ͬ�ķ����ַ���
//��������(ʱ��������/��Ϣ������):ʱ������

//�������ȼ�
#define BDGPS_TASK_PRIO		51
//�����ջ��С	
#define BDGPS_STK_SIZE 		512*4
//������ƿ�
extern OS_TCB BDGPSTastTCB;
//�����ջ	
extern CPU_STK BDGPS_TASK_STK[BDGPS_STK_SIZE];

/**************************���ݲɼ�************************/
//�������:9
//�����ܽ���:ATT7022E���ݲɼ�
//�������ȼ�:10
//��������Ϣ:�������ݸ�GPRSģ����SDcardģ��
//���������Ϣ����������Ϣ��ͨ��RTC��ȡʱ��
//��������(ʱ��������/��Ϣ������):ʱ������
//�������ȼ�
#define DATAGET_TASK_PRIO		10
//�����ջ��С	
#define DATAGET_STK_SIZE 		4096*2
//������ƿ�
extern OS_TCB DATAGETTastTCB;
//�����ջ	
extern CPU_STK DATAGET_TASK_STK[DATAGET_STK_SIZE];

/******************��Ϣ������*************/
extern OS_SEM SDcardWrite;
#endif
