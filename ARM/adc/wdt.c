#include "exynos_4412.h"


void wdt_init(void)
{
	//配置预分频
	WDT.WTCON = (WDT.WTCON & ~(0xff << 8)) | (124 << 8);
	//固定分频128
	WDT.WTCON |= 0x3 << 3;
	//设置初始计数值
	WDT.WTCNT  = 6250;
	
	//是能看门狗
	WDT.WTCON |= 1<<5;
	//复位信号使能
//	WDT.WTCON |= 1;
	WDT.WTCON &= ~1;

	//自动重载寄存器
	WDT.WTDAT  = 6250;

	//使能中断
	WDT.WTCON |= 1<<2;

	//gic中断控制器配置
	ICDDCR |= 1;//4、总中断使能
	//5、使能端口中断
	ICDISER.ICDISER2 |= 1<<11;
	//6、端口优先级配置
	ICDIPR.ICDIPR18  &= ~(0xff<<24);
	//7、配置中断源可以被哪个或哪些CPU相应
	ICDIPTR.ICDIPTR18 =( ICDIPTR.ICDIPTR18 & ~(0xff<<24)) | (0x1 <<24);
	//8、CPU 中断使能寄存器
	CPU0.ICCICR  |= 1;
	//9、CPU 优先级过滤寄存器
	CPU0.ICCPMR |= 0xff;

}



