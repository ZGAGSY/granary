//C库
#include <string.h>

//单片机头文件
#include "sys.h"

//网络协议层
#include "onenet.h"

//网络设备
#include "esp8266.h"

//硬件驱动
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
uint32_t time = 0; // ms 计时变量 
DHT11_Data_TypeDef DHT11_Data;


float value_mq135 = 0.0;    //空气质量
float value_sr04 = 0.0;		//超声波距离
unsigned int weight = 0.0;
int high = 10;
float value_dht11_temp = 0.0;
float value_dht11_humi = 0.0;
u8 status_sr501 = 0;		//人体红外检测到的值
int max_mq135 = 50;			//烟雾阈值
int max_dht11_temp = 28;	//温度阈值
int max_dht11_humi = 40;	//湿度阈值

//硬件初始化
void Hardware_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	
	delay_init();			//延时函数初始化
	
	LCD_Init();	            //液晶屏初始化
	LCD_Clear(GRAY0);
	Gui_DrawFont_GBK16(15,50,GREEN,GRAY0,(u8 *)"粮仓监测系统");
	delay_ms(3000);
	LCD_Clear(GRAY0);
	Gui_DrawFont_GBK16(3,10,BLUE,GRAY0,(u8 *)"系统正在初始化");
	
	Usart1_Init(115200);	//串口1初始化为115200
	Usart2_Init(115200); 	//串口2，驱动ESP8266
	SR501_Init();			//红外
	Hcsr04Init();			//超声波测距	
	Motor_Init();
	ADCx_Init();			//ADC   DMA初始化
	DHT11_GPIO_Config();	//DHT11温湿度传感器初始化
	BEEP_Init();
		
	Gui_DrawFont_GBK16(4,30,RED,GRAY0,(u8*)"设备初始化完成");
	delay_ms(1000);
	Gui_DrawFont_GBK16(4,50,RED,GRAY0,(u8*)"正在连接网络..");
	delay_ms(1000);
}

void Net_Init()
{
	ESP8266_Init();					//初始化ESP8266
	
	while(OneNet_DevLink())			//接入OneNET
		delay_ms(500);
	
	Gui_DrawFont_GBK16(4,70,RED,GRAY0,(u8*)"网络连接完成");
	delay_ms(2000);
}

int main(void)
{
	unsigned char *dataPtr = NULL;
	unsigned short timeCount = 0;	//发送间隔变量
	char str[] = "";
	
	Hardware_Init();				//初始化硬件
	Net_Init();						//网络初始化
	
	LCD_Clear(BLACK);
 	Gui_DrawFont_GBK16(25,4,BLACK,GRAY0,(u8 *)" 系统监测 ");
    GUI_DrawRectangle(1, 1, 128-2, 128-2, WHITE);		//边框
	
	while(1)
	{	
		delay_ms(10);
		timeCount++;
				
		//接收数据
		dataPtr = ESP8266_GetIPD(0);
		if(dataPtr != NULL)
		{
			OneNet_RevPro(dataPtr);
		}	
		
		if(timeCount >= 40)	//发送间隔1s   1000/25 = 40
		{	
			/* 温湿度传感器获取数据*/
			/*调用Read_DHT11读取温湿度，若成功则输出该信息*/
			if( Read_DHT11(&DHT11_Data)==SUCCESS)
			{
				sprintf(str, "温度：%d.%d ℃ ",DHT11_Data.temp_int,DHT11_Data.temp_deci);
				Gui_DrawFont_GBK16(4,20,RED,BLACK,(u8 *)str);
				sprintf(str, "湿度：%d.%d %c ",DHT11_Data.humi_int,DHT11_Data.humi_deci,'%');
				Gui_DrawFont_GBK16(4,40,RED,BLACK,(u8 *)str);
			}
			else
			{
				printf("Read DHT11 ERROR!\r\n");//读取数据失败，串口打印：Read DHT11 ERROR.
				Gui_DrawFont_GBK16(4,20,RED,BLACK,(u8 *)"ERROR!");
				Gui_DrawFont_GBK16(4,40,RED,BLACK,(u8 *)"ERROR!");
			}			
			

			/*MQ135传感器获取空气质量数据*/	
			value_mq135 = MQ135_GetPPM();
			sprintf(str, "烟雾：%.1fppm",value_mq135);
			Gui_DrawFont_GBK16(4,60,BLUE,BLACK,(u8 *)str);
		
			
			/*超声波测距获取距离（CM）*/	
			value_sr04 = Hcsr04GetLength();
			weight = (high - value_sr04) * 3.14 * 25;
			sprintf(str, "质量：%d t", weight);
			Gui_DrawFont_GBK16(4,80,BLUE,BLACK,(u8 *)str);
			
			/*人体检测*/	
//			status_sr501 = SR501_Check();
//			sprintf(str, "红外：%d",status_sr501);
//			Gui_DrawFont_GBK16(4,100,BLUE,BLACK,(u8 *)str);

			timeCount = 0;
			ESP8266_Clear();
			OneNet_SendData();	//发送数据
		}	
		//判断空气质量、温湿度是否超过上限
		value_dht11_temp = DHT11_Data.temp_int;
		value_dht11_humi = DHT11_Data.humi_int;
		if((int)value_mq135 >= max_mq135 || value_dht11_temp >= max_dht11_temp || value_dht11_humi >= max_dht11_humi)
		{
			BEEP = 0;	//蜂鸣器报警
		}
		else
		{
			BEEP = 1;	//蜂鸣器关闭
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



















