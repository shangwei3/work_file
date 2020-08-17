#include "exynos_4412.h"

void led_init(void)
{
	//led2
	GPX2.CON = (GPX2.CON & ~(0xf<<28)) | (1<<28);
	//led3
	GPX1.CON = (GPX1.CON & ~(0xf<<0)) | (1<<0);
	//led4
//	GPF3.CON = (GPF3.CON & ~(0xf<<16)) | (1<<16);
//	GPF3.CON = (GPF3.CON & ~(0xf<<20)) | (1<<20);
	GPF3.CON = (GPF3.CON & ~(0xff<<16)) | (0x11<<16);	
}


/*******************************************
 *  作用:控制led
 *  参数:con   位控制灯的亮灭  0b0011 表示led2 led3 亮
 *		 ctr   为真  表示 con 中亮有效
 *			   为假  表示 con 中灭有效
 * *****************************************/
void led(unsigned char con,unsigned char ctr)
{
	char i ;
	if(ctr)
	{
		//亮有效
		if( con & 1)
		{  GPX2.DAT |= 1<<7; }
		if( con & 1<<1)
		{ 	GPX1.DAT |= 1<<0; }
		if( con & 1<<2)
		{ 	GPF3.DAT |= 1<<4; }
		if( con & 1<<3)
		{   GPF3.DAT |= 1<<5; }
	}
	else 
	{
		//灭有效
		if(!(con & 1))
		{
			GPX2.DAT &= ~(1<<7); 
		}

		if(!( con & 1<<1))
		{
			GPX1.DAT &= ~(1<<0); 
		}

		if( !(con & 1<<2))
		{
			GPF3.DAT &= ~(1<<4); 
		}
		if(!( con & 1<<3))
		{
			GPF3.DAT &= ~(1<<5); 
		}
	}

}


unsigned char get_led_flag(void)
{
	unsigned char tmp = 0;
    if(GPX2.DAT & (1<<7))
	{
		tmp |= 1;
	}
	if(GPX1.DAT & 1)
	{
		tmp |= 1<<1;
	}
	if(GPF3.DAT & (1<<4))
	{
		tmp |= 1<<2;
	}
	if(GPF3.DAT & (1<<5))
	{
		tmp |= (1<<3);
	}
	return tmp;
}

