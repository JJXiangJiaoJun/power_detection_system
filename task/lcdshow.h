#ifndef __LCD_MODULE_H
#define __LCD_MODULE_H
/******************头文件包含*************/
#include "sys.h"
#include "delay.h"
#include "includes.h"

//屏幕打印函数
char printlcd(char *str,...);

//任务初始化函数
char lcd_task_init(void* p_arg);
//任务函数
void lcd_task(void* p_arg);
//任务关闭函数
char lcd_task_deinit(void* p_arg);

#endif
