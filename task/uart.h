#ifndef __UART_MODULE_H
#define __UART_MODULE_H
/******************ͷ�ļ�����*************/
#include "sys.h"
#include "delay.h"
#include "includes.h"


//�����ʼ������
char uart_task_init(void* p_arg);
//������
void uart_task(void* p_arg);
//����رպ���
char uart_task_deinit(void* p_arg);

#endif
