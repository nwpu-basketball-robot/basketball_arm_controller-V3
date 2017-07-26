#ifndef __MOTORCONTROL_H
#define __MOTORCONTROL_H
#include "sys.h"

#define ln1(a) if (a) \
                    GPIO_SetBits(GPIOB,GPIO_Pin_8);\
                else \
                    GPIO_ResetBits(GPIOB,GPIO_Pin_8)
#define ln2(a) if (a) \
                    GPIO_SetBits(GPIOC,GPIO_Pin_13);\
                else \
                    GPIO_ResetBits(GPIOC,GPIO_Pin_13)
                
void handle_data(uint8_t buf[], int D,int len);
void send(void);
void direction(int a);      
void get_ball(void);  
void high_down(void); 
void get_hold_ball(void); 
void down_from_hold(void);
void shot(void);               
               
void Stepper_motor(void);                
#endif
