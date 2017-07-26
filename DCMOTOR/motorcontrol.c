#include "motorcontrol.h"
#include "led.h"
#include "timer.h"
#include "delay.h"
#include "usart.h"	

static int noselect;	
int step=0;                       //����������־λ
volatile int flag=0,hold=0;      //flag��־���Ӷ����Ƿ����
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
                            else if(buf[1]==8) {noselect = 0x0C ;break;}       //�ͷ�
                            
                            
			              }
            case 3:      if(buf[0]==1) {noselect = 0x0a ;break;}	      //Ͷ��   �˴�CASEҪ��Э�����
            
			default:break;
		}	    
}  
void direction(int a)        //��Ϊ��ת����Ϊ��ת
{	 

    if(a>0)
	 {
			ln1(0);      //ʹ��
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
     TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3
		 TIM_Cmd(TIM4, ENABLE);
   }
     
void get_ball()              //����
{
    TIM_Cmd(TIM3, DISABLE);
    TIM_Cmd(TIM4, DISABLE);
    TIM4_PWM_Init(999,10);              //�ӳ���λ���½�
    flag=0;                             //��־λ����
    hold=0;                             //�������
    direction(1); 
    step=3000;                          //stepֵ��Ҫ����
    Stepper_motor();
    while(flag==0);  
    flag=0;
  													//�ͷŵ��
  												
}

void high_down()             	 //����ߴ�����
{  
	
  TIM_Cmd(TIM3, DISABLE);
    TIM_Cmd(TIM4, DISABLE);
    TIM4_PWM_Init(999,10);              //�ӳ���λ���½�
    flag=0;                             //��־λ����
    hold=0;                             //�������
    direction(-1); 
    step=3000;                          //stepֵ��Ҫ����
    Stepper_motor();
    while(flag==0);  
    flag=0;
   	ln1(1);									//�ͷŵ��
}

void get_hold_ball()              //���򲢳���״̬
{
     TIM_Cmd(TIM3, DISABLE);
    TIM_Cmd(TIM4, DISABLE);
    TIM4_PWM_Init(999,10);              //�ӳ���λ���½�
    flag=0;                             //��־λ����
    hold=0;                             //�������
    direction(1); 
    step=7800;                          //stepֵ��Ҫ����
    Stepper_motor();
    while(flag==0);  
    flag=0;
  												//�ͷŵ��
}

void down_from_hold()
{
    TIM_Cmd(TIM3, DISABLE);
    TIM_Cmd(TIM4, DISABLE);
    TIM4_PWM_Init(999,10);              //�ӳ���λ���½�
    flag=0;                             //��־λ����
    hold=0;                             //�������
    direction(-1); 
    step=7800;                          //stepֵ��Ҫ����
    Stepper_motor();
    while(flag==0);  
    flag=0;
  	ln1(1);													//�ͷŵ��
}



void shot()            //����
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
			case 0x04 :            //����һ��                 
				get_ball();					
				noselect=0;
			break;
			case 0x05 :            //�½�һ��
                high_down();					
			  noselect=0;
			break;
			case 0x06 :             //�������		
                get_hold_ball();
					
			  noselect=0;
			break;
			case 0x07 :
				down_from_hold();		//�½����
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

