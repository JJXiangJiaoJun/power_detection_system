#ifndef __TIME_H
#define __TIME_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//RTC��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/13 
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


typedef struct
{
	u8 year;
	u8 month;
	u8 date;
	u8 hour;
	u8 minute;
	u8 second;
	u32 musecond;
}Time_Struct;

Time_Struct getrealtime(void);
void setrealtime(u8 year,u8 month,u8 date,u8 hour,u8 minute,u8 second,u32 musecond);
#endif
