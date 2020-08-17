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
 * unsigned int vaddrstart ��Ҫת����������ʼ��ַ
 * unsigned int vaddrend   ��Ҫת���������ַ�Ľ�����ַ
 * vaddrstart >> 20 ��vaddrstartת����section��ַ
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

	mmu_setmtt(0x0,0xff000000,0x0,0xc12);  //ȫ�ֵ�1:1��ӳ��
	mmu_setmtt(0x91000000,0x92000000,0x11000000,0xc12);

	mmu_set(); //ע�⣬�����������ʱҪ��֤ARM������Ȩģʽ
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
 //��Ϊ֮ǰ����һ��ȫ�ֵ�1:1��ӳ�䣬���� mmu��������£�����֮ǰ��ʵ��ַ���ǿ����������ʡ�
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
