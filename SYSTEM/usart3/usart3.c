#include "usart3.h"
#include "delay.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	  
#include "includes.h"	
#include "totaltask.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F7开发板
//串口3初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2016/6/17
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.0修改说明 
////////////////////////////////////////////////////////////////////////////////// 

//串口发送缓存区 	
__align(8) u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 	      //发送缓冲,最大USART3_MAX_SEND_LEN字节 	  
//串口接收缓存区 	
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 				      //接收缓冲,最大USART3_MAX_RECV_LEN个字节.

UART_HandleTypeDef UART3_Handler;                         //UART句柄
u8 aRxBuffer3[RXBUFFERSIZE3];//HAL库使用的串口接收缓冲
//初始化IO,串口3
//bound:波特率
void uart3_init(u32 bound)
{	
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_GPIOB_CLK_ENABLE();			              //使能GPIOB时钟
	__HAL_RCC_USART3_CLK_ENABLE();			              //使能USART1时钟

	//UART 初始化设置
	UART3_Handler.Instance=USART3;					      //USART3
	UART3_Handler.Init.BaudRate=bound;				      //波特率
	UART3_Handler.Init.WordLength=UART_WORDLENGTH_8B;     //字长为8位数据格式
	UART3_Handler.Init.StopBits=UART_STOPBITS_1;	      //一个停止位
	UART3_Handler.Init.Parity=UART_PARITY_NONE;		      //无奇偶校验位
	UART3_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;     //无硬件流控
	UART3_Handler.Init.Mode=UART_MODE_TX_RX;		      //收发模式
	HAL_UART_Init(&UART3_Handler);					      //HAL_UART_Init()会使能UART3	

	GPIO_Initure.Pin=GPIO_PIN_10|GPIO_PIN_11;			  //PB10、PB11
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		              //复用推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;			              //上拉
	GPIO_Initure.Speed=GPIO_SPEED_FAST;		              //高速
	GPIO_Initure.Alternate=GPIO_AF7_USART3;	              //复用为USART3
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);	            	  //初始化PB10,和PB11
	
	__HAL_UART_DISABLE_IT(&UART3_Handler,UART_IT_TC);
	__HAL_UART_ENABLE_IT(&UART3_Handler,UART_IT_RXNE);    //开启接收中断
	HAL_NVIC_EnableIRQ(USART3_IRQn);				      //使能USART1中断通道
	HAL_NVIC_SetPriority(USART3_IRQn,2,3);			      //抢占优先级2，子优先级3
	USART3_RX_STA=0;				                      //清零 
	
  
}

//蓝牙扫描下：   timer=1S
//非蓝牙扫描下： timer=10ms
//通过判断接收连续2个字符之间的时间差不大于timer来决定是不是一次连续的数据.
//如果2个字符接收间隔超过timer,则认为不是1次连续数据.也就是超过timer没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
u16 USART3_RX_STA=0;  
u8 Res3;
//串口3中断服务程序
void USART3_IRQHandler(void)
{
	OS_ERR err;
	char *p;
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntEnter();    
#endif
	if((__HAL_UART_GET_FLAG(&UART3_Handler,UART_FLAG_RXNE)!=RESET))  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		p = mymalloc(1,sizeof(char));
		HAL_UART_Receive(&UART3_Handler,&Res3,1,1000); 
		*p = Res3;
		OSTaskQPost((OS_TCB*	)&BDGPSTastTCB,	//向任务LCDTast发送消息
                    (void*		)p,
                    (OS_MSG_SIZE)1,
                    (OS_OPT		)OS_OPT_POST_FIFO,
					(OS_ERR*	)&err);
	}
	HAL_UART_IRQHandler(&UART3_Handler);	
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntExit();  											 
#endif
}

//串口3,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void u3_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);//此次发送数据的长度
	for(j=0;j<i;j++)                     //循环发送数据
	{
		while((USART3->ISR&0X40)==0);     //循环发送,直到发送完毕   
		USART3->TDR=USART3_TX_BUF[j];  
	}
}











