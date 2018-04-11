#ifndef __SDCARD_H
#define __SDCARD_H
/******************ͷ�ļ�����*************/
#include "sys.h"
#include "delay.h"
#include "includes.h"

#include "exfuns.h"
#include "ff.h"
#include "ftl.h"

typedef struct{

  u8  year;
  u8  month;
  u8  day;	
  u8  hour;
  u8  min;
  u8  sec; 		
  float  voltage;
  float  circuit;
  float  frequency;
  float  apparentoutput;  	//���ڹ���
  float  activepower;       //�й�����
  float  reactivepower;	    //�޹�����
  float  powerfactor;       //��������
  float  distortion_voltage; //��ѹ�ܻ����� 
}DATAPACKET_TypeDef;


/************���ú���***************/

char sdcard_task_init(void *p_arg);
void sdcard_task(void *p_arg);

/***********************************/
#endif


