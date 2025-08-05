#include "motor_dianji.h"


/*
����countֵ���Ƶ��ת���ٶȣ�0-899�������899��
uint16_t motor_dianji_cout;			//���pwmֵ

TIM_SetCompare1(TIM3,motor_dianji_cout); 		//���ö�ʱ��pwmֵ
*/

/****
*******ֱ�������ʼ������
*****/
void Motor_Dianji_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(MOTOR_DIANJI_TIM_CLK_ENABLE, ENABLE);        									//ʹ�ܶ�ʱ��3ʱ��
	RCC_APB2PeriphClockCmd(MOTOR_DIANJI_GPIO_CLK_ENABLE | RCC_APB2Periph_AFIO, ENABLE);  	//ʹ��GPIO�����AFIO���ù���ģ��ʱ��

	//���ø�����Ϊ�����������,���TIM3 CH2��PWM���岨��        GPIOB.5
	GPIO_InitStructure.GPIO_Pin = MOTOR_DIANJI_PIN; 						//TIM_CH
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 						//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;						//IO���ٶ�Ϊ50MHz
	GPIO_Init(MOTOR_DIANJI_PORT, &GPIO_InitStructure);					//��ʼ��GPIO

	GPIO_InitStructure.GPIO_Pin = MOTOR_DIANJI_PIN_2; 					//TIM_CH
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  					//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;						//IO���ٶ�Ϊ50MHz
	GPIO_Init(MOTOR_DIANJI_PORT, &GPIO_InitStructure);					//��ʼ��GPIO
	GPIO_ResetBits(MOTOR_DIANJI_PORT,MOTOR_DIANJI_PIN_2);

	//��ʼ��TIM3
	TIM_TimeBaseStructure.TIM_Period = 899; 										//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 0; 										//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 								//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(MOTOR_DIANJI_TIM, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	//��ʼ��TIM3 Channel2 PWMģʽ         
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 					//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 	//�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(MOTOR_DIANJI_TIM, &TIM_OCInitStructure);  			//����Tָ���Ĳ�����ʼ������TIM3 OC2
	TIM_OC1PreloadConfig(MOTOR_DIANJI_TIM, TIM_OCPreload_Enable); //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���

	TIM_Cmd(MOTOR_DIANJI_TIM, ENABLE);  												//ʹ��TIM3
}

