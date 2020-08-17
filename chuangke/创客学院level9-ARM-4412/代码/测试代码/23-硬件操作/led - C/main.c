#define GPX2CON  0x11000C40
#define GPX2DAT  0x11000C44

#define REG_READ(addr)        (*(volatile unsigned int*)(addr))
#define REG_WRITE(addr,val)   ((*(volatile unsigned int*)(addr)) = (val)) 
	
//1.设置寄存器
//2.做读写操作
//一般操作:
//[1].硬件初始化
//[2].硬件的操作(读/写)
//[3].退出操作(操作系统的)

void led_init() //初始化 --- 初始化为需要的状态
{
	//配置GPX2CON[31:28] = 0x1;
	int temp = REG_READ( GPX2CON); //(1).读
	temp &= ~(0xf<<28);  //(2).修改
	temp |= 0x1<<28;
	REG_WRITE(GPX2CON,temp);//(3).写
}	
void led_on()
{
		 //在GPX2DAT[7]	= 1;
	int temp = REG_READ( GPX2DAT); //(1).读
	temp &= ~(0x1<<7);  //(2).修改
	temp |= 0x1<<7;
	REG_WRITE(GPX2DAT,temp);//(3).写
}
void led_off()
{	
	//在GPX2DAT[7]	= 0;
	int temp = REG_READ( GPX2DAT); //(1).读
	temp &= ~(0x1<<7);  //(2).修改
	REG_WRITE(GPX2DAT,temp);//(3).写
}	


void delays()
{
	 int i = 1000000;
	 while(i--);
}




int main()
{
	
	while(1)
	{
		//1.关
		led_off();
		//2.延时
		delays();
		//3.开
		led_on();
		//4.延时
		delays();
	}
	return 0;
}