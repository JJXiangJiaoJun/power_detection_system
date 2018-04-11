#ifndef __SDCARD_H
#define __SDCARD_H
/******************头文件包含*************/
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
  float  apparentoutput;  	//视在功率
  float  activepower;       //有功功率
  float  reactivepower;	    //无功功率
  float  powerfactor;       //功率因数
  float  distortion_voltage; //电压总畸变率 
}DATAPACKET_TypeDef;


/************调用函数***************/

char sdcard_task_init(void *p_arg);
void sdcard_task(void *p_arg);

/***********************************/
#endif


