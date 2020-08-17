#include "exynos_4412.h"
#include "uart.h"
#include "led.h"
//int cunt=0;

void do_irq(void)
{
	static int a = 1;
	static int cunt=0;
	int irq_num;
	
	irq_num = CPU0.ICCIAR&0x3ff;  //获取中断号
	//printf("mun %d\n\r",irq_num);
	
	switch(irq_num)
	 {
	 	//cunt++;
	 	case 75:
	 	cunt++;
	 	printf("cunt %d\n\r",cunt);
	 	//PWM.TCON = PWM.TCON & (~(0xff)) | (1 << 3)| (1 << 0) ;
 		PWM.TCON |=1;
		 if(cunt==2)/// 
		{ 
			led(1,1); led(~8,0); printf("LED1\n\r");
		//	PWM.TCON = PWM.TCON & (~(1 << 0)) ;   
		} 
			
		if(cunt==4)
		{ 
			led(2,1); led(~1,0); printf("LED2\n\r"); 	
		//	PWM.TCON = PWM.TCON & (~(1 << 0)) ;  
		} 
		if(cunt==6)
		{ 
			led(4,1); led(~2,0); printf("LED3\n\r");
		//	PWM.TCON = PWM.TCON & (~(1 << 0)) ;  
	 	}
		if(cunt==8)
		{ 
			led(8,1); led(~4,0); printf("LED4\n\r"); 
		//	PWM.TCON = PWM.TCON & (~(1 << 0)) ;
		}
		if(cunt>=8)
			cunt=0;
			break; 
	 }
	
	WDT.WTCLRINT = 1;
	ICDICPR.ICDICPR2 |= 1<<11;
	CPU0.ICCEOIR = irq_num;

		
}
void pwm_init(void)
{
	
	
	GPD0.CON = (GPD0.CON & ~(0xf<<0)) | (0x2<<0);

	PWM.TCFG0 = (PWM.TCFG0 & ~(0xff<<0)) | (0x0<<0);

	PWM.TCFG1 = (PWM.TCFG1 & ~(0xf<<0)) | (0x1<<0);

	PWM.TCNTB0 = 500000  ;

	PWM.TCMPB0 = 1000000  ;
	PWM.TCON |=1<<1;
	PWM.TCON |=~(1<<1);
	PWM.TCON |=3<<2;
//	PWM.TCON = (PWM.TCON & ~(0xf << 0)) | (0xf<<0);


/*	GPD0.CON = GPD0.CON & (~(0xf))| 0x2;
	
	PWM.TCFG0 = PWM.TCFG0 & (~(0xff))|0xf9;
	PWM.TCFG1 = PWM.TCFG1 & (~(0xf)) | 0x2;
	
	PWM.TCMPB0 = 25;
	PWM.TCNTB0 = 200;
	
	PWM.TCON = PWM.TCON & (~(0xf)) | (1 << 0) | (1 << 1) | (1 << 2) ;
	*/
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


void wdt_init(void)
{
	WDT.WTCON = (12 << 8) | (1 << 5) | (1 << 2)&(~1 << 0);
	WDT.WTCON |= 0x3 << 3;
	
	WDT.WTCNT  = 6250;
	
	ICDDCR |= 1;
	
	ICDISER.ICDISER2 |= 1<<11;
	ICDIPR.ICDIPR18  &= ~(0xff<<24);
	ICDIPTR.ICDIPTR18 =( ICDIPTR.ICDIPTR18 & ~(0xff<<24)) | (0x1 <<24);
	CPU0.ICCICR |= 1;
	CPU0.ICCPMR |= 0xff;

}
 
 
int main (void)
{	
	int i,j,data;
	wdt_init();
	led_init();
	pwm_init();
	adc_init();
//	printf("hello reset!\n\r");



loop:
		ADCCON |=1;
  	  while( !(ADCCON & (1<<15) ) )
		{	
			data=ADCDAT & 0xfff;
			printf("data %d \n\r",data);
			//sleep(1);
		}
goto loop;



	
	
	while(1);
	{
	printf("hello !\n\r");
	//定期喂狗
		//for(i=0;i<100;i++)
		//	for(j=0;j<1000;j++);
	 //	WDT.WTCNT = 6250;
	}
   return 0;
}
 

 
