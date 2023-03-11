#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"

#define BEEP_PORT		GPIOB
#define BEEP_RCC		RCC_APB2Periph_GPIOB
#define BEEP_PIN		GPIO_Pin_9
#define BEEP_PIN_MODE	GPIO_Mode_Out_PP  //�������

//�������˿ڶ���
#define BEEP PBout(9)	// BEEP,�������ӿ�	

//�������͵�ƽ����
#define BEEP_ON		0;
#define BEEP_OFF	1;

//����������״̬ȫ�ֱ���	1 ��  0�ر�
extern u8 BEEP_Status;

//��������ʼ��,������Ĭ��Ϊ�ر�
void BEEP_Init(void);

//�رջ�򿪷�����	1 �� 0 �ر�
void BEEP_SetOnOff(u8 onoff);
		 				    
#endif

