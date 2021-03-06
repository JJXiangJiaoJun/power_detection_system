#ifndef __TOTAL_H
#define __TOTAL_H
/******************头文件包含*************/
#include "includes.h"
#include "malloc.h"
/*****************各任务头文件************/
#include "lcdshow.h"
#include "uart.h"
#include "gprs.h"
#include "dataget.h"
/****************曾庆宏************************/
#include "sdcard.h"
/******************任务块定义*************/

//任务序号:1
//任务功能介绍:LED定时闪烁，显示系统未死机
//任务优先级:60
//任务发送消息:无
//任务接受消息：无
//任务类型(时间阻塞型/消息阻塞型):时间阻塞

//任务优先级
#define LED_TASK_PRIO		60
//任务堆栈大小	
#define LED_STK_SIZE 		512
//任务控制块
extern OS_TCB LEDTastTCB;
//任务堆栈	
extern CPU_STK LED_TASK_STK[LED_STK_SIZE];

//任务序号:2
//任务功能介绍:LCD显示模块
//任务优先级:59
//任务发送消息:无
//任务接受消息：接受LCD显示消息
//任务类型(时间阻塞型/消息阻塞型):消息阻塞（任务自带消息队列）

//任务优先级
#define LCD_TASK_PRIO		59
//任务堆栈大小	
#define LCD_STK_SIZE 		512
//任务控制块
extern OS_TCB LCDTastTCB;
//任务堆栈	
extern CPU_STK LCD_TASK_STK[LCD_STK_SIZE];

//任务序号:3
//任务功能介绍:串口数据处理模块
//任务优先级:58
//任务发送消息:无
//任务接受消息：串口中断发来的char字符
//任务类型(时间阻塞型/消息阻塞型):消息阻塞（任务自带消息队列）

//任务优先级
#define UART_TASK_PRIO		58
//任务堆栈大小	
#define UART_STK_SIZE 		512
//任务控制块
extern OS_TCB UARTTastTCB;
//任务堆栈	
extern CPU_STK UART_TASK_STK[UART_STK_SIZE];

//任务序号:4
//任务功能介绍:GPRS模块串口接收处理
//任务优先级:47
//任务发送消息:整理好的回复，相当于rfile.getline()
//任务接受消息：串口3中断发来的char字符
//任务类型(时间阻塞型/消息阻塞型):消息阻塞（任务自带消息队列）

//任务优先级
#define GPRS_TASK_PRIO		47
//任务堆栈大小	
#define GPRS_STK_SIZE 		1024
//任务控制块
extern OS_TCB GPRSTastTCB;
//任务堆栈	
extern CPU_STK GPRS_TASK_STK[GPRS_STK_SIZE];

//任务序号:5
//任务功能介绍:GPRS模块流程管理
//任务优先级:50
//任务发送消息:发往gprs数据发送模块
//任务接受消息：任务4发来的整理好的单行信息
//任务类型(时间阻塞型/消息阻塞型):时间/消息混合阻塞

//任务优先级
#define GPRSRUN_TASK_PRIO		49
//任务堆栈大小	
#define GPRSRUN_STK_SIZE 		2048
//任务控制块
extern OS_TCB GPRSRUNTastTCB;
//任务堆栈	
extern CPU_STK GPRSRUN_TASK_STK[GPRSRUN_STK_SIZE];

//任务序号:6
//任务功能介绍:GPRS模块数据发送
//任务优先级:48
//任务发送消息:串口发送给gprs模块。
//任务接受消息：通过初始化接收不同的发送字符串
//任务类型(时间阻塞型/消息阻塞型):时间阻塞

//任务优先级
#define GPRSSEND_TASK_PRIO		48
//任务堆栈大小	
#define GPRSSEND_STK_SIZE 		1024
//任务控制块
extern OS_TCB GPRSSENDTastTCB;
//任务堆栈	
extern CPU_STK GPRSSEND_TASK_STK[GPRSSEND_STK_SIZE];


/****************曾庆宏************************/


/**********SD卡初始化任务*****************/
//任务序号:7
//任务功能介绍:SD卡
//任务优先级:50
//任务发送消息:串口发送给gprs模块。
//任务接受消息：通过初始化接收不同的发送字符串
//任务类型(时间阻塞型/消息阻塞型):时间阻塞

//任务优先级
#define SDCARD_TASK_PRIO  40
//任务堆栈大小
#define  SDCARD_STK_SIZE  4096
//任务控制块
extern OS_TCB   SDcardTask_TCB;
//任务堆栈
extern CPU_STK  SDCARD_TASK_STK[SDCARD_STK_SIZE];
//消息队列的长度
#define SDCARD_TASK_Q_NUM  4
//extern OS_Q DATAPACK_Msg;   //定义一个消息队列，用于按键消息传递，模拟消息邮�


/************************毛鑫鹏****************************/
//任务序号:8
//任务功能介绍:GPRS模块数据发送
//任务优先级:51
//任务发送消息:串口发送给gprs模块。
//任务接受消息：通过初始化接收不同的发送字符串
//任务类型(时间阻塞型/消息阻塞型):时间阻塞

//任务优先级
#define BDGPS_TASK_PRIO		51
//任务堆栈大小	
#define BDGPS_STK_SIZE 		512*4
//任务控制块
extern OS_TCB BDGPSTastTCB;
//任务堆栈	
extern CPU_STK BDGPS_TASK_STK[BDGPS_STK_SIZE];

/**************************数据采集************************/
//任务序号:9
//任务功能介绍:ATT7022E数据采集
//任务优先级:10
//任务发送消息:发送数据给GPRS模块与SDcard模块
//任务接受消息：不接受消息。通过RTC读取时间
//任务类型(时间阻塞型/消息阻塞型):时间阻塞
//任务优先级
#define DATAGET_TASK_PRIO		10
//任务堆栈大小	
#define DATAGET_STK_SIZE 		4096*2
//任务控制块
extern OS_TCB DATAGETTastTCB;
//任务堆栈	
extern CPU_STK DATAGET_TASK_STK[DATAGET_STK_SIZE];

/******************消息量定义*************/
extern OS_SEM SDcardWrite;
#endif
