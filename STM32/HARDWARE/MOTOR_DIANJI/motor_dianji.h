#ifndef __MOTOR_DIANJI_H__
#define __MOTOR_DIANJI_H__


/**********************************
����ͷ�ļ�
**********************************/
#include "sys.h"


/**********************************
PIN�ڶ���
**********************************/
#define MOTOR_DIANJI_TIM_CLK_ENABLE          RCC_APB1Periph_TIM3				//��ʱ��3��PWMͨ��2����PB5,ͨ��3����PB0
#define MOTOR_DIANJI_TIM             				 TIM3

#define MOTOR_DIANJI_GPIO_CLK_ENABLE         RCC_APB2Periph_GPIOA
#define MOTOR_DIANJI_PORT                    GPIOA
#define MOTOR_DIANJI_PIN                     GPIO_Pin_6
#define MOTOR_DIANJI_PIN_2                   GPIO_Pin_7


/**********************************
��������
**********************************/
void Motor_Dianji_Init(void);								//ֱ�������ʼ������


#endif

