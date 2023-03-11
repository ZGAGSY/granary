#include "mq135.h"
#include "timer.h"
#include "adc_dma.h"
#include <stdio.h>
#include "math.h"
#include "delay.h"

#define CAL_PPM 	15  	// У׼������PPMֵ
#define RL			5.1		// RL��ֵ
static float R0_MQ135 = 4.77; 	// Ԫ���ڽྻ�����е���ֵ

uint32_t times_mq135 = 0;

//�õ�ADC�����ڲ���������ֵ
//ȡ10��,Ȼ��ƽ��
vu16 Get_ADCValue_MQ135(void)
{
	u32 val = 0;
	u8 times = 10;
	u8 count;
	for(count = 0; count < times; count++)
	{
		val += ADC_ConvertedValue[0];//��ȡDMAͨ��ֵ
		delay_ms(5);
	}
	return val/times;
}



 
// ������У׼����
void MQ135_PPM_Calibration(float RS)
{
    R0_MQ135 = RS / pow(CAL_PPM / 613.9f, 1 / -2.074f);
}
 
// ���������ݴ���
float MQ135_GetPPM(void)
{	
    float Vrl = 3.3f * Get_ADCValue_MQ135() / 4096.f;
	Vrl = ( (float)( (int)( (Vrl+0.005)*100 ) ) )/100;
    float RS = (3.3f - Vrl) / Vrl * RL; 
	
	if(times_mq135 < 6000) // ��ȡϵͳִ��ʱ�䣬300msǰ����У׼
    {
		R0_MQ135 = RS / pow(CAL_PPM / 613.9f, 1 / -2.074f);
    }
	
    float ppm = 613.9f * pow(RS/R0_MQ135, -2.074f);
	
    return  ppm;
}
		 







