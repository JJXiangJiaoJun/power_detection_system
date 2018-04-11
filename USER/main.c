#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "ltdc.h"
#include "sdram.h"
#include "usart2.h"
#include "mpu.h"
#include "includes.h"
#include "totaltask.h"

/************曾庆宏*************/
#include "exfuns.h"
#include "ff.h"
#include "ftl.h"
/***********毛鑫鹏*************/
#include "bdgps.h"
#include "rtc.h"
#include "timer.h"
/*****************各任务头文件******************/
#include "test.h"
#include "lcdshow.h"
/************************************************
 优先级0：中断服务服务管理任务 OS_IntQTask()
 优先级1：时钟节拍任务 OS_TickTask()
 优先级2：定时任务 OS_TmrTask()
 优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
 优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()

 共有64个优先级
************************************************/

//任务优先级
#define START_TASK_PRIO		3
//任务堆栈大小	
#define START_STK_SIZE 		128
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

////////////////////////////////////////////////////////

//LCD刷屏时使用的颜色
int lcd_discolor[14]={	WHITE, RED,   BLUE,  BRED,      
						GRED,  GBLUE, BLACK,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };


int main(void)
{
    
	 OS_ERR err;
	/************曾庆宏*************/
	//说明：应用IAP编程时需要设置中断向量表偏移量，同时也需要设置Target中的FLASH起始地址，
	//并在跳转前进入到线程模式
	//SCB->VTOR = FLASH_BASE | 0x10000;//设置偏移量
	/************曾庆宏*************/
	
	CPU_SR_ALLOC();
    
    Write_Through();                //透写
	MPU_Memory_Protection();		//保护相关存储区域
    Cache_Enable();                 //打开L1-Cache
    HAL_Init();				        //初始化HAL库
    Stm32_Clock_Init(432,25,2,9);   //设置时钟,216Mhz 
    delay_init(216);                //延时初始化
	uart_init(115200);		        //串口初始化
	//uart2_init(115200);
    LED_Init();                     //初始化LED
    KEY_Init();                     //初始化按键
    SDRAM_Init();                   //初始化SDRAM
	my_mem_init(SRAMIN);		    //初始化内部内存池
	my_mem_init(SRAMEX);		    //初始化外部内存池
	my_mem_init(SRAMDTCM);		    //初始化DTCM内存池
	LCD_Init();                     //初始化LCD
	OSInit(&err);		    	        //初始化UCOSIII
		/**********曾庆宏***********/
	FTL_Init();                     //文件系统初始化
 	exfuns_init();                  //文件系统操作函数初始化
	/*****************毛鑫鹏******************/
	RTC_Init();
	TIM3_Init(100-1,1080-1);//无法得知精度
	//uart3_init(38400);
	OS_CRITICAL_ENTER();	            //进入临界区	
	//创建开始任务
	LCD_ShowString(30,0,500,100,32,"Project Title");
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP, //任务选项,为了保险起见，所有任务都保存浮点寄存器的值
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);      //开启UCOSIII
    while(1)
    {
	} 
}

//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	
	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	  //使能时间片轮转调度功能,设置默认的时间片长度
	OSSchedRoundRobinCfg(DEF_ENABLED,10,&err);  
#endif	
/*********************创建消息队列**********************/
		/***********曾庆宏******************/
	//创建消息队列DATAPACK_Msg，数据包
//	OSQCreate ((OS_Q*		)&DATAPACK_Msg,	     //消息队列
//                (CPU_CHAR*	)"DATAPACK Msg",	//消息队列名称
//                (OS_MSG_QTY	)SDCARD_TASK_Q_NUM,	//消息队列长度，这里设置为1
//                (OS_ERR*	)&err);		        //错误码
//	/************曾庆宏*****************/

/********************软定时器初始化*********************/
	
/*********************任务初始化************************/
	led_task_init(0);
	lcd_task_init(0);
	uart_task_init(0);
	gprs_task_init(0);
	/***************毛鑫鹏******************************/
	//bdgps_task_init(0);
	/***********曾庆宏（SD卡初始化)******************/

    sdcard_task_init(0);
	/************数据采集初始化*********************/
		dataget_task_init(0);

/************************************************/
	OS_CRITICAL_EXIT();	//退出临界区
	OSTaskDel((OS_TCB*)0,&err);	//删除start_task任务自身
}
