#include "dataget.h"
#include "totaltask.h"
#include "ATT7022E.h"
#include "bdgps.h"
/****************任务控制块与任务堆栈定义**************/
//任务控制块
OS_TCB DATAGETTastTCB;
//任务堆栈	
CPU_STK DATAGET_TASK_STK[DATAGET_STK_SIZE];
/**********************内部函数*************************/



/**********************************外部函数*************************************/

//dataget初始化函数
static void init()
{
	GPIO_InitTypeDef GPIO_Initure;
	GPIO_Initure.Pin=GPIO_PIN_1;			  //PB0
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_OD;		              //复用推挽输出
	GPIO_Initure.Pull=GPIO_NOPULL;			              //上拉
	GPIO_Initure.Speed=GPIO_SPEED_FAST;		              //高速
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);	            	  //初始化PB1
}
static void open()
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,1);
}
static void close()
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,0);
}
char dataget_task_init(void* p_arg)
{
	OS_ERR err;
	
	OSTaskCreate((OS_TCB 	* )&DATAGETTastTCB,		//任务控制块
				 (CPU_CHAR	* )"dataget task", 		//任务名字
                 (OS_TASK_PTR )dataget_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )DATAGET_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&DATAGET_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)DATAGET_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)DATAGET_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )10,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP, //任务选项,为了保险起见，所有任务都保存浮点寄存器的值
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	if(err != OS_ERR_NONE)
		return 0;//失败返回0
	return 1;//成功返回1
}


static void sendToSdcard()
{
	OS_ERR err;
	OSSemPost(&SDcardWrite,OS_OPT_POST_1,&err);
}
void dataget_task(void* p_arg)
{
	OS_ERR err;
	init();
	do
	{
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
		//检查初始化是否成功
		InitAmmeter();
		printlcd("try init att7022e");
	}
	while(!ATT7022E_Check());
	printlcd("att7022e ok");
	while(1)
	{
		bdgps_task_deinit(0);
		//close();
		//OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
		//OSSchedLock(&err);
		if(ReadAmmeterData())
		//if(ReadSend_ALL_AmmeterData())
		{
			//OSSchedUnlock(&err);	
			ReadSend_ALL_AmmeterData();
			printlcd("att7022e read ok");
			//OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err);
			printlcd("send to SDCard");
			//OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);
			//ReadSend_ALL_AmmeterData();
			sendToSdcard();
		}
		else
		{
			//OSSchedUnlock(&err);	
			printlcd("att7022e read wrong");
		}
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
		bdgps_task_init(0);
		OSTimeDlyHMSM(0,0,19,0,OS_OPT_TIME_HMSM_STRICT,&err);
		//open();	
	}
}
char dataget_task_deinit(void* p_arg)
{
	/************错误处理需改进*********/
	OS_ERR err;
	OSTaskDel((OS_TCB*)&DATAGETTastTCB,&err);
	if(err != OS_ERR_NONE)
		return 0;//失败返回0
	return 1;//成功返回1
}
