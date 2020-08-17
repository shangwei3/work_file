#include "exynos_4412.h"
#include "key.h"

#define  NULL (void *)0

irq_tp irq_hand = NULL;


void key_init( irq_tp irq_f )
{
	//配置GPIO k2 k3 k4
	GPX1.CON |= 0xf << 4;
	GPX1.CON |= 0xf << 8;
	GPX3.CON |= 0xf << 8;

	EXT_INT41_CON = (EXT_INT41_CON & ~ (0x7 << 4)) | (0x3 << 4);
	EXT_INT41_CON = (EXT_INT41_CON & ~ (0x7 << 8)) | (0x3 << 8);
	EXT_INT43_CON = (EXT_INT43_CON & ~ (0x7 << 8)) | (0x3 << 8);
	
	EXT_INT41_MASK &= ~(1<<1); 
	EXT_INT41_MASK &= ~(1<<2); 
	EXT_INT43_MASK &= ~(1<<2); 

	//配置GIC
	ICDDCR |= 1;//4、总中断使能
	//5、使能端口中断
	ICDISER.ICDISER1 |= 1<<25;
	ICDISER.ICDISER1 |= 1<<26;
	ICDISER.ICDISER2 |= 1;
	
	//6、端口优先级配置
	ICDIPR.ICDIPR14  &= ~(0xff<<8);
	ICDIPR.ICDIPR14  &= ~(0xff<<16);
	ICDIPR.ICDIPR16  &= ~(0xff<<0);
	
	//7、配置中断源可以被哪个或哪些CPU相应
	ICDIPTR.ICDIPTR14 =( ICDIPTR.ICDIPTR14 & ~(0xff<<8)) | (0x1 <<8);
	ICDIPTR.ICDIPTR14 =( ICDIPTR.ICDIPTR14 & ~(0xff<<16)) | (0x1 <<16);
	ICDIPTR.ICDIPTR16 =( ICDIPTR.ICDIPTR16 & ~(0xff<<0)) | (0x1 <<0);
	
	//8、CPU 中断使能寄存器
	CPU0.ICCICR  |= 1;
	//9、CPU 优先级过滤寄存器
	CPU0.ICCPMR |= 0xff;

	//注册中断函数
	irq_hand  =  irq_f;
}

void do_irq(void )
{
	int irq_id ;
	//1、获取中断ID
	irq_id = CPU0.ICCIAR;

	if(irq_hand != NULL)
		irq_hand(irq_id);

	//2、判断中断ID,对不同中断做相应的处理
	switch (irq_id)
	{
		case 57:
			//清除GPIO中断挂起
			EXT_INT41_PEND |= 1<<1;
			//清除GIC中断挂起
			ICDICPR.ICDICPR1 |= 1<<25;
			break;
		case 58:
			//清除GPIO中断挂起
			EXT_INT41_PEND |= 1<<2;
			//清除GIC中断挂起
			ICDICPR.ICDICPR1 |= 1<<26;
			break;			
		case 64:
			//清除GPIO中断挂起
			EXT_INT43_PEND |= 1<<2;
			//清除GIC中断挂起
			ICDICPR.ICDICPR2 |= 1<<0;
			break;		
	}
	//结束中断
	CPU0.ICCEOIR = irq_id;
}





