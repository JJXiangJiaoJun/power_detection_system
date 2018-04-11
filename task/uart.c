#include "uart.h"
#include <string.h>
#include "totaltask.h"
/****************������ƿ��������ջ����**************/
//�������:1
OS_TCB UARTTastTCB;
CPU_STK UART_TASK_STK[UART_STK_SIZE];


char uart_task_init(void* p_arg)
{
	OS_ERR err;
	OSTaskCreate((OS_TCB 	* )&UARTTastTCB,		//������ƿ�
				 (CPU_CHAR	* )"led task", 		//��������
                 (OS_TASK_PTR )uart_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )UART_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&UART_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)UART_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)UART_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )10,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP, //����ѡ��,Ϊ�˱���������������񶼱��渡��Ĵ�����ֵ
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	if(err != OS_ERR_NONE)
		return 0;//ʧ�ܷ���0
	return 1;//�ɹ�����1
}

void uart_task(void* p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE size;
	int buffernum = 0;
	char buffer[50];
	char* p;
	char temp;
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
		printf("%c",temp);
		if(temp == '\n')
		{
			//printlcd("%s",buffer);
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

char uart_task_deinit(void* p_arg)
{
	OS_ERR err;
	OSTaskDel((OS_TCB*)&UARTTastTCB,&err);
	if(err != OS_ERR_NONE)
		return 0;//ʧ�ܷ���0
	return 1;//�ɹ�����1
}
