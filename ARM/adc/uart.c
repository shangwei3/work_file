/*************************************************************************
	> File Name: uart.c
	> Author: 
	> Mail: 
    > Created Time: 2018年01月23日 09时42分13秒 HKT
 ************************************************************************/

#include"exynos_4412.h"
#include"uart.h"

void uart_init(void)
{
    //引脚功能配置
        //使能引脚为串口功能
    GPA1.CON = (GPA1.CON & ~0xff) | 0x22; 

    //串口控制器配置
        //设置字长，校验，停止位
    UART2.ULCON2 = 0x03;
        //设置串口工作模式
    UART2.UCON2  = 0x05;
        //设置串口波特率 115200
    UART2.UBRDIV2 = 53;
    UART2.UFRACVAL2 = 4;
}

void putc(char c)
{
    while(!(UART2.UTRSTAT2 & 0x02));
    UART2.UTXH2 = c;
}

void puts(const char *p)
{
    while(*p != '\0')
    putc(*p++);
}



