#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "queue.h"
#include "motorcontrol.h"

#define MAX_DATA_LEN 100
volatile Queue rx_queue;

uint8_t buffer[MAX_DATA_LEN]={0};
uint8_t data,flag_11=1;
   static uint16_t checksum;
 int main(void)
 {		
 
     uint8_t datalen1;
     int device;
     short data_len;
     int cur=0;
     
//     uint8_t flag_cc=1;
//     uint8_t w=0;
     enum Recstate
     {
        RECFF1,RECFF2,RECID,RECLEN1,RECLEN2,RECSEL,RECCHECK
     }rs485Recstate=RECFF1;
     delay_init();
     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
     uart_init(115200);
     RCC_Init();		                //所有时钟使能
     GPIO_init();
    // LED_Init();
    TIM3_Int_Init(999,10);
	 TIM4_PWM_Init(999,10);       //1200Hz
  //  TIM8_PWM_Init(1799,799);
   TIM_Configuration();	        //  TIM7主中断设置		
     ln1(1);
     queue_init(&rx_queue);
     GPIO_ResetBits(GPIOC,GPIO_Pin_7);
     
     	
     delay_ms(1000);delay_ms(1000);	delay_ms(1000);	delay_ms(1000);		
     delay_ms(1000);delay_ms(1000);	delay_ms(1000);	delay_ms(1000);		
     delay_ms(1000);delay_ms(1000);	delay_ms(1000);	delay_ms(1000);	
     delay_ms(1000);delay_ms(1000);	delay_ms(1000);	delay_ms(1000);	
     delay_ms(1000);delay_ms(1000);	delay_ms(1000);	delay_ms(1000);	
     get_hold_ball();
     
     while(1)
     {   
//         if(flag_11==0)    //测试用
//         {
//            get_hold_ball();
//             flag_11=1;
//         }
//         
//         else if(flag_11==2)
//         {
//             down_from_hold();
//             flag_11=1;
//         }
//         else if(flag_11==3)
//         {
//             shot();
//             flag_11=1;
//         }
//          else if(flag_11==4)
//         {
//            get_ball();	
//             flag_11=1;
//         }
//          else if(flag_11==5)
//         {
//            high_down();
//             flag_11=1;
//         }
    if(queue_empty(rx_queue))
            continue;
        data=(uint8_t)queue_get(&rx_queue);
 		switch (rs485Recstate) 
		{
			case RECFF1:
				if (data == 0xff)	  
				{	
					rs485Recstate = RECFF2;	
					checksum=0;		
					cur = 0;								//校验位清零
                    
                }
	
				break;
	
			case RECFF2:
				if (data == 0xff)
                rs485Recstate = RECID;
               
			    else
					rs485Recstate = RECFF1;
		
				break;
            case RECID:
                flag_11=1;
			 if(data==0x02)                       //设备编号0x02
					{
						device=2;
					    checksum += data;
					    rs485Recstate = RECLEN1;
                        
				      }
              else if(data==0x03)                        //设备编号0x03投球 
				     {
							device=3;
					    checksum += data;
					    rs485Recstate = RECLEN1;
				      }
			else
				 rs485Recstate = RECFF1;	  
				break;
	
			case RECLEN1:				 				
				checksum += data;
				datalen1 = data;
				rs485Recstate = RECLEN2;	  
				break;
		
			case RECLEN2:	
             
				checksum += data;			 				
				data_len = (short)datalen1 << 8	 | data;
				rs485Recstate = RECSEL;	  
				break;
	
			case RECSEL:
				checksum += data;
				buffer[cur++] = data;
				if(cur >= data_len)
					rs485Recstate = RECCHECK;
				break;
	
			case RECCHECK:
               
				checksum=checksum%255;
				if(data == checksum)
				{	
                    
					handle_data(buffer,device, data_len);
                    send();				
					checksum=0;	
					rs485Recstate = RECFF1;	 
                    
				}
				else
					rs485Recstate = RECFF1;
				break;
			 default:
					rs485Recstate = RECFF1;       
     
        } 
    }
}	 
 
