#ifndef __BDGPS_MODULE_H
#define __BDGPS_MODULE_H
/******************头文件包含*************/
#include "sys.h"
#include "delay.h"
#include "includes.h"

char bdgps_task_init(void* p_arg);
void bdgps_task(void* p_arg);
char bdgps_task_deinit(void* p_arg);

typedef struct  
{										    
 	u8 year;	//年份
	u8 month;	//月份
	u8 date;	//日期
	u8 hour; 	//小时
	u8 min; 	//分钟
	u8 sec; 	//秒钟
	u8 sub;		//亚秒
}utc; 

#endif
