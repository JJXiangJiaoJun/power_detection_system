#include "usart3.h"
#include "delay.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	  
#include "includes.h"	
#include "totaltask.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//����3��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2016/6/17
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.0�޸�˵�� 
////////////////////////////////////////////////////////////////////////////////// 

//���ڷ��ͻ����� 	
__align(8) u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 	      //���ͻ���,���USART3_MAX_SEND_LEN�ֽ� 	  
//���ڽ��ջ����� 	
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 				      //���ջ���,���USART3_MAX_RECV_LEN���ֽ�.

UART_HandleTypeDef UART3_Handler;                         //UART���
u8 aRxBuffer3[RXBUFFERSIZE3];//HAL��ʹ�õĴ��ڽ��ջ���
//��ʼ��IO,����3
//bound:������
void uart3_init(u32 bound)
{	
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_GPIOB_CLK_ENABLE();			              //ʹ��GPIOBʱ��
	__HAL_RCC_USART3_CLK_ENABLE();			              //ʹ��USART1ʱ��

	//UART ��ʼ������
	UART3_Handler.Instance=USART3;					      //USART3
	UART3_Handler.Init.BaudRate=bound;				      //������
	UART3_Handler.Init.WordLength=UART_WORDLENGTH_8B;     //�ֳ�Ϊ8λ���ݸ�ʽ
	UART3_Handler.Init.StopBits=UART_STOPBITS_1;	      //һ��ֹͣλ
	UART3_Handler.Init.Parity=UART_PARITY_NONE;		      //����żУ��λ
	UART3_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;     //��Ӳ������
	UART3_Handler.Init.Mode=UART_MODE_TX_RX;		      //�շ�ģʽ
	HAL_UART_Init(&UART3_Handler);					      //HAL_UART_Init()��ʹ��UART3	

	GPIO_Initure.Pin=GPIO_PIN_10|GPIO_PIN_11;			  //PB10��PB11
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		              //�����������
	GPIO_Initure.Pull=GPIO_PULLUP;			              //����
	GPIO_Initure.Speed=GPIO_SPEED_FAST;		              //����
	GPIO_Initure.Alternate=GPIO_AF7_USART3;	              //����ΪUSART3
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);	            	  //��ʼ��PB10,��PB11
	
	__HAL_UART_DISABLE_IT(&UART3_Handler,UART_IT_TC);
	__HAL_UART_ENABLE_IT(&UART3_Handler,UART_IT_RXNE);    //���������ж�
	HAL_NVIC_EnableIRQ(USART3_IRQn);				      //ʹ��USART1�ж�ͨ��
	HAL_NVIC_SetPriority(USART3_IRQn,2,3);			      //��ռ���ȼ�2�������ȼ�3
	USART3_RX_STA=0;				                      //���� 
	
  
}

//����ɨ���£�   timer=1S
//������ɨ���£� timer=10ms
//ͨ���жϽ�������2���ַ�֮���ʱ������timer�������ǲ���һ������������.
//���2���ַ����ռ������timer,����Ϊ����1����������.Ҳ���ǳ���timerû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
u16 USART3_RX_STA=0;  
u8 Res3;
//����3�жϷ������
void USART3_IRQHandler(void)
{
	OS_ERR err;
	char *p;
#if SYSTEM_SUPPORT_OS	 	//ʹ��OS
	OSIntEnter();    
#endif
	if((__HAL_UART_GET_FLAG(&UART3_Handler,UART_FLAG_RXNE)!=RESET))  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		p = mymalloc(1,sizeof(char));
		HAL_UART_Receive(&UART3_Handler,&Res3,1,1000); 
		*p = Res3;
		OSTaskQPost((OS_TCB*	)&BDGPSTastTCB,	//������LCDTast������Ϣ
                    (void*		)p,
                    (OS_MSG_SIZE)1,
                    (OS_OPT		)OS_OPT_POST_FIFO,
					(OS_ERR*	)&err);
	}
	HAL_UART_IRQHandler(&UART3_Handler);	
#if SYSTEM_SUPPORT_OS	 	//ʹ��OS
	OSIntExit();  											 
#endif
}

//����3,printf ����
//ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
void u3_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);//�˴η������ݵĳ���
	for(j=0;j<i;j++)                     //ѭ����������
	{
		while((USART3->ISR&0X40)==0);     //ѭ������,ֱ���������   
		USART3->TDR=USART3_TX_BUF[j];  
	}
}











