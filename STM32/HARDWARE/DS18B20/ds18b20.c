#include "ds18b20.h"
#include "delay.h"

/****
*******DS18B20�����������
*****/
void DS18B20_GPIO_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(DS18B20_GPIO_CLK_ENABLE, ENABLE);	 	//ʹ�ܶ˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;				 					//��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 				//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 				//IO���ٶ�Ϊ50MHz
	GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);					 			//�����趨������ʼ��
}

/****
*******DS18B20������������
*****/
void DS18B20_GPIO_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(DS18B20_GPIO_CLK_ENABLE, ENABLE);	 	//ʹ�ܶ˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;				 					//��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 					//���ó���������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 				//IO���ٶ�Ϊ50MHz
	GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);					 			//�����趨������ʼ��
}
/****
*******1.��λ����: 	�������͡�����ʱ480-960us�������Ÿߡ�����ʱ15-60us
*****/
void DS18B20_RST(void) 																				
{
	DS18B20_GPIO_OUT(); 																				//�л�Ϊ���ģʽ
	DS18B20_OUT = 0;																						//����
	Delay_us(700);
	DS18B20_OUT = 1;																						//����
	Delay_us(60);
}
/****
*******2.Ӧ�����:	����������Ϊ����ģʽ�����ж����ŵ͵�ƽʱ���Ƿ����60us��С��240us��������Ӧ����
*****/
int DS18B20_Check(void) 																			//��Ӧ1--ʧ��		��Ӧ0----�ɹ�
{	
	uint16_t retry=0;
	DS18B20_GPIO_IN();																					//�л�������ģʽ
	while(DS18B20_IN && retry<10000) 														//����һֱΪ�ߣ�δ���豸�������͡��ṩ200us�ĳ�ʱʱ��
	{
		retry++;
		Delay_us(1);
	};
	if(retry>=200) return 1;																		//��ʱ��Ϊ��Ӧ��200us��
    else retry=0;
	while(!DS18B20_IN && retry<240)															// ������Ӧ��  �ж����ŵ͵�ƽʱ���Ƿ����60us��С��240us��������Ӧ����
	{
		retry++;
		Delay_us(1);
	}
	if(retry>=240)return 1;  																		//Ӧ���ʱ�����ʧ��     
  return 0; 																								//����ɹ�������0
	
}
/****
*******��0����1����
*******����������Ϊ���ģʽ�����������͡�����ʱ2us�����������ߡ�������Ϊ����ģʽ������ʱ2us������ȡ����״̬�������ض�ȡ���	����60us
*****/
uint8_t DS18B20_Read_Bit(void) 															
{
	uint8_t data;																								//�ݴ�����
	DS18B20_GPIO_OUT();																					//�л����ģʽ
	DS18B20_OUT = 0; 																						//����
	Delay_us(2);
	DS18B20_OUT = 1;																						//�ͷ�
	DS18B20_GPIO_IN();																					//�л�����ģʽ
	Delay_us(12);
	if(DS18B20_IN)data =1 ;
	else data = 0;
	Delay_us(50);
	//printf("���ݣ�%d\r\n",data);
	return data;

}
/****
*******д1���� ��������Ϊ���ģʽ�����������͡�����ʱ2us�����������ߡ�����ʱ����60us
*****/
void DS18B20_Write_One(void) 
{
  DS18B20_GPIO_OUT();        																//SET PG11 OUTPUT;
	DS18B20_OUT = 0;																						//����2us
	Delay_us(2);                           
	DS18B20_OUT = 1;
	Delay_us(60);
}
/****
*******д0���� ��������Ϊ���ģʽ�����������͡�����ʱ60-120us�����������ߡ�����ʱ2us
*****/
void DS18B20_Write_Zero(void)  
{
	DS18B20_GPIO_OUT();        																	//SET PG11 OUTPUT;
	DS18B20_OUT = 0;																						//����2us   
	Delay_us(60);                           
	DS18B20_OUT = 1;
	Delay_us(2);
}
/****
*******��ȡһ���ֽ�8bit
*****/
uint8_t DS18B20_Read_Byte(void)   
{
	u8 i,j,dat;
	dat=0;
	for (i=1;i<=8;i++) 
	{
			j=DS18B20_Read_Bit();
			dat=(j<<7)|(dat>>1);
	}						    
  return dat;
}
/****
*******дһ���ֽ�
*****/
void DS18B20_Write_Byte(uint8_t dat)  
 {            
	u8 j;
	u8 testb;
	DS18B20_GPIO_OUT();																				//SET PA0 OUTPUT;
	for (j=1;j<=8;j++) 
	{
		testb=dat&0x01;
		dat=dat>>1;
		if (testb) 
		{
			DS18B20_OUT=0;																	// Write 1
			Delay_us(2);                            
			DS18B20_OUT=1;
			Delay_us(60);             
		}
		else 
		{
			DS18B20_OUT=0;// Write 0
			Delay_us(60);             
			DS18B20_OUT=1;
			Delay_us(2);                          
		}
   }
}
 /****
*******�����豸
*****/
void DS18B20_Start(void) 
{                                                                  
   DS18B20_RST();            																//��λ
   DS18B20_Check();    		 																	//�ȴ���Ӧ
   DS18B20_Write_Byte(0xcc);  															// ����һ�� ROM ָ��		0ccH����ROM
   DS18B20_Write_Byte(0x44);    														// ���ʹ洢��ָ��		�����¶�ת��	
}
//��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
//����1:������
//����0:����    	 
u8 DS18B20_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(DS18B20_GPIO_CLK_ENABLE, ENABLE);	//ʹ��PORTA��ʱ�� 
	
 	GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;								//PORTA0 �������
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);

 	GPIO_SetBits(DS18B20_PORT,DS18B20_PIN);    								//���1

	DS18B20_RST();

	return DS18B20_Check();
}  
// ��ȡ��������ֵ
short	DS18B20_Get_Temp(void)
{
	u8 temp;
	uint8_t TH,TL;
	short tem;
	DS18B20_Start();
	DS18B20_RST();
	DS18B20_Check();
	DS18B20_Write_Byte(0xCC); 																// ����ROM��ַ��ֱ�ӷ�������
	DS18B20_Write_Byte(0xBE); 																// ��ȡ�ݴ�����9�ֽ�����
	TL = DS18B20_Read_Byte(); 																// ��8λ
	TH = DS18B20_Read_Byte(); 																// �Ͱ�λ
	
	if(TH>7)
	{
			TH=~TH;
			TL=~TL; 
			temp=0;																								//�¶�Ϊ��  
	}else temp=1;																							//�¶�Ϊ��	  	  
	tem=TH; 																									//��ø߰�λ
	tem<<=8;    
	tem+=TL;																									//��õװ�λ
	tem=(float)tem*0.625;																			//ת��     
	if(temp)return tem; 																			//�����¶�ֵ
	else return -tem;    
}


