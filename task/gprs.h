#ifndef __GPRS_MODULE_H
#define __GPRS_MODULE_H
/******************头文件包含*************/
#include "sys.h"
#include "delay.h"
#include "includes.h"


//任务初始化函数
char gprs_task_init(void* p_arg);
//任务函数
void gprs_task(void* p_arg);
void gprs_run_task(void* p_arg);
void gprs_send_task(void* p_arg);
//任务关闭函数
char gprs_task_deinit(void* p_arg);

#endif
