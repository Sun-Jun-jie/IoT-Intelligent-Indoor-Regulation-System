#ifndef __DS18B20_H__
#define __DS18B20_H__


/**********************************
包含头文件
**********************************/
#include "sys.h"


/**********************************
重定义关键词
**********************************/
#define DS18B20_GPIO_CLK_ENABLE             			RCC_APB2Periph_GPIOB				//DS18B20引脚
#define DS18B20_PORT                           		GPIOB
#define DS18B20_PIN                            		GPIO_Pin_13
#define DS18B20_OUT 															PBout(13)
#define DS18B20_IN  															PBin(13)


/**********************************
函数声明
**********************************/
short	DS18B20_Get_Temp(void);						//DS18B20读取温度函数
u8 DS18B20_Init(void);  								//DS18B20初始化函数


#endif
