#ifndef __TEST_MODULE_H
#define __TEST_MODULE_H
/******************头文件包含*************/
#include "sys.h"
#include "delay.h"
#include "includes.h"


//任务初始化函数
char led_task_init(void* p_arg);
//任务函数
void led_task(void* p_arg);
//任务关闭函数
char led_task_deinit(void* p_arg);

#endif
