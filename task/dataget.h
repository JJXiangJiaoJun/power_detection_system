#ifndef __DATAGET_MODULE_H
#define __DATAGET_MODULE_H
/******************头文件包含*************/
#include "sys.h"
#include "delay.h"
#include "includes.h"

char dataget_task_init(void* p_arg);
void dataget_task(void* p_arg);
char dataget_task_deinit(void* p_arg);

#endif
