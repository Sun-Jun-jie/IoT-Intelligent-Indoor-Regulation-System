#include "motor_dianji.h"


/*
控制count值控制电机转动速度（0-899，最快是899）
uint16_t motor_dianji_cout;			//电机pwm值

TIM_SetCompare1(TIM3,motor_dianji_cout); 		//设置定时器pwm值
*/

/****
*******直流电机初始化函数
*****/
void Motor_Dianji_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(MOTOR_DIANJI_TIM_CLK_ENABLE, ENABLE);        									//使能定时器3时钟
	RCC_APB2PeriphClockCmd(MOTOR_DIANJI_GPIO_CLK_ENABLE | RCC_APB2Periph_AFIO, ENABLE);  	//使能GPIO外设和AFIO复用功能模块时钟

	//设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形        GPIOB.5
	GPIO_InitStructure.GPIO_Pin = MOTOR_DIANJI_PIN; 						//TIM_CH
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 						//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;						//IO口速度为50MHz
	GPIO_Init(MOTOR_DIANJI_PORT, &GPIO_InitStructure);					//初始化GPIO

	GPIO_InitStructure.GPIO_Pin = MOTOR_DIANJI_PIN_2; 					//TIM_CH
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  					//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;						//IO口速度为50MHz
	GPIO_Init(MOTOR_DIANJI_PORT, &GPIO_InitStructure);					//初始化GPIO
	GPIO_ResetBits(MOTOR_DIANJI_PORT,MOTOR_DIANJI_PIN_2);

	//初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = 899; 										//设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = 0; 										//设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 								//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
	TIM_TimeBaseInit(MOTOR_DIANJI_TIM, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	//初始化TIM3 Channel2 PWM模式         
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 					//选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 	//输出极性:TIM输出比较极性高
	TIM_OC1Init(MOTOR_DIANJI_TIM, &TIM_OCInitStructure);  			//根据T指定的参数初始化外设TIM3 OC2
	TIM_OC1PreloadConfig(MOTOR_DIANJI_TIM, TIM_OCPreload_Enable); //使能TIM3在CCR2上的预装载寄存器

	TIM_Cmd(MOTOR_DIANJI_TIM, ENABLE);  												//使能TIM3
}

