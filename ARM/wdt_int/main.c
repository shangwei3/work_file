
#include"exynos_4412.h"
#include"uart.h"
#include"wdt.h"

unsigned int tic = 0;

void sleep(unsigned int s)
{
	unsigned int tmp = tic;
	while (tic < tmp + s);
}


int main()
{
	int i,j;
	wdt_init();
    uart_init();
    printf("hello!sdt start!\r\n");
	sleep(5);
	printf("5s!\n\r");
    while(1)
	{
		
		
  printf("hello!sdt start!\r\n");
	sleep(5);
	printf("5s!\n\r");
		//定期喂狗
//		for(i=0;i<100;i++)
//			for(j=0;j<1000;j++);
//		WDT.WTCNT = 6250;
	}
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


