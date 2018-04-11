#include "totaltask.h"
#include "lcdshow.h"
#include "lcd.h"
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
/********************��ʾ������*************************/
/*
            x,y
            0,0                        272,0
                ***********************
                *                     *
                *                     *
                *                     *
                *                     *
                *                     *
                *                     *
                *                     *
                *                     *
                *                     *
                *                     *
                *                     *
                *                     *
                *                     *
                *                     *
                *                     *
                ***********************
           0,0                          272,480
���ֺŴ�СΪ16�������
�ܹ�������ʾ480/16 = 30��
		������ʾ272/8 = 34��
����ռ���С����Թ���28�п�����ʾ��Ϣ

*/
/****************������ƿ��������ջ����**************/
//�������:1
OS_TCB LCDTastTCB;
CPU_STK LCD_TASK_STK[LCD_STK_SIZE];
/*******************�ֲ���������***********************/
//char input[20];
/********************�ڲ�����**************************/

/********************���⺯��**************************/
char printlcd(char *str,...)
{
	OS_ERR err;
	int len;
	char *a = NULL;
	va_list arg;
	//memset(input,0,sizeof(input));
	//a = malloc(sizeof(char)*34);
	a = mymalloc(1,sizeof(char)*34);
  va_start(arg,str);
  vsprintf(a, str, arg);
  va_end(arg);
	len = strlen(a);
	OSTaskQPost((OS_TCB*	)&LCDTastTCB,	//������LCDTast������Ϣ
                    (void*		)a,
                    (OS_MSG_SIZE)len,
                    (OS_OPT		)OS_OPT_POST_FIFO,
					(OS_ERR*	)&err);
	if(err != OS_ERR_NONE)
		return 0;//ʧ�ܷ���0
	return 1;//�ɹ�����1
}

/********************��������**************************/
char lcd_task_init(void* p_arg)
{
	OS_ERR err;
	OSTaskCreate((OS_TCB 	* )&LCDTastTCB,		//������ƿ�
				 (CPU_CHAR	* )"led task", 		//��������
                 (OS_TASK_PTR )lcd_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )LCD_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&LCD_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)LCD_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)LCD_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )10,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP, //����ѡ��,Ϊ�˱���������������񶼱��渡��Ĵ�����ֵ
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	if(err != OS_ERR_NONE)
		return 0;//ʧ�ܷ���0
	return 1;//�ɹ�����1
}

void lcd_task(void* p_arg)
{
	OS_MSG_SIZE size;
	OS_ERR err; 
	int row = 0;
	char *p;
	while(1)
	{
		p=OSTaskQPend((OS_TICK		)0,
                      (OS_OPT		)OS_OPT_PEND_BLOCKING,
                      (OS_MSG_SIZE*	)&size,
                      (CPU_TS*		)0,
                      (OS_ERR*      )&err);
		if(row>=27)
		{
			//LCD_Clear(WHITE);
			LCD_Fill(0,32,271,479,WHITE);
			row = 0;
		}
		LCD_ShowString(0,32+row*16,272,16,16,p);
		myfree(1,p);
		//memset(buffer,0,sizeof(buffer));
		row++;
	}
}

char lcd_task_deinit(void* p_arg)
{
	OS_ERR err;
	LCD_Clear(WHITE);
	OSTaskDel((OS_TCB*)&LCDTastTCB,&err);
	if(err != OS_ERR_NONE)
		return 0;//ʧ�ܷ���0
	return 1;//�ɹ�����1
}
