//GPIO --led
#define GPX2CON  0x11000C40
#define GPX2DAT  0x11000C44
//GPIO --key2
#define GPX1CON  0x11000C20
#define GPX1DAT  0x11000C24


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


//1.硬件初始化
void key_init()
{
	 	//配置GPX1CON[7:4] = 0x0;
	int temp = REG_READ(GPX1CON); //(1).读
	temp &= ~(0xf<<4);  //(2).修改
	temp |= 0x0<<4;
	REG_WRITE(GPX1CON,temp);//(3).写
}

//2.硬件操作
int get_key_state()
{
   int state = REG_READ(GPX1DAT);
	
	 return state;
}
int main()
{
	
	int state;
	 led_init();
  key_init();
		while(1)
		{		
			//1.按键如果按下 --- 亮		
			state = REG_READ(GPX1DAT);  //(轮询方式)
			if(state & 0x2) // !0 ---> 没有按下  //  0 ---> 按下 
      {				              
				   led_off();
			}else if(!(state &0x2))
      {
				  led_on();
				  while(!(REG_READ(GPX1DAT)&0x2)); //松开按键
			}
			//2.按键如果再次按下 --- 灭 
		}
		
		return 0;
}