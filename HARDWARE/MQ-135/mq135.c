#include "mq135.h"
#include "timer.h"
#include "adc_dma.h"
#include <stdio.h>
#include "math.h"
#include "delay.h"

#define CAL_PPM 	15  	// 校准环境中PPM值
#define RL			5.1		// RL阻值
static float R0_MQ135 = 4.77; 	// 元件在洁净空气中的阻值

uint32_t times_mq135 = 0;

//得到ADC采样内部传感器的值
//取10次,然后平均
vu16 Get_ADCValue_MQ135(void)
{
	u32 val = 0;
	u8 times = 10;
	u8 count;
	for(count = 0; count < times; count++)
	{
		val += ADC_ConvertedValue[0];//获取DMA通道值
		delay_ms(5);
	}
	return val/times;
}



 
// 传感器校准函数
void MQ135_PPM_Calibration(float RS)
{
    R0_MQ135 = RS / pow(CAL_PPM / 613.9f, 1 / -2.074f);
}
 
// 传感器数据处理
float MQ135_GetPPM(void)
{	
    float Vrl = 3.3f * Get_ADCValue_MQ135() / 4096.f;
	Vrl = ( (float)( (int)( (Vrl+0.005)*100 ) ) )/100;
    float RS = (3.3f - Vrl) / Vrl * RL; 
	
	if(times_mq135 < 6000) // 获取系统执行时间，300ms前进行校准
    {
		R0_MQ135 = RS / pow(CAL_PPM / 613.9f, 1 / -2.074f);
    }
	
    float ppm = 613.9f * pow(RS/R0_MQ135, -2.074f);
	
    return  ppm;
}
		 







