
#include "exynos_4412.h"
#include "uart.h"
//#include "key.h"
#include "led.h"




void keey_init(void)
{
	GPX1.CON |= 0xf << 4;
	GPX1.CON |= 0xf << 8;
	GPX3.CON |= 0xf << 8;
	EXT_INT41_CON = (EXT_INT41_CON & ~ (0x7 << 4)) | (0x3 << 4);
	EXT_INT41_CON = (EXT_INT41_CON & ~ (0x7 << 8)) | (0x3 << 8);
	EXT_INT43_CON = (EXT_INT43_CON & ~ (0x7 << 8)) | (0x3 << 8);
	
	EXT_INT41_MASK &= ~(1<<1); 
	EXT_INT41_MASK &= ~(1<<2); 
	EXT_INT43_MASK &= ~(1<<2); 

	ICDDCR |= 1;//4�����ж�ʹ��
	//5��ʹ�ܶ˿��ж�
	ICDISER.ICDISER1 |= 1<<25;
	ICDISER.ICDISER1 |= 1<<26;
	ICDISER.ICDISER2 |= 1;
	
	//6���˿����ȼ�����
	ICDIPR.ICDIPR14  &= ~(0xff<<8);
	ICDIPR.ICDIPR14  &= ~(0xff<<16);
	ICDIPR.ICDIPR16  &= ~(0xff<<0);
	
	//7�������ж�Դ���Ա��ĸ�����ЩCPU��Ӧ
	ICDIPTR.ICDIPTR14 =( ICDIPTR.ICDIPTR14 & ~(0xff<<8)) | (0x1 <<8);
	ICDIPTR.ICDIPTR14 =( ICDIPTR.ICDIPTR14 & ~(0xff<<16)) | (0x1 <<16);
	ICDIPTR.ICDIPTR16 =( ICDIPTR.ICDIPTR16 & ~(0xff<<0)) | (0x1 <<0);
	
	//8��CPU �ж�ʹ�ܼĴ���
	CPU0.ICCICR  |= 1;
	//9��CPU ���ȼ����˼Ĵ���
	CPU0.ICCPMR |= 0xff;
}

void do_irq(void)
{
	int irq_id;
	//1����ȡ�ж�ID
	irq_id = CPU0.ICCIAR;

	//2���ж��ж�ID,�Բ�ͬ�ж�����Ӧ�Ĵ���
	switch (irq_id)
	{
		case 57:
		led(8,1);
			 printf("a\n\r" );
			//���GPIO�жϹ���
			EXT_INT41_PEND |= 1<<1;
			//���GIC�жϹ���
			ICDICPR.ICDICPR1 |= 1<<25;
			break;
		case 58:
		led(4,1);
			 printf("b\r\n" );
			//���GPIO�жϹ���
			EXT_INT41_PEND |= 1<<2;
			//���GIC�жϹ���
			ICDICPR.ICDICPR1 |= 1<<26;
			break;			
		case 64:
		led(2,1);
			 printf("c\r\n" );
			//���GPIO�жϹ���
			EXT_INT43_PEND |= 1<<2;
			//���GIC�жϹ���
			ICDICPR.ICDICPR2 |= 1<<0;
			break;		
	}
	//�����ж�
	CPU0.ICCEOIR = irq_id;
}


int main()
{
    int a = 100;
	led_init();
    uart_init();
    printf("hello!a=%d\r\n",a);

	keey_init();

    while(1);
    return 0;
}



