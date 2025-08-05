#ifndef __DS18B20_H__
#define __DS18B20_H__


/**********************************
����ͷ�ļ�
**********************************/
#include "sys.h"


/**********************************
�ض���ؼ���
**********************************/
#define DS18B20_GPIO_CLK_ENABLE             			RCC_APB2Periph_GPIOB				//DS18B20����
#define DS18B20_PORT                           		GPIOB
#define DS18B20_PIN                            		GPIO_Pin_13
#define DS18B20_OUT 															PBout(13)
#define DS18B20_IN  															PBin(13)


/**********************************
��������
**********************************/
short	DS18B20_Get_Temp(void);						//DS18B20��ȡ�¶Ⱥ���
u8 DS18B20_Init(void);  								//DS18B20��ʼ������


#endif
