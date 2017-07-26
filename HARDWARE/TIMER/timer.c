#include "stm32f10x.h"	
#include "timer.h"
#include "led.h"
#include "motorcontrol.h"

GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure; 
NVIC_InitTypeDef NVIC_InitStructure;
TIM_ICInitTypeDef TIM_ICInitStructure;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

extern int step,flag,hold;		        //步进电机步数
u16 test=1;

void RCC_Init(void)
{  		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE ); 	  	//PC 口用来控制电机正反转
}

void GPIO_init(void)
{
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE);	   //很重要的一条必须加上复用功能
  //PC 口用来控制电机正反转 两个一组 PC0 1控制电机1；  2 3 控制电机2；  4 5 控制电机3；  6 7控制电机4
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

//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


	TIM_Cmd(TIM3, ENABLE);  //使能TIMx					 
}

void TIM8_PWM_Init(u16 arr,u16 psc)
{ 
    
    
    GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);	//使能定时器8时钟
 	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE );
	   
 
   //设置该引脚为复用输出功能,输出TIM8 CH1 CH4的PWM脉冲波形	GPIOC.6,GPIOC.9
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_6 ;		
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;		   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_9 ;		
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;		   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);
 
   //初始化TIM8
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
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 	//使能定时器4时钟
 	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	   
 
   //设置该引脚为复用输出功能,输出TIM4 CH1的PWM脉冲波形	GPIOB.6
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_6 ;		
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;		   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
 
   //初始化TIM4
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
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); 	//使能定时器5时钟
 	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	   
 
   //设置该引脚为复用输出功能,输出TIM5 CH3的PWM脉冲波形	GPIOA.2
    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_2 ;		
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;		   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
   //初始化TIM5
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
    TIM_Cmd(TIM5, ENABLE);  //使能TIM5
}


void TIM_Configuration (void)
{

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		     //-----TIM7
	NVIC_InitStructure.NVIC_IRQChannel =TIM7_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	 //使能优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =2;			//配置抢断优先级	2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		   //配置响应优先级	  0
	NVIC_Init(&NVIC_InitStructure); 	
	NVIC_SetVectorTable (NVIC_VectTab_FLASH ,0x0);	 //设置存入寄存器             设置向量表的位置和偏移？？？？？？？？？？？？？？？？？？？？？？？
	
	TIM_TimeBaseStructure.TIM_Period =9999;   //设置计数溢出大小，每计2000个数就产生一个更新事件
	TIM_TimeBaseStructure.TIM_Prescaler =7199;	   //预分频系数为36000-1，这样计数器时钟为72MHz/36000 = 2kHz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;   //设置时钟分割 TIM_CKD_DIV1=0x0000,不分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up ;	  //设置计数器模式为向上计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseStructure); //将配置应用到TIM7中
		
	TIM_UpdateRequestConfig( TIM7, TIM_UpdateSource_Regular);	
	TIM_Cmd(TIM7, ENABLE); //使能计数器
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);	//使能中断
	//TIM_ClearFlag(TIM7, TIM_FLAG_Update);//清除标志位
	

 }

//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
    static int count=0;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
		{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志 
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
	if(TIM_GetITStatus(TIM7, TIM_IT_Update)== SET)	   //检测是否发生溢出更新事件
	{
		//test=-test;
      flag_hold=-flag_hold;
      if(hold==1)
		 direction(flag_hold);
		TIM_ClearITPendingBit(TIM7 , TIM_FLAG_Update);

	}
}









