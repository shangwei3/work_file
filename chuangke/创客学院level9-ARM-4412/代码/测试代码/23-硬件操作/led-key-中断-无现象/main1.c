#define    GPX2CON  0x11000C40
#define    GPX2DAT  0x11000C44
#define    GPX1CON  0x11000C20
#define    GPX1PUD  0x11000c28
#define    GPX1DAT  0x11000C24

//---------GPIO-----------------------
//(1). muxed  ---选择为中断功能
#define GPX1CON    0x11000C20
//(2).引脚关于中断的配置
// falling
#define EXT_INT41_CON  0x11000E04
//(3).使能中断
#define  EXT_INT41_MASK 0x11000F04
//(4).清除中断时用   写"1"表示清除
#define   EXT_INT41_PEND  0x11000F44
//----------GIC-----------------------
//(1).enable for distributor      //
#define   ICDISER1_CPU 0x10490104 //??? 需要关注
#define   ICDDCR       0x10490000
//(2).target cpu0	    
#define   ICDIPTR14    0x10490838   
//(3).mask priority ----0xff 最低优先级的值
#define   ICCPMR_CPU0  0x10480004
//(4).enable for cpu0 interfacex
#define   ICCICR_CPU0  0x10480000	  
//(5).no secure
#define   ICDISR1_CPU0 0x10490084
//(6).清除操作
#define	  ICDICPR1_CPU0 0x10490284
#define	  ICCEOIR       0x10480010
#define   ICCIAR_CPU0   0x1048000c 

//对于硬件的操作，寄存器操作
//读
#define REG_READ(addr) (*((volatile unsigned int * )addr))
//写
#define REG_WRITE(addr,val)  ((*((volatile unsigned int * )addr)) = val)

int flag = 0; //灭

//---------------------------------led control start -------------------------------------------
//(1).初始化
void led_init()
{
   unsigned int temp = REG_READ(GPX2CON); 
   temp = (~(0xf<<28))&temp;
   temp = (0x1 << 28)|temp;
   REG_WRITE(GPX2CON,temp);

}
//(2).开灯
void led_on()
{
   unsigned int temp = REG_READ(GPX2DAT); 
   temp = (~(0x1<<7))&temp;
   temp = (0x1 << 7)|temp;
   REG_WRITE(GPX2DAT,temp);
}
//(3).关灯
void led_off()
{
   unsigned int temp = REG_READ(GPX2DAT); 
   temp = (~(0x1<<7))&temp;
   temp = (0x0 << 7)|temp;
   REG_WRITE(GPX2DAT,temp);
}
//---------------------------------led control end -------------------------------------------
void key_init()
{
   unsigned int temp = REG_READ(GPX1CON); 
   temp = (~(0xf<<4))&temp; //清零	---0x0 --- input模式
   //temp = (0x0 << 4)&temp;
   REG_WRITE(GPX1CON,temp);

}
int key_check()
{
     int ret = REG_READ(GPX1DAT);

	 //GPX1DAT[1] == 1 表示没有按下
	 if((ret & 0x2) == 0)
	 {
	   return 1; //按下
	 }else 
	 {
	    return 0; //表示没有按下
	 }
	 //GPX1DAT[1] == 0 表示按下 
}

int delays()
{
   //int i = ;
   int y = 10000000;
   for(; y > 0; y--)
       ;
	   
   return 0;
}
void interupt_init()
{
     //(1).gpio 配置---------------------------------------------
			//(1). muxed  ---选择为中断功能 
			REG_WRITE(GPX1CON,(REG_READ(GPX1CON)&(~(0xf<<4)))|(0xf<<4)); 
			REG_WRITE(GPX1PUD,(REG_READ(GPX1PUD)&(~(0x3<<2)))); 
			//(2).引脚关于中断的配置
			REG_WRITE(EXT_INT41_CON,(REG_READ(EXT_INT41_CON)&(~(0x7<<4)))|(0x2<<4)); 
			//(3)使能中断 
			REG_WRITE(EXT_INT41_MASK,(REG_READ(EXT_INT41_MASK)&(~(0x1<<1))));
	 //(2).GIC  配置 ---------------------------------------------
			//(1).enable for distributor   
			REG_WRITE(ICDDCR,REG_READ(ICDDCR) |0x1);
			REG_WRITE(ICDISER1_CPU,(REG_READ(ICDISER1_CPU)&(~(0x1<<25))) | (0x1<<25));  
			//(2).target cpu0
			REG_WRITE(ICDIPTR14,(REG_READ(ICDIPTR14)&(~(0xFF<<8))) | (0x1<<8)); 
			//(4).enable for cpu0 interface
			REG_WRITE( ICCICR_CPU0,REG_READ( ICCICR_CPU0) |0x1);
    	//(3).mask priority ----0xff 最低优先级的值 
			REG_WRITE(ICCPMR_CPU0,(REG_READ(ICCPMR_CPU0)&(~(0xFF))) | 0xff);
		
			//(5). no secure
			REG_WRITE(ICDISR1_CPU0,(REG_READ(ICDISR1_CPU0)&(~(0x1<<25))) | (0x1<<25)); 
	}

int main()
{ 

   //【1】.硬件的初始化
   //      1.初始化 对应的GPIO口
    led_init();
  	key_init();
  	interupt_init();//中断初始化
    led_off();//灭
	
		while(1)
		{
				led_on(); //亮
				delays();
				led_off(); //亮
				delays();
		}
   return 0;
}


//void do_irq(void)

//处理中断
void do_irq(void)
{
  //1.获取中断号
  unsigned int temp = 0;
	int irq_num = REG_READ(ICCIAR_CPU0)&0x3ff; // 0b11 1111 1111 

	switch(irq_num)
	{
	    case 57:
	     if(flag == 0)
			 {
		       //按下---亮
			     temp = REG_READ(GPX2DAT); 
			     temp = (~(0x1<<7))&temp;
			     temp = (0x1 << 7)|temp;
			     REG_WRITE(GPX2DAT,temp);
				 flag = 1;
			 } else 
			 {
			       //不按---灭 
			     REG_READ(GPX2DAT); 
				   temp = (~(0x1<<7))&temp;
				   temp = (0x0 << 7)|temp;
				   REG_WRITE(GPX2DAT,temp);
				   flag = 0;
		     }
			     //清除中断
			     REG_WRITE(EXT_INT41_PEND,(REG_READ( EXT_INT41_PEND)&(~(0x1<<1))) | (0x1<<1));
		       REG_WRITE(ICDICPR1_CPU0 ,(REG_READ(ICDICPR1_CPU0)&(~(0x1<<25)))  | (0x1<<25));
			 break;

		default:
		     break;
	}
	REG_WRITE(ICCEOIR,(REG_READ(ICCEOIR) & (~(0x3ff)) | irq_num ));	
	//清除结束中断信号
  //REG_WRITE(ICCEOIR,REG_READ(ICCIAR_CPU0) | irq_num);
}