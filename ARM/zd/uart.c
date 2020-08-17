#include"exynos_4412.h" 

void putc(char c)
{
	while( !(UART2.UTRSTAT2 & 2));
	UART2.UTXH2	= c;
}

void puts(char *s)
{
	while(*s != 0)
	putc(*s++);
}

void uart_init(void)
{
	
	GPA1.CON = (GPA1.CON & ~(0xff)) | 0x22;
	UART2.ULCON2 = 3;
	UART2.UCON2  =(UART2.UCON2 & ~(0xf)) | 0x5;
	UART2.UBRDIV2  =53;
	UART2.UFRACVAL2=4 ;

}
