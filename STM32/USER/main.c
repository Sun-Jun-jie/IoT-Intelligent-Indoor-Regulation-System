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
// 在文件顶部添加宏定义
#define AUTO_MODE 1
#define MANUAL_MODE 0
/**********************************
函数声明
**********************************/
void Usart1_Init(unsigned int baud);			//串口1初始化
void USART1_Clear(void);									//串口1清除缓存
_Bool USART1_WaitRecive(void);						//串口1等待接收完成
void Usart2_Init(unsigned int baud);			//串口2初始化
void USART2_Clear(void);									//串口2清除缓存
_Bool USART2_WaitRecive(void);						//串口2等待接收完成
void Usart_SendString(USART_TypeDef *USARTx, char *str, int len);		//发送数据
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);		//发送数据

void Display_function(void);					//显示函数
void Manage_function(void);						//处理函数
/**********************************
变量定义
**********************************/
int count = 0;

char usart1_buf[256] = {0};											//串口1接收数组
int usart1_cnt = 0, usart1_cntPre = 0;

char usart2_buf[256];											//串口2接收数组
int usart2_cnt = 0, usart2_cntPre = 0;

float received_temp = 0.0;								//接温度

int  temp_high = 0;										    //温度整数
int  temp_low = 0;										    //温度小数
int hum = 0;
int light = 0;
int co2 = 0;
int smoke = 0;

// 设备状态变量（在文件顶部添加）
int acStatus = 0;          // 0=关, 1=制冷, 2=制热
int buzzerStatus = 0;
int relayStatus = 0;
int humStatus = 0;
int lightStatus = 0;
int modeStatus = 0; //1自动 0手动

char display_buf[32];									//显示缓存区

int value = 0;                 //舒适度计算
/****
*******	主函数 
*****/
int main(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //配置中断优先分组
	Delay_Init();	    	 								//延时初始化	  
	Gpio_Init();		  									//IO初始化
	Oled_Init();		  									//OLED初始化
	Oled_Clear_All();										//清屏
	Usart1_Init(115200);    							//串口1初始化
	//Usart2_Init(9600);    							//串口2初始化
	TIM3_Init(99,7199);									//定时器初始化函数 10ms
	
  Delay_ms(1000);
	Delay_ms(1000);

	
	while(1)
	{

		Display_function();								//显示函数

		Manage_function();								//处理函数
	}
}


/****
*******显示函数
*****/
void Display_function(void)
{

		Oled_ShowCHinese(1, 0, "温度:");
		sprintf(display_buf,"%d.%dC  ",temp_high,temp_low);				//显示温度的值
		Oled_ShowString(1, 6, display_buf);
		
		Oled_ShowString(2, 0, "Hum:");
		sprintf(display_buf,"%d%%  ",hum);				//显示湿度的值
		Oled_ShowString(2, 6, display_buf);
		
		Oled_ShowCHinese(3, 0, "光照:");
		sprintf(display_buf,"%dlux  ",light);				//显示光照的值
		Oled_ShowString(3, 6, display_buf);
		
		Oled_ShowString(4, 0, "CO2:");
		sprintf(display_buf,"%dppm  ",co2);				//显示二氧化碳的值
		Oled_ShowString(4, 6, display_buf);

}

/****
*******处理函数
*****/

// 修改Manage_function函数
void Manage_function(void)
{
    // 在主循环中检查接收完成
     if(USART1_WaitRecive() == 0)
    {
			//Usart_SendString(USART1,usart1_buf, strlen(usart1_buf));
      if(strstr((char*)usart1_buf, "DA") != NULL)  // 传感器数据
      {
					// 使用sscanf一次性解析所有数据
					if(sscanf((char*)usart1_buf, "DA:T%fH%dL%dC%dS%d", 
										&received_temp, &hum, &light, &co2, &smoke) == 5)
					{
							// 成功解析，更新温度显示变量
							temp_high = (int)received_temp;
							temp_low = (int)((received_temp - temp_high) * 10);
					}

			}
			else
			{
				if(strstr(usart1_buf, "9101") != NULL)  // 风扇开
				{
						Set_Fan_Relay(1);
						relayStatus = 1;
						Usart_SendString(USART1, "9101\r\n", 6);
				}
				if(strstr(usart1_buf, "9100") != NULL)  // 风扇关
				{
						Set_Fan_Relay(0);
						relayStatus = 0;
						Usart_SendString(USART1, "9100\r\n", 6);
				}
				if(strstr(usart1_buf, "201") != NULL)  // 加湿器开
				{
						Set_Humidifier(1);
						humStatus = 1;
						Usart_SendString(USART1, "201\r\n", 5);
				}
				if(strstr(usart1_buf, "200") != NULL)  // 加湿器关
				{
						Set_Humidifier(0);
						humStatus = 0;
						Usart_SendString(USART1, "200\r\n", 5);
				}
				if(strstr(usart1_buf, "301") != NULL)  // 自动模式
				{
						modeStatus = 1;
						Usart_SendString(USART1, "301\r\n", 5);
				}
				if(strstr(usart1_buf, "300") != NULL)  // 手动模式
				{
						modeStatus = 0;
						Usart_SendString(USART1, "300\r\n", 5);
				}
				if(strstr(usart1_buf, "9401") != NULL)  // 补光灯开
				{
						Set_Light(1);
						lightStatus = 1;
						Usart_SendString(USART1, "9401\r\n", 6);
				}
				if(strstr(usart1_buf, "9400") != NULL)  // 补光灯关
				{
						Set_Light(0);
						lightStatus = 0;
						Usart_SendString(USART1, "9400\r\n", 6);
				}
				if(strstr(usart1_buf, "501") != NULL)  // 蜂鸣器开
				{
						Set_Buzzer(1);
						buzzerStatus = 1;
						Usart_SendString(USART1, "501\r\n", 5);
				}
				if(strstr(usart1_buf, "500") != NULL)  // 蜂鸣器关
				{
						Set_Buzzer(0);
						buzzerStatus = 0;
						Usart_SendString(USART1, "500\r\n", 5);
				}
				if(strstr(usart1_buf, "600") != NULL)  // 空调全关
				{
						Set_AC_Cool(0);
						Set_AC_Heat(0);
						acStatus = 0;
						Usart_SendString(USART1, "600\r\n", 5);
				}
				if(strstr(usart1_buf, "601") != NULL)  // 空调制冷开
				{
						Set_AC_Cool(1);
						Set_AC_Heat(0);  // 确保制热关闭
						acStatus = 1;
						Usart_SendString(USART1, "601\r\n", 5);
				}
				if(strstr(usart1_buf, "602") != NULL)  // 空调制热开
				{
						Set_AC_Cool(0);  // 确保制冷关闭
						Set_AC_Heat(1);
						acStatus = 2;
						Usart_SendString(USART1, "602\r\n", 5);
				}
			}
      
      
      // 清空缓冲区
			memset(usart1_buf,0x00,sizeof(usart1_buf));
      USART1_Clear();
    }
    
    // 自动模式处理
    if(modeStatus == AUTO_MODE) {
        handle_auto_mode();
    }
}

// 添加自动模式处理函数
void handle_auto_mode(void)
{
    // 温度控制
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
    
    // 湿度控制
    if(hum < 50) {
        Set_Humidifier(1);
        humStatus = 1;
    } 
    else if(hum > 60) {
        Set_Humidifier(0);
        humStatus = 0;
    }
    
    // 光照控制
    if(light < 100) {
        Set_Light(1);
        lightStatus = 1;
    } 
    else {
        Set_Light(0);
        lightStatus = 0;
    }
    
    // 烟雾报警
    if(smoke == 1) {
        Set_Buzzer(1);
        buzzerStatus = 1;
    }
    else {
        Set_Buzzer(0);
        buzzerStatus = 0;
    }
		// 排风扇控制
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
*	函数名称：	Usart1_Init
*
*	函数功能：	串口1初始化
*
*	入口参数：	baud：设定的波特率
*
*	返回参数：	无
*
*	说明：			TX-PA9		RX-PA10
************************************************************
*/
void Usart1_Init(unsigned int baud)
{
	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);														//GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);													//使能USART1时钟
	
	//USART1 PA9	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;																			//复用推挽输出
	gpioInitStruct.GPIO_Pin = GPIO_Pin_9;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//USART1 PA10	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;																//浮空输入
	gpioInitStruct.GPIO_Pin = GPIO_Pin_10;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//Usart1 NVIC 配置
	nvicInitStruct.NVIC_IRQChannel = USART1_IRQn;																		//串口1中断
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;																			//IRQ通道使能
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;														//抢占优先级0
	nvicInitStruct.NVIC_IRQChannelSubPriority = 3;																	//子优先级3
	NVIC_Init(&nvicInitStruct);																											//根据指定的参数初始化VIC寄存器
	
	//USART1 初始化设置
	usartInitStruct.USART_BaudRate = baud;																					//串口波特率
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;			//无硬件数据流控制
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;											//接收和发送模式
	usartInitStruct.USART_Parity = USART_Parity_No;																	//无奇偶校验位
	usartInitStruct.USART_StopBits = USART_StopBits_1;															//1位停止位
	usartInitStruct.USART_WordLength = USART_WordLength_8b;													//8位数据位
	
	USART_Init(USART1, &usartInitStruct); 																					//初始化串口1
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);																	//使能接收中断
	
	USART_Cmd(USART1, ENABLE);																											//使能串口
}

/*
************************************************************
*	函数名称：	Usart2_Init
*
*	函数功能：	串口2初始化
*
*	入口参数：	baud：设定的波特率
*
*	返回参数：	无
*
*	说明：			TX-PA2		RX-PA3
************************************************************
*/
void Usart2_Init(unsigned int baud)
{
	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);														//GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);													//使能USART2时钟
	
	//USART2 PA2	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;																			//复用推挽输出
	gpioInitStruct.GPIO_Pin = GPIO_Pin_2;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//USART2 PA3	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;																//浮空输入
	gpioInitStruct.GPIO_Pin = GPIO_Pin_3;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//Usart2 NVIC 配置
	nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;																		//串口2中断
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;																			//IRQ通道使能
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;														//抢占优先级1
	nvicInitStruct.NVIC_IRQChannelSubPriority = 2;																	//子优先级3
	NVIC_Init(&nvicInitStruct);																											//根据指定的参数初始化VIC寄存器
	
	//USART1 初始化设置
	usartInitStruct.USART_BaudRate = baud;																					//串口波特率
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;			//无硬件数据流控制
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;											//接收和发送模式
	usartInitStruct.USART_Parity = USART_Parity_No;																	//无奇偶校验位
	usartInitStruct.USART_StopBits = USART_StopBits_1;															//1位停止位
	usartInitStruct.USART_WordLength = USART_WordLength_8b;													//8位数据位
	
	USART_Init(USART2, &usartInitStruct); 																					//初始化串口2
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);																	//使能串口2接收中断
	
	USART_Cmd(USART2, ENABLE);																											//使能串口2
}

/*
************************************************************
*	函数名称：	Usart_SendString
*
*	函数功能：	串口数据发送
*
*	入口参数：	USARTx：串口组
*				str：要发送的数据
*				len：数据长度
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Usart_SendString(USART_TypeDef *USARTx, char *str, int len)
{
	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);									//发送数据
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待发送完成
	}

}

/*
************************************************************
*	函数名称：	UsartPrintf
*
*	函数功能：	格式化打印
*
*	入口参数：	USARTx：串口组
*				fmt：不定长参
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{
	unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	
	va_start(ap, fmt);
	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//格式化
	va_end(ap);
	
	while(*pStr != 0)
	{
		USART_SendData(USARTx, *pStr++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	}
}

/*
************************************************************
*	函数名称：	USART1_Clear
//
*	函数功能：	清空缓存
//
*	入口参数：	无
//
*	返回参数：	无
//
*	说明：		
************************************************************
*/
void USART1_Clear(void)
{
	memset(usart1_buf, 0, sizeof(usart1_buf));
	usart1_cnt = 0;
}

/*
************************************************************
*	函数名称：	USART1_WaitRecive
//
*	函数功能：	等待接收完成
//
*	入口参数：	无
//
*	返回参数：	0-接收完成		1-接收超时未完成
//
*	说明：		循环调用检测是否接收完成
************************************************************
*/
_Bool USART1_WaitRecive(void)
{
	if(usart1_cnt == 0) 								//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return 1;
		
	if(usart1_cnt == usart1_cntPre)			//如果上一次的值和这次相同，则说明接收完毕
	{
		usart1_cnt = 0;										//清0接收计数
			
		return 0;													//返回接收完成标志
	}
		
	usart1_cntPre = usart1_cnt;					//置为相同
	
	return 1;														//返回接收未完成标志
}

/*
************************************************************
*	函数名称：	USART2_Clear
//
*	函数功能：	清空缓存
//
*	入口参数：	无
//
*	返回参数：	无
//
*	说明：		
************************************************************
*/
void USART2_Clear(void)
{
	memset(usart2_buf, 0, sizeof(usart2_buf));
	usart2_cnt = 0;
}

/*
************************************************************
*	函数名称：	USART2_WaitRecive
//
*	函数功能：	等待接收完成
//
*	入口参数：	无
//
*	返回参数：	0-接收完成		1-接收超时未完成
//
*	说明：		循环调用检测是否接收完成
************************************************************
*/
_Bool USART2_WaitRecive(void)
{

	if(usart2_cnt == 0) 								//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return 1;
		
	if(usart2_cnt == usart2_cntPre)			//如果上一次的值和这次相同，则说明接收完毕
	{
		usart2_cnt = 0;										//清0接收计数
			
		return 0;													//返回接收完成标志
	}
		
	usart2_cntPre = usart2_cnt;					//置为相同
	
	return 1;														//返回接收未完成标志
}


/*
************************************************************
*	函数名称：	USART1_IRQHandler
*
*	函数功能：	串口1收发中断
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 		//接收中断
	{
		if(usart1_cnt >= sizeof(usart1_buf))	usart1_cnt = 0; 	//防止串口被刷爆
		usart1_buf[usart1_cnt++] = USART1->DR;
		
		USART_ClearFlag(USART1, USART_FLAG_RXNE);
	}
}

/*
************************************************************
*	函数名称：	USART1_IRQHandler
*
*	函数功能：	串口2收发中断
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 		//接收中断
	{
		if(usart2_cnt >= sizeof(usart2_buf))	usart2_cnt = 0; 	//防止串口被刷爆
		usart2_buf[usart2_cnt++] = USART2->DR;
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}
}
