
#include"exynos_4412.h"
#include"uart.h"
#include"wdt.h"
#include"led.h"
unsigned int tic = 0;

void sleep(unsigned int s)
{
	unsigned int tmp = tic;
	while (tic < tmp + s);
}

void adc_init(void)
{
	ADCCON |=1<<16;
	ADCCON |=1<<14;
	ADCCON =(ADCCON & (~0xff<<6)) | 19<<6;
	ADCCON &=~(3<<1);
	//ADCCON &=1<<2;
	ADCMUX |=3;
}
 void delay_init(void)
 {
 	PWM.TCFG0= (PWM.TCFG0 & ~(0xff<<8))|(99<<8);
 	PWM.TCFG1 &=~(0xf<<16);
 	PWM.TCNTB4=0xffffffff;
 	PWM.TCON |= (3<<21);
 	PWM.TCON &= ~(1<<21);
 	PWM.TCON |= (1<<20);	
 }
 
 void pwm_init(void)
{
	
	
//	PWM.TCON = (PWM.TCON & ~(0xf << 0)) | (0xf<<0);


	GPD0.CON = GPD0.CON & (~(0xf))| 0x2;
	
	PWM.TCFG0 = PWM.TCFG0 & (~(0xff))|0xf9;
	PWM.TCFG1 = PWM.TCFG1 & (~(0xf)) | 0x2;
	
	PWM.TCMPB0 = 25;
	PWM.TCNTB0 = 200;
	
	PWM.TCON = PWM.TCON & (~(0xf)) | (1 << 0) | (1 << 1) | (1 << 2) ;
	
}
 
  
void start_pwm(void)
{
	PWM.TCON |= 1;
}
void stop_pwm(void)
{
	PWM.TCON |= ~1;
}
void delayms(unsigned int ms)
{
	unsigned int us = PWM.TCNTO4;
	while(PWM.TCNTO4<us + ms *1000);
}
int main()
{
	int i,j,data,cunt=0;
	wdt_init();
    uart_init();
    //delay_init();
    adc_init();
    led_init();
    pwm_init();
   
loop:
		ADCCON |=1;
  	  while( !(ADCCON & (1<<15) ) )
		{	
			data=ADCDAT & 0xfff;
			printf("data %d \n\r",data);
			sleep(1);
			//delayms(1000);
	 	cunt++;
	 
	 		
	 	printf("cunt %d\n\r",cunt);
	 	PWM.TCON = PWM.TCON & (~(0xff)) | (1 << 3)| (1 << 0) ;
 		//PWM.TCON |=1;
		 if(cunt==1)/// 
		{ 
			led(1,1); led(~8,0); printf("LED1\n\r");
			PWM.TCON = PWM.TCON & (~(1 << 0)) ;   
		} 
			
		if(cunt==2)
		{ 
			led(2,1); led(~1,0); printf("LED2\n\r"); 	
			PWM.TCON = PWM.TCON & (~(1 << 0)) ;  
		} 
		if(cunt==3)
		{ 
			led(4,1); led(~2,0); printf("LED3\n\r");
			PWM.TCON = PWM.TCON & (~(1 << 0)) ;  
	 	}
		if(cunt==4)
		{ 
			led(8,1); led(~4,0); printf("LED4\n\r"); 
			PWM.TCON = PWM.TCON & (~(1 << 0)) ;
		}
		if(cunt>=4)
			cunt=0;
			break;
			
			
		///////////////	
		}
goto loop;
while(1);	
    return 0;
}

void do_irq(void )
{
	int irq_id ;
	//1、获取中断ID
	irq_id = CPU0.ICCIAR;
		//printf("wdt time out!\n\r");
		tic ++;
	//清除WDT中断挂起
	WDT.WTCLRINT = 1;
	//清除GIC中断挂起
	ICDICPR.ICDICPR2 |= 1<<11;
	//结束中断
	CPU0.ICCEOIR = irq_id;
}


