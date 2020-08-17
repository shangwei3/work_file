#define  GPX2CON  0x11000c40
#define  GPX2DAT  0x11000c44
#define	 REG_READ(addr)  (*(volatile unsigned int *)(addr))
#define	 REG_WRITE(addr,val)  ((*(volatile unsigned int *)(addr))=(val))

void led_init()
{	
	unsigned int val=0xf;
	val=val<<28;
	REG_WRITE(GPX2CON,val);
	val=0x1;
	val=val<<28;
	REG_WRITE(GPX2CON,val);
}
void led_on()
{
	int on=0x1;
	on=on<<7;
	REG_WRITE(GPX2DAT,on);
}

void led_off()
{
	 int off=0x0;

	 REG_WRITE(GPX2DAT,off);
}

int delay(int time)
{
//	int i=1000;
//	int j=500;
	while(time--);
//	{
//		for(;j>0;j--)
//			for(;i>0;i--);
//	}
	return 0;
}
int main()
{
	unsigned int time=0x1000000;
	led_init();
	while(1)
	{
		led_on();
		delay(time);
		led_off();
		delay(time);
	}
	return 0;
}