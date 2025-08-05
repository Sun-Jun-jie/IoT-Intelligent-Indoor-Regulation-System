#ifndef _USART_H_
#define _USART_H_


/**********************************
����ͷ�ļ�
**********************************/
#include "sys.h"


/**********************************
��������
**********************************/
void Usart1_Init(unsigned int baud);			//����1��ʼ��
void USART1_Clear(void);									//����1�������
_Bool USART1_WaitRecive(void);						//����1�ȴ��������
void Usart2_Init(unsigned int baud);			//����2��ʼ��
void USART2_Clear(void);									//����2�������
_Bool USART2_WaitRecive(void);						//����2�ȴ��������
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);		//��������
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);		//��������
extern uint16_t usart1_cnt;

#endif
