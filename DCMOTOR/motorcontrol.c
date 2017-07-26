#include "motorcontrol.h"
#include "led.h"
#include "timer.h"
#include "delay.h"
#include "usart.h"	

static int noselect;	
int step=0;                       //步进数，标志位
volatile int flag=0,hold=0;      //flag标志铲子动作是否完成
u16 check=0;

void handle_data(uint8_t buf[], int D,int len)    
{
	 
		switch(D)
		{ 
            case 2:      if(buf[0]==1) 
		            	  {
                            if(buf[1]==1) {noselect = 0x04 ;break;}       //get_ball
							else if(buf[1]==2) {noselect = 0x05 ;break;}       //hign_down
							else if(buf[1]==3) {noselect = 0x06 ;break;}       //get_hold_ball
                            else if(buf[1]==4) {noselect = 0x07 ;break;}       //down_from_hold
                            else if(buf[1]==8) {noselect = 0x0C ;break;}       //释放
                            
                            
			              }
            case 3:      if(buf[0]==1) {noselect = 0x0a ;break;}	      //投球   此处CASE要和协议配合
            
			default:break;
		}	    
}  
void direction(int a)        //正为正转，负为反转
{	 

    if(a>0)
	 {
			ln1(0);      //使能
			ln2(1);
	 }
	if(a<0)
	 {

          ln1(0); 
		 ln2(0);
	 }
}

void Stepper_motor()
	 {
     TIM_Cmd(TIM3, ENABLE);  //使能TIM3
		 TIM_Cmd(TIM4, ENABLE);
   }
     
void get_ball()              //得球
{
    TIM_Cmd(TIM3, DISABLE);
    TIM_Cmd(TIM4, DISABLE);
    TIM4_PWM_Init(999,10);              //从持球位置下降
    flag=0;                             //标志位清零
    hold=0;                             //解除持球
    direction(1); 
    step=3000;                          //step值需要测试
    Stepper_motor();
    while(flag==0);  
    flag=0;
  													//释放电机
  												
}

void high_down()             	 //从最高处落下
{  
	
  TIM_Cmd(TIM3, DISABLE);
    TIM_Cmd(TIM4, DISABLE);
    TIM4_PWM_Init(999,10);              //从持球位置下降
    flag=0;                             //标志位清零
    hold=0;                             //解除持球
    direction(-1); 
    step=3000;                          //step值需要测试
    Stepper_motor();
    while(flag==0);  
    flag=0;
   	ln1(1);									//释放电机
}

void get_hold_ball()              //得球并持球状态
{
     TIM_Cmd(TIM3, DISABLE);
    TIM_Cmd(TIM4, DISABLE);
    TIM4_PWM_Init(999,10);              //从持球位置下降
    flag=0;                             //标志位清零
    hold=0;                             //解除持球
    direction(1); 
    step=7800;                          //step值需要测试
    Stepper_motor();
    while(flag==0);  
    flag=0;
  												//释放电机
}

void down_from_hold()
{
    TIM_Cmd(TIM3, DISABLE);
    TIM_Cmd(TIM4, DISABLE);
    TIM4_PWM_Init(999,10);              //从持球位置下降
    flag=0;                             //标志位清零
    hold=0;                             //解除持球
    direction(-1); 
    step=7800;                          //step值需要测试
    Stepper_motor();
    while(flag==0);  
    flag=0;
  	ln1(1);													//释放电机
}



void shot()            //射球
{
    
    GPIO_SetBits(GPIOC,GPIO_Pin_12);	
	delay_ms(1000);	
    GPIO_ResetBits(GPIOC,GPIO_Pin_12);	
    
    
	GPIO_SetBits(GPIOC,GPIO_Pin_7);	
	delay_ms(1000);	
//    delay_ms(1000);
//    delay_ms(1000);
//    delay_ms(1000);
//    delay_ms(1000);
//    delay_ms(1000);
    GPIO_ResetBits(GPIOC,GPIO_Pin_7);
   
}

void door_close()
{
    TIM_SetCompare3(TIM5,180);
    delay_ms(600);
}
void send(void)
{				
		switch(noselect)
		{
			case 0x00 :
				break;
			case 0x04 :            //上升一半                 
				get_ball();					
				noselect=0;
			break;
			case 0x05 :            //下降一半
                high_down();					
			  noselect=0;
			break;
			case 0x06 :             //上升最高		
                get_hold_ball();
					
			  noselect=0;
			break;
			case 0x07 :
				down_from_hold();		//下降最高
			  noselect=0;
			break;
			case 0x0a :
				shot();				
				noselect=0;
			break;	
            case 0x0C :
				ln1(1);		
				noselect=0;
			break;	
			 }		 
}

