#include "gpio.h"
#include "stm32f10x.h"

// ���Ŷ��� (����ʵ�ʵ�·����)
#define AC_COOL_PIN     GPIO_Pin_0    // PA0: �յ�����
#define AC_HEAT_PIN     GPIO_Pin_1    // PA1: �յ�����
#define BUZZER_PIN      GPIO_Pin_4    // PA4: ������
#define FAN_RELAY_PIN   GPIO_Pin_5    // PA5: ���ȼ̵���
#define HUMIDIFIER_PIN  GPIO_Pin_2    // PA2: ��ʪ��
#define LIGHT_PIN       GPIO_Pin_3    // PA3: �����



#define GPIO_PORT       GPIOA         // �����豸ʹ��GPIOA

/****
*******	GPIO��ʼ��
*****/    
void Gpio_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 1. ʹ��GPIOAʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 2. ������������Ϊ�������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = AC_COOL_PIN | AC_HEAT_PIN | HUMIDIFIER_PIN | 
                                 LIGHT_PIN | BUZZER_PIN | FAN_RELAY_PIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 3. ��ʼ״̬ȫ���õ� (�ߵ�ƽ�����豸Ĭ�Ϲر�)
    GPIO_ResetBits(GPIOA, GPIO_InitStructure.GPIO_Pin);
}

/******************** ֱ��GPIO���ƺ��� ********************/
// ���ÿյ�����״̬ (1=�ߵ�ƽ/����, 0=�͵�ƽ/�ر�)
void Set_AC_Cool(uint8_t state)
{
    if(state) GPIO_SetBits(GPIO_PORT, AC_COOL_PIN);
    else GPIO_ResetBits(GPIO_PORT, AC_COOL_PIN);
}

// ���ÿյ�����״̬
void Set_AC_Heat(uint8_t state)
{
    if(state) GPIO_SetBits(GPIO_PORT, AC_HEAT_PIN);
    else GPIO_ResetBits(GPIO_PORT, AC_HEAT_PIN);
}

// ���ü�ʪ��״̬
void Set_Humidifier(uint8_t state)
{
    if(state) GPIO_SetBits(GPIO_PORT, HUMIDIFIER_PIN);
    else GPIO_ResetBits(GPIO_PORT, HUMIDIFIER_PIN);
}

// ���ò����״̬
void Set_Light(uint8_t state)
{
    if(state) GPIO_SetBits(GPIO_PORT, LIGHT_PIN);
    else GPIO_ResetBits(GPIO_PORT, LIGHT_PIN);
}

// ���÷�����״̬
void Set_Buzzer(uint8_t state)
{
    if(state) GPIO_SetBits(GPIO_PORT, BUZZER_PIN);
    else GPIO_ResetBits(GPIO_PORT, BUZZER_PIN);
}

// ���÷��ȼ̵���״̬
void Set_Fan_Relay(uint8_t state)
{
    if(state) GPIO_SetBits(GPIO_PORT, FAN_RELAY_PIN);
    else GPIO_ResetBits(GPIO_PORT, FAN_RELAY_PIN);
}

/******************** ������������ ********************/
// �ر������豸
void All_Off(void)
{
    GPIO_ResetBits(GPIO_PORT, AC_COOL_PIN | AC_HEAT_PIN | HUMIDIFIER_PIN | 
                  LIGHT_PIN | BUZZER_PIN | FAN_RELAY_PIN);
}

// ��ȡ��ǰ�����豸״̬ (��λ����)
uint8_t Get_All_States(void)
{
    uint16_t pinState = GPIO_ReadOutputData(GPIO_PORT);
    return (uint8_t)((pinState >> 0) & 0x3F); // ��ȡ��6λ״̬
}
