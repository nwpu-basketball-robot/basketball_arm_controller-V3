#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEKս��STM32������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   

void RCC_Init(void);
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM8_PWM_Init(u16 arr,u16 psc); 
void TIM4_PWM_Init(u16 arr,u16 psc);
void TIM5_PWM_Init(u16 arr,u16 psc);
void TIM_Configuration (void);
void GPIO_init(void);
#endif