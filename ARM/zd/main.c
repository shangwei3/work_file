#include "exynos_4412.h"
#include "uart.h"
#include "led.h"

void do_irq(void)
{
	static int a = 1;
	int irq_num;
	printf("hello \n\r");
	//printf("num %d\n",CPU0.ICCIAR);
	//irq_num = CPU0.ICCIAR&0x3ff;  //获取中断号
	irq_num = CPU0.ICCIAR;
	switch(irq_num)
	{
		
	case 57:
		printf("\n\r");
			if(a)
			{
				printf("LED1\n\n\r");
				led(0x8,1);	
			}
			else
			{
				printf("LED0\n\r");
				led(~0x8,0);	
			}
			
			a = !a;
			EXT_INT41_PEND = EXT_INT41_PEND |((0x1 << 1)); 
			ICDICPR.ICDICPR1 = ICDICPR.ICDICPR1 | (0x1 << 25); 
		break;
			case 58:
		if(a){
				printf("LED2\n\n\r");
				led(0x4,1);	
		}
		else{
				printf("LED2\n\n\r");
				led(~0x4,0);	
		}
		EXT_INT41_PEND |= 1<<1;
		ICDICPR.ICDICPR1 |= 1<<26;
		a = !a;
		break;
	case 64:
		if(a){
				printf("LED3\n\n\r");
				led(0x3,1);	
		}
		else{
				printf("LED3\n\n\r");
				led(~0x3,0);	
		}
		EXT_INT43_PEND |= 1<<1;
		ICDICPR.ICDICPR2 |= 1;
		a = !a;
		break;
	}
	CPU0.ICCEOIR = CPU0.ICCEOIR&(~(0x3ff))|irq_num; 
}

int main (void)
{
	/*/配置GPIO 
	GPX1.CON |=0xf << 4; 
	GPX1.CON |=0xf << 8; 
	//GPX1.PUD = GPX1.PUD & (~(0x3 << 2));

       
	EXT_INT41_CON = (EXT_INT41_CON & ~(7<< 4))|(0x3 << 4); 
	EXT_INT41_MASK &= ~(1<<1); 
	
	EXT_INT41_CON = (EXT_INT41_CON & ~(7<< 8))|(0x3 << 8); 
	EXT_INT41_MASK &= ~(1<<2);
	//配置GIC 

	ICDDCR |= 1;   //总中断使能 
	
	
	
	ICDISER.ICDISER1 |= (0x1 << 25);//使能端口中断 
	ICDIPR.ICDIPR14 &= ~(0xff << 8);//优先级 
	ICDIPTR.ICDIPTR14 =(ICDIPTR.ICDIPTR14 & ~(0xff << 8))|(0x1<<8);
	
	//配置GIC 

	ICDISER.ICDISER1 |= (0x1 << 26);//使能端口中断 
	ICDIPR.ICDIPR14 &= ~(0xff <<16);// 
	ICDIPTR.ICDIPTR14 =(ICDIPTR.ICDIPTR14 & ~(0xff <<16))|(0x1<<16);
	
	
	CPU0.ICCICR = 1;  //配置中断源可以被哪个或哪些CPU相应
	CPU0.ICCPMR = 0xff; /*/  
	
	//GPIO
	printf("STATR\n\n\r");
	GPX1.CON |= 0xf<<4;
	GPX1.CON |= 0xf<<8;
	GPX3.CON |= 0xf<<8; //GPX3_2

	EXT_INT41_CON |= 0x3<<4;
	EXT_INT41_CON |= 0x3<<8;
	EXT_INT43_CON |= 0x3<<8;

	EXT_INT41_MASK &= 0<<1;
	EXT_INT41_MASK &= 0<<2;
	EXT_INT43_MASK &= 0<<2;
//	EXT_INT41_PEND  

	//GIC
	ICDDCR |= 1;
	
	ICDISER.ICDISER1 |= 1<<25;
	ICDISER.ICDISER1 |= 1<<26;
	ICDISER.ICDISER2 |= 1; //32 64;
                                                                                                 
	ICDIPR.ICDIPR14 = ICDIPR.ICDIPR14 &(0x00<<8);
	ICDIPR.ICDIPR14 = ICDIPR.ICDIPR14 &(0x00<<16);
	ICDIPR.ICDIPR16 &= 0x00;
	
	ICDIPTR.ICDIPTR14 =(ICDIPTR.ICDIPTR14 &~(0xff)) |(0x1<<8);
	ICDIPTR.ICDIPTR14 =(ICDIPTR.ICDIPTR14 &~(0xff)) |(0x1<<16);
	ICDIPTR.ICDIPTR16 =(ICDIPTR.ICDIPTR14 &~(0xff)) | 0x1;

	CPU0.ICCICR |= 1;
	CPU0.ICCPMR |= 0xff;
	

	
	led_init();
	while(1)
	{
 
	}
   return 0;
}
 
