#include "stm32f10x.h"	
#include "timer.h"
#include "led.h"
#include "motorcontrol.h"

GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure; 
NVIC_InitTypeDef NVIC_InitStructure;
TIM_ICInitTypeDef TIM_ICInitStructure;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

extern int step,flag,hold;		        //�����������
u16 test=1;

void RCC_Init(void)
{  		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE ); 	  	//PC ���������Ƶ������ת
}

void GPIO_init(void)
{
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE);	   //����Ҫ��һ��������ϸ��ù���
  //PC ���������Ƶ������ת ����һ�� PC0 1���Ƶ��1��  2 3 ���Ƶ��2��  4 5 ���Ƶ��3��  6 7���Ƶ��4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13;	 
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
    
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_7;	 
	GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12;	 
	GPIO_Init(GPIOC, &GPIO_InitStructure);     
	
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8;	 
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
   
}
//////////////////////////////////////////////////////////////////////////////////   	 

//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx					 
}

void TIM8_PWM_Init(u16 arr,u16 psc)
{ 
    
    
    GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);	//ʹ�ܶ�ʱ��8ʱ��
 	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE );
	   
 
   //���ø�����Ϊ�����������,���TIM8 CH1 CH4��PWM���岨��	GPIOC.6,GPIOC.9
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_6 ;		
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;		   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_9 ;		
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;		   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);
 
   //��ʼ��TIM8
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period =arr;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;	
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;	
	
  TIM_OC1Init(TIM8, &TIM_OCInitStructure);	
	
	TIM_OC4Init(TIM8, &TIM_OCInitStructure);	
	
	TIM_CtrlPWMOutputs(TIM8, ENABLE);
	
	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable); 
    
    TIM_Cmd(TIM8, ENABLE); 
    
	TIM_SetCompare1(TIM8,800);
    TIM_SetCompare4(TIM8,800);
}


void TIM4_PWM_Init(u16 arr,u16 psc)
{ 
    
    
    GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 	//ʹ�ܶ�ʱ��4ʱ��
 	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	   
 
   //���ø�����Ϊ�����������,���TIM4 CH1��PWM���岨��	GPIOB.6
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_6 ;		
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;		   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
 
   //��ʼ��TIM4
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period =arr;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);	
	
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);	
	
	TIM_CtrlPWMOutputs(TIM4, ENABLE);
	
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable); 
	 TIM_SetCompare1(TIM4,850);
}


void TIM5_PWM_Init(u16 arr,u16 psc)
{ 
    
    
    GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); 	//ʹ�ܶ�ʱ��5ʱ��
 	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	   
 
   //���ø�����Ϊ�����������,���TIM5 CH3��PWM���岨��	GPIOA.2
    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_2 ;		
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;		   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
   //��ʼ��TIM5
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period =arr;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;	
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;	
	
    TIM_OC1Init(TIM5, &TIM_OCInitStructure);	
	TIM_OC3Init(TIM5, &TIM_OCInitStructure);	
	TIM_OC4Init(TIM5, &TIM_OCInitStructure);	
	
	TIM_CtrlPWMOutputs(TIM5, ENABLE);
	
	TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable); 
    TIM_Cmd(TIM5, ENABLE);  //ʹ��TIM5
}


void TIM_Configuration (void)
{

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		     //-----TIM7
	NVIC_InitStructure.NVIC_IRQChannel =TIM7_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	 //ʹ�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =2;			//�����������ȼ�	2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		   //������Ӧ���ȼ�	  0
	NVIC_Init(&NVIC_InitStructure); 	
	NVIC_SetVectorTable (NVIC_VectTab_FLASH ,0x0);	 //���ô���Ĵ���             �����������λ�ú�ƫ�ƣ���������������������������������������������
	
	TIM_TimeBaseStructure.TIM_Period =9999;   //���ü��������С��ÿ��2000�����Ͳ���һ�������¼�
	TIM_TimeBaseStructure.TIM_Prescaler =7199;	   //Ԥ��Ƶϵ��Ϊ36000-1������������ʱ��Ϊ72MHz/36000 = 2kHz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;   //����ʱ�ӷָ� TIM_CKD_DIV1=0x0000,���ָ�
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up ;	  //���ü�����ģʽΪ���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseStructure); //������Ӧ�õ�TIM7��
		
	TIM_UpdateRequestConfig( TIM7, TIM_UpdateSource_Regular);	
	TIM_Cmd(TIM7, ENABLE); //ʹ�ܼ�����
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);	//ʹ���ж�
	//TIM_ClearFlag(TIM7, TIM_FLAG_Update);//�����־λ
	

 }

//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
    static int count=0;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
		{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
			count++;
			test++;
			if (count>step)
			{
			count=0;
			step=0;
			flag=1;
			if(hold==0)
                {
                    TIM_Cmd(TIM4, DISABLE);
                    TIM_Cmd(TIM3, DISABLE);
                }		
            }
		}
}


void TIM7_IRQHandler(void) 							   
{ 
	static int flag_hold=1;
	if(TIM_GetITStatus(TIM7, TIM_IT_Update)== SET)	   //����Ƿ�����������¼�
	{
		//test=-test;
      flag_hold=-flag_hold;
      if(hold==1)
		 direction(flag_hold);
		TIM_ClearITPendingBit(TIM7 , TIM_FLAG_Update);

	}
}









