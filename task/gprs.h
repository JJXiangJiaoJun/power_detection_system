#ifndef __GPRS_MODULE_H
#define __GPRS_MODULE_H
/******************ͷ�ļ�����*************/
#include "sys.h"
#include "delay.h"
#include "includes.h"


//�����ʼ������
char gprs_task_init(void* p_arg);
//������
void gprs_task(void* p_arg);
void gprs_run_task(void* p_arg);
void gprs_send_task(void* p_arg);
//����رպ���
char gprs_task_deinit(void* p_arg);

#endif
