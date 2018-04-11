#ifndef __UART_MODULE_H
#define __UART_MODULE_H
/******************头文件包含*************/
#include "sys.h"
#include "delay.h"
#include "includes.h"


//任务初始化函数
char uart_task_init(void* p_arg);
//任务函数
void uart_task(void* p_arg);
//任务关闭函数
char uart_task_deinit(void* p_arg);

#endif
