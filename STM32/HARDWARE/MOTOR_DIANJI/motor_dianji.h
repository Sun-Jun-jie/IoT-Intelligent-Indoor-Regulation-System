#ifndef __MOTOR_DIANJI_H__
#define __MOTOR_DIANJI_H__


/**********************************
包含头文件
**********************************/
#include "sys.h"


/**********************************
PIN口定义
**********************************/
#define MOTOR_DIANJI_TIM_CLK_ENABLE          RCC_APB1Periph_TIM3				//定时器3的PWM通道2引脚PB5,通道3引脚PB0
#define MOTOR_DIANJI_TIM             				 TIM3

#define MOTOR_DIANJI_GPIO_CLK_ENABLE         RCC_APB2Periph_GPIOA
#define MOTOR_DIANJI_PORT                    GPIOA
#define MOTOR_DIANJI_PIN                     GPIO_Pin_6
#define MOTOR_DIANJI_PIN_2                   GPIO_Pin_7


/**********************************
函数声明
**********************************/
void Motor_Dianji_Init(void);								//直流电机初始化函数


#endif

