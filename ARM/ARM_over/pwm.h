#ifndef __PWM_H
#define __PWM_H 

void pwm_init(unsigned int p,unsigned int b);
void pwm_start(void);
void pwm_stop(void);
void delayms(unsigned int ms);
void delay_init(void);

#endif
