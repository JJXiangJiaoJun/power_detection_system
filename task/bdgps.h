#ifndef __BDGPS_MODULE_H
#define __BDGPS_MODULE_H
/******************ͷ�ļ�����*************/
#include "sys.h"
#include "delay.h"
#include "includes.h"

char bdgps_task_init(void* p_arg);
void bdgps_task(void* p_arg);
char bdgps_task_deinit(void* p_arg);

typedef struct  
{										    
 	u8 year;	//���
	u8 month;	//�·�
	u8 date;	//����
	u8 hour; 	//Сʱ
	u8 min; 	//����
	u8 sec; 	//����
	u8 sub;		//����
}utc; 

#endif
