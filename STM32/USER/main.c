#include "sys.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "delay.h"
#include "gpio.h"
#include "oled.h"

#include "timer.h"
#include <stdarg.h>
#include <stdio.h>

void handle_auto_mode(void);
// ���ļ�������Ӻ궨��
#define AUTO_MODE 1
#define MANUAL_MODE 0
/**********************************
��������
**********************************/
void Usart1_Init(unsigned int baud);			//����1��ʼ��
void USART1_Clear(void);									//����1�������
_Bool USART1_WaitRecive(void);						//����1�ȴ��������
void Usart2_Init(unsigned int baud);			//����2��ʼ��
void USART2_Clear(void);									//����2�������
_Bool USART2_WaitRecive(void);						//����2�ȴ��������
void Usart_SendString(USART_TypeDef *USARTx, char *str, int len);		//��������
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);		//��������

void Display_function(void);					//��ʾ����
void Manage_function(void);						//������
/**********************************
��������
**********************************/
int count = 0;

char usart1_buf[256] = {0};											//����1��������
int usart1_cnt = 0, usart1_cntPre = 0;

char usart2_buf[256];											//����2��������
int usart2_cnt = 0, usart2_cntPre = 0;

float received_temp = 0.0;								//���¶�

int  temp_high = 0;										    //�¶�����
int  temp_low = 0;										    //�¶�С��
int hum = 0;
int light = 0;
int co2 = 0;
int smoke = 0;

// �豸״̬���������ļ�������ӣ�
int acStatus = 0;          // 0=��, 1=����, 2=����
int buzzerStatus = 0;
int relayStatus = 0;
int humStatus = 0;
int lightStatus = 0;
int modeStatus = 0; //1�Զ� 0�ֶ�

char display_buf[32];									//��ʾ������

int value = 0;                 //���ʶȼ���
/****
*******	������ 
*****/
int main(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�����ж����ȷ���
	Delay_Init();	    	 								//��ʱ��ʼ��	  
	Gpio_Init();		  									//IO��ʼ��
	Oled_Init();		  									//OLED��ʼ��
	Oled_Clear_All();										//����
	Usart1_Init(115200);    							//����1��ʼ��
	//Usart2_Init(9600);    							//����2��ʼ��
	TIM3_Init(99,7199);									//��ʱ����ʼ������ 10ms
	
  Delay_ms(1000);
	Delay_ms(1000);

	
	while(1)
	{

		Display_function();								//��ʾ����

		Manage_function();								//������
	}
}


/****
*******��ʾ����
*****/
void Display_function(void)
{

		Oled_ShowCHinese(1, 0, "�¶�:");
		sprintf(display_buf,"%d.%dC  ",temp_high,temp_low);				//��ʾ�¶ȵ�ֵ
		Oled_ShowString(1, 6, display_buf);
		
		Oled_ShowString(2, 0, "Hum:");
		sprintf(display_buf,"%d%%  ",hum);				//��ʾʪ�ȵ�ֵ
		Oled_ShowString(2, 6, display_buf);
		
		Oled_ShowCHinese(3, 0, "����:");
		sprintf(display_buf,"%dlux  ",light);				//��ʾ���յ�ֵ
		Oled_ShowString(3, 6, display_buf);
		
		Oled_ShowString(4, 0, "CO2:");
		sprintf(display_buf,"%dppm  ",co2);				//��ʾ������̼��ֵ
		Oled_ShowString(4, 6, display_buf);

}

/****
*******������
*****/

// �޸�Manage_function����
void Manage_function(void)
{
    // ����ѭ���м��������
     if(USART1_WaitRecive() == 0)
    {
			//Usart_SendString(USART1,usart1_buf, strlen(usart1_buf));
      if(strstr((char*)usart1_buf, "DA") != NULL)  // ����������
      {
					// ʹ��sscanfһ���Խ�����������
					if(sscanf((char*)usart1_buf, "DA:T%fH%dL%dC%dS%d", 
										&received_temp, &hum, &light, &co2, &smoke) == 5)
					{
							// �ɹ������������¶���ʾ����
							temp_high = (int)received_temp;
							temp_low = (int)((received_temp - temp_high) * 10);
					}

			}
			else
			{
				if(strstr(usart1_buf, "9101") != NULL)  // ���ȿ�
				{
						Set_Fan_Relay(1);
						relayStatus = 1;
						Usart_SendString(USART1, "9101\r\n", 6);
				}
				if(strstr(usart1_buf, "9100") != NULL)  // ���ȹ�
				{
						Set_Fan_Relay(0);
						relayStatus = 0;
						Usart_SendString(USART1, "9100\r\n", 6);
				}
				if(strstr(usart1_buf, "201") != NULL)  // ��ʪ����
				{
						Set_Humidifier(1);
						humStatus = 1;
						Usart_SendString(USART1, "201\r\n", 5);
				}
				if(strstr(usart1_buf, "200") != NULL)  // ��ʪ����
				{
						Set_Humidifier(0);
						humStatus = 0;
						Usart_SendString(USART1, "200\r\n", 5);
				}
				if(strstr(usart1_buf, "301") != NULL)  // �Զ�ģʽ
				{
						modeStatus = 1;
						Usart_SendString(USART1, "301\r\n", 5);
				}
				if(strstr(usart1_buf, "300") != NULL)  // �ֶ�ģʽ
				{
						modeStatus = 0;
						Usart_SendString(USART1, "300\r\n", 5);
				}
				if(strstr(usart1_buf, "9401") != NULL)  // ����ƿ�
				{
						Set_Light(1);
						lightStatus = 1;
						Usart_SendString(USART1, "9401\r\n", 6);
				}
				if(strstr(usart1_buf, "9400") != NULL)  // ����ƹ�
				{
						Set_Light(0);
						lightStatus = 0;
						Usart_SendString(USART1, "9400\r\n", 6);
				}
				if(strstr(usart1_buf, "501") != NULL)  // ��������
				{
						Set_Buzzer(1);
						buzzerStatus = 1;
						Usart_SendString(USART1, "501\r\n", 5);
				}
				if(strstr(usart1_buf, "500") != NULL)  // ��������
				{
						Set_Buzzer(0);
						buzzerStatus = 0;
						Usart_SendString(USART1, "500\r\n", 5);
				}
				if(strstr(usart1_buf, "600") != NULL)  // �յ�ȫ��
				{
						Set_AC_Cool(0);
						Set_AC_Heat(0);
						acStatus = 0;
						Usart_SendString(USART1, "600\r\n", 5);
				}
				if(strstr(usart1_buf, "601") != NULL)  // �յ����俪
				{
						Set_AC_Cool(1);
						Set_AC_Heat(0);  // ȷ�����ȹر�
						acStatus = 1;
						Usart_SendString(USART1, "601\r\n", 5);
				}
				if(strstr(usart1_buf, "602") != NULL)  // �յ����ȿ�
				{
						Set_AC_Cool(0);  // ȷ������ر�
						Set_AC_Heat(1);
						acStatus = 2;
						Usart_SendString(USART1, "602\r\n", 5);
				}
			}
      
      
      // ��ջ�����
			memset(usart1_buf,0x00,sizeof(usart1_buf));
      USART1_Clear();
    }
    
    // �Զ�ģʽ����
    if(modeStatus == AUTO_MODE) {
        handle_auto_mode();
    }
}

// ����Զ�ģʽ������
void handle_auto_mode(void)
{
    // �¶ȿ���
    if(temp_high > 28) {
        Set_AC_Cool(1);
        Set_AC_Heat(0);
        acStatus = 1;
    } 
    else if(temp_high < 22) {
        Set_AC_Cool(0);
        Set_AC_Heat(1);
        acStatus = 2;
    }
    else {
        Set_AC_Cool(0);
        Set_AC_Heat(0);
        acStatus = 0;
    }
    
    // ʪ�ȿ���
    if(hum < 50) {
        Set_Humidifier(1);
        humStatus = 1;
    } 
    else if(hum > 60) {
        Set_Humidifier(0);
        humStatus = 0;
    }
    
    // ���տ���
    if(light < 100) {
        Set_Light(1);
        lightStatus = 1;
    } 
    else {
        Set_Light(0);
        lightStatus = 0;
    }
    
    // ������
    if(smoke == 1) {
        Set_Buzzer(1);
        buzzerStatus = 1;
    }
    else {
        Set_Buzzer(0);
        buzzerStatus = 0;
    }
		// �ŷ��ȿ���
    if(co2 > 450 && acStatus == 0) {
        Set_Fan_Relay(1);
        buzzerStatus = 1;
    }
    else {
        Set_Fan_Relay(0);
        buzzerStatus = 0;
    }
		
}

/*
************************************************************
*	�������ƣ�	Usart1_Init
*
*	�������ܣ�	����1��ʼ��
*
*	��ڲ�����	baud���趨�Ĳ�����
*
*	���ز�����	��
*
*	˵����			TX-PA9		RX-PA10
************************************************************
*/
void Usart1_Init(unsigned int baud)
{
	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);														//GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);													//ʹ��USART1ʱ��
	
	//USART1 PA9	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;																			//�����������
	gpioInitStruct.GPIO_Pin = GPIO_Pin_9;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//USART1 PA10	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;																//��������
	gpioInitStruct.GPIO_Pin = GPIO_Pin_10;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//Usart1 NVIC ����
	nvicInitStruct.NVIC_IRQChannel = USART1_IRQn;																		//����1�ж�
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;																			//IRQͨ��ʹ��
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;														//��ռ���ȼ�0
	nvicInitStruct.NVIC_IRQChannelSubPriority = 3;																	//�����ȼ�3
	NVIC_Init(&nvicInitStruct);																											//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	//USART1 ��ʼ������
	usartInitStruct.USART_BaudRate = baud;																					//���ڲ�����
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;			//��Ӳ������������
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;											//���պͷ���ģʽ
	usartInitStruct.USART_Parity = USART_Parity_No;																	//����żУ��λ
	usartInitStruct.USART_StopBits = USART_StopBits_1;															//1λֹͣλ
	usartInitStruct.USART_WordLength = USART_WordLength_8b;													//8λ����λ
	
	USART_Init(USART1, &usartInitStruct); 																					//��ʼ������1
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);																	//ʹ�ܽ����ж�
	
	USART_Cmd(USART1, ENABLE);																											//ʹ�ܴ���
}

/*
************************************************************
*	�������ƣ�	Usart2_Init
*
*	�������ܣ�	����2��ʼ��
*
*	��ڲ�����	baud���趨�Ĳ�����
*
*	���ز�����	��
*
*	˵����			TX-PA2		RX-PA3
************************************************************
*/
void Usart2_Init(unsigned int baud)
{
	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);														//GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);													//ʹ��USART2ʱ��
	
	//USART2 PA2	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;																			//�����������
	gpioInitStruct.GPIO_Pin = GPIO_Pin_2;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//USART2 PA3	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;																//��������
	gpioInitStruct.GPIO_Pin = GPIO_Pin_3;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//Usart2 NVIC ����
	nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;																		//����2�ж�
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;																			//IRQͨ��ʹ��
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;														//��ռ���ȼ�1
	nvicInitStruct.NVIC_IRQChannelSubPriority = 2;																	//�����ȼ�3
	NVIC_Init(&nvicInitStruct);																											//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	//USART1 ��ʼ������
	usartInitStruct.USART_BaudRate = baud;																					//���ڲ�����
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;			//��Ӳ������������
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;											//���պͷ���ģʽ
	usartInitStruct.USART_Parity = USART_Parity_No;																	//����żУ��λ
	usartInitStruct.USART_StopBits = USART_StopBits_1;															//1λֹͣλ
	usartInitStruct.USART_WordLength = USART_WordLength_8b;													//8λ����λ
	
	USART_Init(USART2, &usartInitStruct); 																					//��ʼ������2
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);																	//ʹ�ܴ���2�����ж�
	
	USART_Cmd(USART2, ENABLE);																											//ʹ�ܴ���2
}

/*
************************************************************
*	�������ƣ�	Usart_SendString
*
*	�������ܣ�	�������ݷ���
*
*	��ڲ�����	USARTx��������
*				str��Ҫ���͵�����
*				len�����ݳ���
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Usart_SendString(USART_TypeDef *USARTx, char *str, int len)
{
	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);									//��������
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//�ȴ��������
	}

}

/*
************************************************************
*	�������ƣ�	UsartPrintf
*
*	�������ܣ�	��ʽ����ӡ
*
*	��ڲ�����	USARTx��������
*				fmt����������
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{
	unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	
	va_start(ap, fmt);
	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//��ʽ��
	va_end(ap);
	
	while(*pStr != 0)
	{
		USART_SendData(USARTx, *pStr++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	}
}

/*
************************************************************
*	�������ƣ�	USART1_Clear
//
*	�������ܣ�	��ջ���
//
*	��ڲ�����	��
//
*	���ز�����	��
//
*	˵����		
************************************************************
*/
void USART1_Clear(void)
{
	memset(usart1_buf, 0, sizeof(usart1_buf));
	usart1_cnt = 0;
}

/*
************************************************************
*	�������ƣ�	USART1_WaitRecive
//
*	�������ܣ�	�ȴ��������
//
*	��ڲ�����	��
//
*	���ز�����	0-�������		1-���ճ�ʱδ���
//
*	˵����		ѭ�����ü���Ƿ�������
************************************************************
*/
_Bool USART1_WaitRecive(void)
{
	if(usart1_cnt == 0) 								//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return 1;
		
	if(usart1_cnt == usart1_cntPre)			//�����һ�ε�ֵ�������ͬ����˵���������
	{
		usart1_cnt = 0;										//��0���ռ���
			
		return 0;													//���ؽ�����ɱ�־
	}
		
	usart1_cntPre = usart1_cnt;					//��Ϊ��ͬ
	
	return 1;														//���ؽ���δ��ɱ�־
}

/*
************************************************************
*	�������ƣ�	USART2_Clear
//
*	�������ܣ�	��ջ���
//
*	��ڲ�����	��
//
*	���ز�����	��
//
*	˵����		
************************************************************
*/
void USART2_Clear(void)
{
	memset(usart2_buf, 0, sizeof(usart2_buf));
	usart2_cnt = 0;
}

/*
************************************************************
*	�������ƣ�	USART2_WaitRecive
//
*	�������ܣ�	�ȴ��������
//
*	��ڲ�����	��
//
*	���ز�����	0-�������		1-���ճ�ʱδ���
//
*	˵����		ѭ�����ü���Ƿ�������
************************************************************
*/
_Bool USART2_WaitRecive(void)
{

	if(usart2_cnt == 0) 								//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return 1;
		
	if(usart2_cnt == usart2_cntPre)			//�����һ�ε�ֵ�������ͬ����˵���������
	{
		usart2_cnt = 0;										//��0���ռ���
			
		return 0;													//���ؽ�����ɱ�־
	}
		
	usart2_cntPre = usart2_cnt;					//��Ϊ��ͬ
	
	return 1;														//���ؽ���δ��ɱ�־
}


/*
************************************************************
*	�������ƣ�	USART1_IRQHandler
*
*	�������ܣ�	����1�շ��ж�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 		//�����ж�
	{
		if(usart1_cnt >= sizeof(usart1_buf))	usart1_cnt = 0; 	//��ֹ���ڱ�ˢ��
		usart1_buf[usart1_cnt++] = USART1->DR;
		
		USART_ClearFlag(USART1, USART_FLAG_RXNE);
	}
}

/*
************************************************************
*	�������ƣ�	USART1_IRQHandler
*
*	�������ܣ�	����2�շ��ж�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 		//�����ж�
	{
		if(usart2_cnt >= sizeof(usart2_buf))	usart2_cnt = 0; 	//��ֹ���ڱ�ˢ��
		usart2_buf[usart2_cnt++] = USART2->DR;
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}
}
