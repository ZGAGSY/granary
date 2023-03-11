//C��
#include <string.h>

//��Ƭ��ͷ�ļ�
#include "sys.h"

//����Э���
#include "onenet.h"

//�����豸
#include "esp8266.h"

//Ӳ������
#include "delay.h"
#include "usart.h"
#include "LCD.h"	 
#include "dht11.h" 	
#include "adc_dma.h"
#include "mq135.h"
#include "timer.h"
#include "gui.h"
#include "LCD.h"
#include "hc_sr501.h"
#include "hc_sr04.h"
#include "beep.h"
#include "motor.h"
uint32_t time = 0; // ms ��ʱ���� 
DHT11_Data_TypeDef DHT11_Data;


float value_mq135 = 0.0;    //��������
float value_sr04 = 0.0;		//����������
unsigned int weight = 0.0;
int high = 10;
float value_dht11_temp = 0.0;
float value_dht11_humi = 0.0;
u8 status_sr501 = 0;		//��������⵽��ֵ
int max_mq135 = 50;			//������ֵ
int max_dht11_temp = 28;	//�¶���ֵ
int max_dht11_humi = 40;	//ʪ����ֵ

//Ӳ����ʼ��
void Hardware_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	
	delay_init();			//��ʱ������ʼ��
	
	LCD_Init();	            //Һ������ʼ��
	LCD_Clear(GRAY0);
	Gui_DrawFont_GBK16(15,50,GREEN,GRAY0,(u8 *)"���ּ��ϵͳ");
	delay_ms(3000);
	LCD_Clear(GRAY0);
	Gui_DrawFont_GBK16(3,10,BLUE,GRAY0,(u8 *)"ϵͳ���ڳ�ʼ��");
	
	Usart1_Init(115200);	//����1��ʼ��Ϊ115200
	Usart2_Init(115200); 	//����2������ESP8266
	SR501_Init();			//����
	Hcsr04Init();			//���������	
	Motor_Init();
	ADCx_Init();			//ADC   DMA��ʼ��
	DHT11_GPIO_Config();	//DHT11��ʪ�ȴ�������ʼ��
	BEEP_Init();
		
	Gui_DrawFont_GBK16(4,30,RED,GRAY0,(u8*)"�豸��ʼ�����");
	delay_ms(1000);
	Gui_DrawFont_GBK16(4,50,RED,GRAY0,(u8*)"������������..");
	delay_ms(1000);
}

void Net_Init()
{
	ESP8266_Init();					//��ʼ��ESP8266
	
	while(OneNet_DevLink())			//����OneNET
		delay_ms(500);
	
	Gui_DrawFont_GBK16(4,70,RED,GRAY0,(u8*)"�����������");
	delay_ms(2000);
}

int main(void)
{
	unsigned char *dataPtr = NULL;
	unsigned short timeCount = 0;	//���ͼ������
	char str[] = "";
	
	Hardware_Init();				//��ʼ��Ӳ��
	Net_Init();						//�����ʼ��
	
	LCD_Clear(BLACK);
 	Gui_DrawFont_GBK16(25,4,BLACK,GRAY0,(u8 *)" ϵͳ��� ");
    GUI_DrawRectangle(1, 1, 128-2, 128-2, WHITE);		//�߿�
	
	while(1)
	{	
		delay_ms(10);
		timeCount++;
				
		//��������
		dataPtr = ESP8266_GetIPD(0);
		if(dataPtr != NULL)
		{
			OneNet_RevPro(dataPtr);
		}	
		
		if(timeCount >= 40)	//���ͼ��1s   1000/25 = 40
		{	
			/* ��ʪ�ȴ�������ȡ����*/
			/*����Read_DHT11��ȡ��ʪ�ȣ����ɹ����������Ϣ*/
			if( Read_DHT11(&DHT11_Data)==SUCCESS)
			{
				sprintf(str, "�¶ȣ�%d.%d �� ",DHT11_Data.temp_int,DHT11_Data.temp_deci);
				Gui_DrawFont_GBK16(4,20,RED,BLACK,(u8 *)str);
				sprintf(str, "ʪ�ȣ�%d.%d %c ",DHT11_Data.humi_int,DHT11_Data.humi_deci,'%');
				Gui_DrawFont_GBK16(4,40,RED,BLACK,(u8 *)str);
			}
			else
			{
				printf("Read DHT11 ERROR!\r\n");//��ȡ����ʧ�ܣ����ڴ�ӡ��Read DHT11 ERROR.
				Gui_DrawFont_GBK16(4,20,RED,BLACK,(u8 *)"ERROR!");
				Gui_DrawFont_GBK16(4,40,RED,BLACK,(u8 *)"ERROR!");
			}			
			

			/*MQ135��������ȡ������������*/	
			value_mq135 = MQ135_GetPPM();
			sprintf(str, "����%.1fppm",value_mq135);
			Gui_DrawFont_GBK16(4,60,BLUE,BLACK,(u8 *)str);
		
			
			/*����������ȡ���루CM��*/	
			value_sr04 = Hcsr04GetLength();
			weight = (high - value_sr04) * 3.14 * 25;
			sprintf(str, "������%d t", weight);
			Gui_DrawFont_GBK16(4,80,BLUE,BLACK,(u8 *)str);
			
			/*������*/	
//			status_sr501 = SR501_Check();
//			sprintf(str, "���⣺%d",status_sr501);
//			Gui_DrawFont_GBK16(4,100,BLUE,BLACK,(u8 *)str);

			timeCount = 0;
			ESP8266_Clear();
			OneNet_SendData();	//��������
		}	
		//�жϿ�����������ʪ���Ƿ񳬹�����
		value_dht11_temp = DHT11_Data.temp_int;
		value_dht11_humi = DHT11_Data.humi_int;
		if((int)value_mq135 >= max_mq135 || value_dht11_temp >= max_dht11_temp || value_dht11_humi >= max_dht11_humi)
		{
			BEEP = 0;	//����������
		}
		else
		{
			BEEP = 1;	//�������ر�
		}
		if(value_dht11_temp >= max_dht11_temp || value_dht11_humi >= max_dht11_humi)
		{
			Go();
		}
		else
		{
			Stop();
		}
	}
		
}



















