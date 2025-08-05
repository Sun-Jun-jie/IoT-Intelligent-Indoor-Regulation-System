#include "usart.h"

//C��
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

/*
extern uint8_t usart1_buf[256];				//����1��������
extern uint8_t usart2_buf[256];				//����2��������
extern uint8_t usart3_buf[256];				//����3��������

	if(USART1_WaitRecive() == 0)
	{
		if(strstr((char*)usart1_buf,"+CMT:") != NULL)
		{

		}

		USART1_Clear();
	}
*/

/**********************************
��������
**********************************/
uint8_t usart1_buf[256];											//����1��������
uint16_t usart1_cnt = 0, usart1_cntPre = 0;

uint8_t usart2_buf[256];											//����2��������
uint16_t usart2_cnt = 0, usart2_cntPre = 0;

/*
************************************************************
*	�������ƣ�	Usart1_Init
*
*	�������ܣ�	����1��ʼ��
*
*	��ڲ�����	baud���趨�Ĳ�����
*
*	���ز�����	��
*
*	˵����			TX-PA9		RX-PA10
************************************************************
*/
void Usart1_Init(unsigned int baud)
{
	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);														//GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);													//ʹ��USART1ʱ��
	
	//USART1 PA9	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;																			//�����������
	gpioInitStruct.GPIO_Pin = GPIO_Pin_9;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//USART1 PA10	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;																//��������
	gpioInitStruct.GPIO_Pin = GPIO_Pin_10;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//Usart1 NVIC ����
	nvicInitStruct.NVIC_IRQChannel = USART1_IRQn;																		//����1�ж�
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;																			//IRQͨ��ʹ��
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;														//��ռ���ȼ�0
	nvicInitStruct.NVIC_IRQChannelSubPriority = 3;																	//�����ȼ�3
	NVIC_Init(&nvicInitStruct);																											//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	//USART1 ��ʼ������
	usartInitStruct.USART_BaudRate = baud;																					//���ڲ�����
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;			//��Ӳ������������
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;											//���պͷ���ģʽ
	usartInitStruct.USART_Parity = USART_Parity_No;																	//����żУ��λ
	usartInitStruct.USART_StopBits = USART_StopBits_1;															//1λֹͣλ
	usartInitStruct.USART_WordLength = USART_WordLength_8b;													//8λ����λ
	
	USART_Init(USART1, &usartInitStruct); 																					//��ʼ������1
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);																	//ʹ�ܽ����ж�
	
	USART_Cmd(USART1, ENABLE);																											//ʹ�ܴ���
}

/*
************************************************************
*	�������ƣ�	Usart2_Init
*
*	�������ܣ�	����2��ʼ��
*
*	��ڲ�����	baud���趨�Ĳ�����
*
*	���ز�����	��
*
*	˵����			TX-PA2		RX-PA3
************************************************************
*/
void Usart2_Init(unsigned int baud)
{
	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);														//GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);													//ʹ��USART2ʱ��
	
	//USART2 PA2	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;																			//�����������
	gpioInitStruct.GPIO_Pin = GPIO_Pin_2;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//USART2 PA3	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;																//��������
	gpioInitStruct.GPIO_Pin = GPIO_Pin_3;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//Usart2 NVIC ����
	nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;																		//����2�ж�
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;																			//IRQͨ��ʹ��
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;														//��ռ���ȼ�1
	nvicInitStruct.NVIC_IRQChannelSubPriority = 2;																	//�����ȼ�3
	NVIC_Init(&nvicInitStruct);																											//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	//USART1 ��ʼ������
	usartInitStruct.USART_BaudRate = baud;																					//���ڲ�����
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;			//��Ӳ������������
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;											//���պͷ���ģʽ
	usartInitStruct.USART_Parity = USART_Parity_No;																	//����żУ��λ
	usartInitStruct.USART_StopBits = USART_StopBits_1;															//1λֹͣλ
	usartInitStruct.USART_WordLength = USART_WordLength_8b;													//8λ����λ
	
	USART_Init(USART2, &usartInitStruct); 																					//��ʼ������2
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);																	//ʹ�ܴ���2�����ж�
	
	USART_Cmd(USART2, ENABLE);																											//ʹ�ܴ���2
}

/*
************************************************************
*	�������ƣ�	Usart_SendString
*
*	�������ܣ�	�������ݷ���
*
*	��ڲ�����	USARTx��������
*				str��Ҫ���͵�����
*				len�����ݳ���
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{
	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);									//��������
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//�ȴ��������
	}

}

/*
************************************************************
*	�������ƣ�	UsartPrintf
*
*	�������ܣ�	��ʽ����ӡ
*
*	��ڲ�����	USARTx��������
*				fmt����������
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{
	unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	
	va_start(ap, fmt);
	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//��ʽ��
	va_end(ap);
	
	while(*pStr != 0)
	{
		USART_SendData(USARTx, *pStr++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	}
}

/*
************************************************************
*	�������ƣ�	USART1_Clear
//
*	�������ܣ�	��ջ���
//
*	��ڲ�����	��
//
*	���ز�����	��
//
*	˵����		
************************************************************
*/
void USART1_Clear(void)
{
	memset(usart1_buf, 0, sizeof(usart1_buf));
	usart1_cnt = 0;
}

/*
************************************************************
*	�������ƣ�	USART1_WaitRecive
//
*	�������ܣ�	�ȴ��������
//
*	��ڲ�����	��
//
*	���ز�����	0-�������		1-���ճ�ʱδ���
//
*	˵����		ѭ�����ü���Ƿ�������
************************************************************
*/
_Bool USART1_WaitRecive(void)
{
	if(usart1_cnt == 0) 								//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return 1;
		
	if(usart1_cnt == usart1_cntPre)			//�����һ�ε�ֵ�������ͬ����˵���������
	{
		usart1_cnt = 0;										//��0���ռ���
			
		return 0;													//���ؽ�����ɱ�־
	}
		
	usart1_cntPre = usart1_cnt;					//��Ϊ��ͬ
	
	return 1;														//���ؽ���δ��ɱ�־
}

/*
************************************************************
*	�������ƣ�	USART2_Clear
//
*	�������ܣ�	��ջ���
//
*	��ڲ�����	��
//
*	���ز�����	��
//
*	˵����		
************************************************************
*/
void USART2_Clear(void)
{
	memset(usart2_buf, 0, sizeof(usart2_buf));
	usart2_cnt = 0;
}

/*
************************************************************
*	�������ƣ�	USART2_WaitRecive
//
*	�������ܣ�	�ȴ��������
//
*	��ڲ�����	��
//
*	���ز�����	0-�������		1-���ճ�ʱδ���
//
*	˵����		ѭ�����ü���Ƿ�������
************************************************************
*/
_Bool USART2_WaitRecive(void)
{

	if(usart2_cnt == 0) 								//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return 1;
		
	if(usart2_cnt == usart2_cntPre)			//�����һ�ε�ֵ�������ͬ����˵���������
	{
		usart2_cnt = 0;										//��0���ռ���
			
		return 0;													//���ؽ�����ɱ�־
	}
		
	usart2_cntPre = usart2_cnt;					//��Ϊ��ͬ
	
	return 1;														//���ؽ���δ��ɱ�־
}


/*
************************************************************
*	�������ƣ�	USART1_IRQHandler
*
*	�������ܣ�	����1�շ��ж�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 		//�����ж�
	{
		if(usart1_cnt >= sizeof(usart1_buf))	usart1_cnt = 0; 	//��ֹ���ڱ�ˢ��
		usart1_buf[usart1_cnt++] = USART1->DR;
		
		USART_ClearFlag(USART1, USART_FLAG_RXNE);
	}
}

/*
************************************************************
*	�������ƣ�	USART1_IRQHandler
*
*	�������ܣ�	����2�շ��ж�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 		//�����ж�
	{
		if(usart2_cnt >= sizeof(usart2_buf))	usart2_cnt = 0; 	//��ֹ���ڱ�ˢ��
		usart2_buf[usart2_cnt++] = USART2->DR;
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}
}


