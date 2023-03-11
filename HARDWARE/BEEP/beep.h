#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"

#define BEEP_PORT		GPIOB
#define BEEP_RCC		RCC_APB2Periph_GPIOB
#define BEEP_PIN		GPIO_Pin_9
#define BEEP_PIN_MODE	GPIO_Mode_Out_PP  //推挽输出

//蜂鸣器端口定义
#define BEEP PBout(9)	// BEEP,蜂鸣器接口	

//蜂鸣器低电平驱动
#define BEEP_ON		0;
#define BEEP_OFF	1;

//申明蜂鸣器状态全局变量	1 打开  0关闭
extern u8 BEEP_Status;

//蜂鸣器初始化,蜂鸣器默认为关闭
void BEEP_Init(void);

//关闭或打开蜂鸣器	1 打开 0 关闭
void BEEP_SetOnOff(u8 onoff);
		 				    
#endif

