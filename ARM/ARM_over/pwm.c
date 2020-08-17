#include "exynos_4412.h"

//pwm4 配置为定时器
void delay_init(void)
{
	//配置预分频为９９＋１
	PWM.TCFG0 = (PWM.TCFG0 & ~(0xff << 8)) | (99<<8);
	//配置固定分频1
	PWM.TCFG1 &= ~(0xf << 16);
	//设置重载值
	PWM.TCNTB4 = 0xffffffff;
	//使能更新
	PWM.TCON  |= (1<<21);
	//关闭更新
	PWM.TCON &= ~(1<<21);
	PWM.TCON  |= 1<<22;
	//启动ｐｗｍ４
	PWM.TCON |= 1<<20;
}


void pwm_init(unsigned int p,unsigned int b)
{
	//GPIO
	GPD0.CON = (GPD0.CON & ~(0xf) )| 0x2;
	//PWM 
	//预分频1
	PWM.TCFG0 = PWM.TCFG0 & (~(0xff))|0xf9;
	//设置固定分频 1/1
	PWM.TCFG1 = PWM.TCFG1 & (~(0xf)) | 0x1;
	//重载 1M
	PWM.TCNTB0 = p;
	//比较值
	PWM.TCMPB0 = b;
	//使能更新
	PWM.TCON  |= 1<<1;
	//关闭使能更新
	PWM.TCON  &= ~(1<<1);
	//使能自动重载和使能输出
	PWM.TCON  |= 3<<2;
}

void pwm_start(void)
{
	PWM.TCON |= 1;
}

void pwm_stop(void)
{
	PWM.TCON &= ~1;
}

void delayms(unsigned int ms)
{
	unsigned int us = PWM.TCNTO4;
	while( us - PWM.TCNTO4  < ms *1000);
}


