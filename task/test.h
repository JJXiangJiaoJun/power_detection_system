#ifndef __TEST_MODULE_H
#define __TEST_MODULE_H
/******************ͷ�ļ�����*************/
#include "sys.h"
#include "delay.h"
#include "includes.h"


//�����ʼ������
char led_task_init(void* p_arg);
//������
void led_task(void* p_arg);
//����رպ���
char led_task_deinit(void* p_arg);

#endif
