#define GPX1CON  0x11000c20
#define GPX1DAT  0x11000c24
#define    GPA1CON 	  0x11400020
#define    UBRDIV2	  0x13820028	  //设置波特率
#define    UFRACVAL2  0x1382002c
#define    ULCON2     0x13820000	  //数据校验
#define    UCON2      0x13820004	  //uart收发模式
#define    UFCON2     0x13820008      //关闭FIFO模式
#define    UMCON      0x1382000c
#define    UTXH2      0x13820020	  //发送数据
#define    URXH2      0x13820024
#define    UTRSTAT2   0x13820010	  //检验是否发出完成

#define  reg_read(addr)  (*(volatile unsigned long *)(addr))
#define  reg_write(addr,var) (*(volatile unsigned long *)(addr)=(var))


int uart2_init();
void delay_ms(volatile int ms);
void uart2_send_char(char data);

int main()
{
	 reg_write(GPX1CON,(reg_read(GPX1CON)&(~(0XF)))|(0X1));
     reg_write(GPX1DAT,reg_read(GPX1DAT)|(0X1));
	 uart2_init();
	 while(1)
	 {
	 	uart2_send_char('v');
		delay_ms(1000);
	 }	  
	 return 0;	
}


int uart2_init()
{
	reg_write(GPA1CON,(reg_read(GPA1CON)&~(0xff))|(0x22));	  //设置为UART_2_RXD
	reg_write(UBRDIV2,53) ;			 //设置波特率
	reg_write(UFRACVAL2,4) ;
	reg_write(ULCON2,0x3);			 //数据检验
	reg_write(UCON2,(reg_read(UCON2)&(~0Xf))|0X5);  //收发模式
	reg_write(UFCON2,reg_read(UFCON2)&(~1));		 //关闭FIFO模式
	reg_write(UMCON,reg_read(UMCON)|0X1);
	/*while(1){
	 reg_write(GPX1CON,(reg_read(GPX1CON)&(~(0XF)))|(0X1));
     reg_write(GPX1DAT,reg_read(GPX1DAT)|(0X1));} */	
	return 0;
} 

void do_irq(void)
{
}

void delay_ms(volatile int ms)
{
	int i,j;
	while(ms--)
	{
		for(i=0;i<5;i++)
			for(j=0;j<514;j++);
	}
}

void uart2_send_char(char data)
{
	  reg_write(UTXH2,data);
	  while(!(reg_read(UTRSTAT2)&2));
}

unsigned char uart2_recvive_char()
{
	unsigned char data;
	while(!((reg_read(UTRSTAT2)&1)==1));
	data= (unsigned char )(reg_read(URXH2)&0xff);
	return data;

}

