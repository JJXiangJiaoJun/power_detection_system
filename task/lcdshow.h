#ifndef __LCD_MODULE_H
#define __LCD_MODULE_H
/******************ͷ�ļ�����*************/
#include "sys.h"
#include "delay.h"
#include "includes.h"

//��Ļ��ӡ����
char printlcd(char *str,...);

//�����ʼ������
char lcd_task_init(void* p_arg);
//������
void lcd_task(void* p_arg);
//����رպ���
char lcd_task_deinit(void* p_arg);

#endif
