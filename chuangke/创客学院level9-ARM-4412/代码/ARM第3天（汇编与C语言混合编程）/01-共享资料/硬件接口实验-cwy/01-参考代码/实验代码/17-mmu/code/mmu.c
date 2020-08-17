#include "exynos_4412.h"
#include "uart.h"


extern void mmu_set(void);
/**********************************************************************
 * @brief		mydelay_ms program body
 * @param[in]	int (ms)
 * @return 		None
 **********************************************************************/
void mydelay_ms(int time)
{
	int i, j;
	while(time--)
	{
		for (i = 0; i < 5; i++)
			for (j = 0; j < 514; j++);
	}
}

/*
 * unsigned int vaddrstart 是要转换的虚拟起始地址
 * unsigned int vaddrend   是要转换的虚拟地址的结束地址
 * vaddrstart >> 20 把vaddrstart转换成section地址
 */
void mmu_setmtt(unsigned int vaddrstart, unsigned int vaddrend, unsigned int paddrstart, int attr)
{
		unsigned int *ptt;
		int i,nsection,tt;
		ptt = (unsigned int *)0x41000000+(vaddrstart >> 20);
		nsection = (vaddrend >> 20) - (vaddrstart >> 20);
		for(i = 0; i< nsection; i++)
		*ptt++ = attr | (((paddrstart >> 20) + i) << 20);
}

int main()
{		
	volatile int count;
	uart_init();

	mmu_setmtt(0x0,0xff000000,0x0,0xc12);  //全局的1:1的映射
	mmu_setmtt(0x91000000,0x92000000,0x11000000,0xc12);

	mmu_set(); //注意，调用这个函数时要保证ARM处于特权模式
	VGPX1.CON = (VGPX1.CON & ~(0xf)) | 1; //GPX1_0:output, LED3
	while(1){
        //Turn on LED3
        VGPX1.DAT |= 0x1;
        mydelay_ms(500);
        //Turn off LED3
        VGPX1.DAT &= ~0x1;
        mydelay_ms(500);
	}
/*
 //因为之前做过一次全局的1:1的映射，所以 mmu开启情况下，访问之前的实地址还是可以正常访问。
	GPX1.CON = (GPX1.CON & ~(0xf)) | 1; //GPX1_0:output, LED3
	while(1){
        //Turn on LED3
        GPX1.DAT |= 0x1;
        mydelay_ms(500);
        //Turn off LED3
        GPX1.DAT &= ~0x1;
        mydelay_ms(500);
	}
*/
}
