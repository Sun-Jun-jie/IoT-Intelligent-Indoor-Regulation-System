#include "gpio.h"
#include "stm32f10x.h"

// 引脚定义 (根据实际电路调整)
#define AC_COOL_PIN     GPIO_Pin_0    // PA0: 空调制冷
#define AC_HEAT_PIN     GPIO_Pin_1    // PA1: 空调制热
#define BUZZER_PIN      GPIO_Pin_4    // PA4: 蜂鸣器
#define FAN_RELAY_PIN   GPIO_Pin_5    // PA5: 风扇继电器
#define HUMIDIFIER_PIN  GPIO_Pin_2    // PA2: 加湿器
#define LIGHT_PIN       GPIO_Pin_3    // PA3: 补光灯



#define GPIO_PORT       GPIOA         // 所有设备使用GPIOA

/****
*******	GPIO初始化
*****/    
void Gpio_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 1. 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 2. 配置所有引脚为推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = AC_COOL_PIN | AC_HEAT_PIN | HUMIDIFIER_PIN | 
                                 LIGHT_PIN | BUZZER_PIN | FAN_RELAY_PIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 3. 初始状态全部置低 (高电平触发设备默认关闭)
    GPIO_ResetBits(GPIOA, GPIO_InitStructure.GPIO_Pin);
}

/******************** 直接GPIO控制函数 ********************/
// 设置空调制冷状态 (1=高电平/开启, 0=低电平/关闭)
void Set_AC_Cool(uint8_t state)
{
    if(state) GPIO_SetBits(GPIO_PORT, AC_COOL_PIN);
    else GPIO_ResetBits(GPIO_PORT, AC_COOL_PIN);
}

// 设置空调制热状态
void Set_AC_Heat(uint8_t state)
{
    if(state) GPIO_SetBits(GPIO_PORT, AC_HEAT_PIN);
    else GPIO_ResetBits(GPIO_PORT, AC_HEAT_PIN);
}

// 设置加湿器状态
void Set_Humidifier(uint8_t state)
{
    if(state) GPIO_SetBits(GPIO_PORT, HUMIDIFIER_PIN);
    else GPIO_ResetBits(GPIO_PORT, HUMIDIFIER_PIN);
}

// 设置补光灯状态
void Set_Light(uint8_t state)
{
    if(state) GPIO_SetBits(GPIO_PORT, LIGHT_PIN);
    else GPIO_ResetBits(GPIO_PORT, LIGHT_PIN);
}

// 设置蜂鸣器状态
void Set_Buzzer(uint8_t state)
{
    if(state) GPIO_SetBits(GPIO_PORT, BUZZER_PIN);
    else GPIO_ResetBits(GPIO_PORT, BUZZER_PIN);
}

// 设置风扇继电器状态
void Set_Fan_Relay(uint8_t state)
{
    if(state) GPIO_SetBits(GPIO_PORT, FAN_RELAY_PIN);
    else GPIO_ResetBits(GPIO_PORT, FAN_RELAY_PIN);
}

/******************** 批量操作函数 ********************/
// 关闭所有设备
void All_Off(void)
{
    GPIO_ResetBits(GPIO_PORT, AC_COOL_PIN | AC_HEAT_PIN | HUMIDIFIER_PIN | 
                  LIGHT_PIN | BUZZER_PIN | FAN_RELAY_PIN);
}

// 获取当前所有设备状态 (按位返回)
uint8_t Get_All_States(void)
{
    uint16_t pinState = GPIO_ReadOutputData(GPIO_PORT);
    return (uint8_t)((pinState >> 0) & 0x3F); // 获取低6位状态
}
