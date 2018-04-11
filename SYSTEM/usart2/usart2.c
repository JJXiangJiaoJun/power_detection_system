#include "usart2.h"
#include "delay.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	  
#include "includes.h"	
#include "totaltask.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//����2��ʼ��		   
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
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	      //���ͻ���,���USART3_MAX_SEND_LEN�ֽ� 	  
//���ڽ��ջ����� 	
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				      //���ջ���,���USART3_MAX_RECV_LEN���ֽ�.

UART_HandleTypeDef UART2_Handler;                         //UART���
u8 aRxBuffer2[RXBUFFERSIZE2];//HAL��ʹ�õĴ��ڽ��ջ���
//��ʼ��IO,����2
//bound:������
void uart2_init(u32 bound)
{	
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_GPIOA_CLK_ENABLE();			//ʹ��GPIOAʱ��
	__HAL_RCC_USART2_CLK_ENABLE();			              //ʹ��USART1ʱ��

	//UART ��ʼ������
	UART2_Handler.Instance=USART2;					      //USART3
	UART2_Handler.Init.BaudRate=bound;				      //������
	UART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;     //�ֳ�Ϊ8λ���ݸ�ʽ
	UART2_Handler.Init.StopBits=UART_STOPBITS_1;	      //һ��ֹͣλ
	UART2_Handler.Init.Parity=UART_PARITY_NONE;		      //����żУ��λ
	UART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;     //��Ӳ������
	UART2_Handler.Init.Mode=UART_MODE_TX_RX;		      //�շ�ģʽ
	HAL_UART_Init(&UART2_Handler);					      //HAL_UART_Init()��ʹ��UART3	

	GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3; //PA2,3
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
	GPIO_Initure.Pull=GPIO_PULLUP;			//����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;		//����
	GPIO_Initure.Alternate=GPIO_AF7_USART2;	//����ΪUSART2
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA2,3
	
	__HAL_UART_DISABLE_IT(&UART2_Handler,UART_IT_TC);
	__HAL_UART_ENABLE_IT(&UART2_Handler,UART_IT_RXNE);    //���������ж�
	HAL_NVIC_EnableIRQ(USART2_IRQn);				      //ʹ��USART1�ж�ͨ��
	HAL_NVIC_SetPriority(USART2_IRQn,2,2);			      //��ռ���ȼ�2�������ȼ�3
	USART2_RX_STA=0;				                      //���� 
	
  
}

//����ɨ���£�   timer=1S
//������ɨ���£� timer=10ms
//ͨ���жϽ�������2���ַ�֮���ʱ������timer�������ǲ���һ������������.
//���2���ַ����ռ������timer,����Ϊ����1����������.Ҳ���ǳ���timerû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
u16 USART2_RX_STA=0;  
u8 Res;
//����3�жϷ������
void USART2_IRQHandler(void)
{
	OS_ERR err;
	char *p;
#if SYSTEM_SUPPORT_OS	 	//ʹ��OS
	OSIntEnter();    
#endif
	if((__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_RXNE)!=RESET))  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		p = mymalloc(1,sizeof(char));
		HAL_UART_Receive(&UART2_Handler,&Res,1,1000); 
		*p = Res;
		OSTaskQPost((OS_TCB*	)&GPRSTastTCB,	//������LCDTast������Ϣ
                    (void*		)p,
                    (OS_MSG_SIZE)1,
                    (OS_OPT		)OS_OPT_POST_FIFO,
					(OS_ERR*	)&err);
	}
	HAL_UART_IRQHandler(&UART2_Handler);	
#if SYSTEM_SUPPORT_OS	 	//ʹ��OS
	OSIntExit();  											 
#endif
}

//����2,printf ����
//ȷ��һ�η������ݲ�����USART2_MAX_SEND_LEN�ֽ�
void u2_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_TX_BUF);//�˴η������ݵĳ���
	for(j=0;j<i;j++)                     //ѭ����������
	{
		while((USART2->ISR&0X40)==0);     //ѭ������,ֱ���������   
		USART2->TDR=USART2_TX_BUF[j];  
	}
}











