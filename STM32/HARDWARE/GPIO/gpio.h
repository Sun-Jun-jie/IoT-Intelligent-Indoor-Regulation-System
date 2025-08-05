#ifndef __GPIO_H
#define __GPIO_H


/**********************************
包含头文件
**********************************/
#include "sys.h"

void Gpio_Init(void);
void Set_AC_Cool(uint8_t state);
void Set_AC_Heat(uint8_t state);
void Set_Humidifier(uint8_t state);
void Set_Light(uint8_t state);
void Set_Buzzer(uint8_t state);
void Set_Fan_Relay(uint8_t state);
void All_Off(void);
uint8_t Get_All_States(void);
	
#endif

