#ifndef __ADC_H
#define __ADC_H


/**********************************
����ͷ�ļ�
**********************************/
#include "sys.h"


/**********************************
PIN�ڶ���
**********************************/
#define ADC1_0_GPIO_CLK_ENABLE      		RCC_APB2Periph_GPIOA				//ADC1_0����
#define ADC1_0_CHANNEL_CLK_ENABLE      	RCC_APB2Periph_ADC1
#define ADC1_0_PORT                			GPIOA
#define ADC1_0_PIN                  		GPIO_Pin_0
#define ADC1_0  												ADC1


/**********************************
��������
**********************************/
void Adc_Init(void);																								//ADC��ʼ������
u16  Get_Adc(u8 ch); 																								//���ADCֵ����
u16 Get_Adc_Average(u8 ch,u8 times); 																//���ADCƽ��ֵ����


#endif 

