#include "totaltask.h"
#include "lcdshow.h"
#include "lcd.h"
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
/********************显示屏介绍*************************/
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
在字号大小为16的情况下
总共可以显示480/16 = 30行
		可以显示272/8 = 34列
标题占两列。所以共有28列可以显示信息

*/
/****************任务控制块与任务堆栈定义**************/
//任务序号:1
OS_TCB LCDTastTCB;
CPU_STK LCD_TASK_STK[LCD_STK_SIZE];
/*******************局部变量定义***********************/
//char input[20];
/********************内部函数**************************/

/********************对外函数**************************/
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
	OSTaskQPost((OS_TCB*	)&LCDTastTCB,	//向任务LCDTast发送消息
                    (void*		)a,
                    (OS_MSG_SIZE)len,
                    (OS_OPT		)OS_OPT_POST_FIFO,
					(OS_ERR*	)&err);
	if(err != OS_ERR_NONE)
		return 0;//失败返回0
	return 1;//成功返回1
}

/********************函数部分**************************/
char lcd_task_init(void* p_arg)
{
	OS_ERR err;
	OSTaskCreate((OS_TCB 	* )&LCDTastTCB,		//任务控制块
				 (CPU_CHAR	* )"led task", 		//任务名字
                 (OS_TASK_PTR )lcd_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )LCD_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&LCD_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)LCD_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)LCD_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )10,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP, //任务选项,为了保险起见，所有任务都保存浮点寄存器的值
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	if(err != OS_ERR_NONE)
		return 0;//失败返回0
	return 1;//成功返回1
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
		return 0;//失败返回0
	return 1;//成功返回1
}
