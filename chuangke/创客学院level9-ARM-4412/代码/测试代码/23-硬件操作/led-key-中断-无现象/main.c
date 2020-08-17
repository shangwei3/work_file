//GPIO --led
#define GPX2CON  0x11000C40
#define GPX2DAT  0x11000C44
//GPIO --key2
#define GPX1CON  0x11000C20
#define GPX1DAT  0x11000C24
#define  GPX1PUD  0x11000c28


//GPIO--interrupt
#define EXT_INT41_CON 0x11000E04
#define EXT_INT41_MASK 0x11000F04
#define EXT_INT41_PEND 0x11000F44

//GIC 
#define ICDISER1_CPU  0x10490104
#define ICDDCR        0x10490000

#define ICDIPTR14     0x10490838
#define ICCPMR_CPU0   0x10480004

#define ICCICR_CPU0   0x10480000
#define ICDISR1_CPU0  0x10490084

#define ICDICPR1_CPU0 0x10490284
#define ICCEOIR       0x10480010

#define REG_READ(addr)        (*(volatile unsigned int*)(addr))
#define REG_WRITE(addr,val)   ((*(volatile unsigned int*)(addr)) = (val)) 
	
#define ICCIAR_CPU0  0x1048000c
	
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



//1.中断初始化
void interrup_init()
{
	   //(1). muxed  ---选择为中断功能
     //GPX1CON[4:7] = 0b1111 //0xF = WAKEUP_INT1[7] 0x1100_0C20 
	   REG_WRITE(GPX1CON,REG_READ(GPX1CON)|(0xf<<4));	
	   REG_WRITE(GPX1PUD,(REG_READ(GPX1PUD)&(~(0x3<<2)))); 
	   //(2).引脚关于中断的配置
	   // falling  ---下降沿 触发中断
     //  EXT_INT41_CON[4:6] = 0b010       //  0x1100_0E04 (0x2)
	   REG_WRITE(EXT_INT41_CON,(REG_READ(EXT_INT41_CON)&(~(0x7<<4)))|(0x2<<4));
	   //(3).使能中断
     //EXT_INT41_MASK     = 0x0 ;       //  0x1100_0F04
	   REG_WRITE(EXT_INT41_MASK,REG_READ(EXT_INT41_MASK)&(~(0x1<<1)));
	  //(1).enable for distributor      //
	    //ICDISER1_CPU[25] = 0b1         //0x1049_0104
      //ICDDCR[0] = 0b1                //0x1049_0000
	  REG_WRITE(ICDISER1_CPU,(REG_READ(ICDISER1_CPU)&(~(0x1<<25))) | (0x1<<25));
	  REG_WRITE(ICDDCR,(REG_READ(ICDDCR)&(~0x1)) | (0x1));
	  //(2).target cpu0
     //  ICDIPTR14[8:15] = 0b1         //0x1049_0838   
	                                 //(57除4 商 14 余1) --
									                  //一个字节控制一个中断
																		
			REG_WRITE(ICDIPTR14,(REG_READ(ICDIPTR14)&(~(0xff<<8))) | (0x1<<8));
	   
      //(3).mask priority ----0xff 最低优先级的值
      //ICCPMR_CPU0[0:7] = 0xff        //0x1048_0004			
		  REG_WRITE(ICCPMR_CPU0,(REG_READ(ICCPMR_CPU0)&(~(0xff))) | (0xff));
     
	   //(4).enable for cpu0 interface
     //ICCICR_CPU0[0] = 0b1          //0x1048_0000
	    REG_WRITE(ICCICR_CPU0,(REG_READ(ICCICR_CPU0)&(~(0x1))) | (0x1));
      //(5). no secure
      //ICDISR1_CPU0[25] = 0b1         //0x1049_0084
      REG_WRITE(ICDISR1_CPU0,(REG_READ(ICDISR1_CPU0)&(~(0x1<<25))) | (0x1<<25));
}

int flag = 0;
int main()
{
	
 //	 int state;
	 led_init();
   key_init();
	 interrup_init();
		while(1)
		{				
				  led_off();
			    //delays();
			    //led_on();
		//	    delays();
		}
		
		return 0;
}


//中断处理函数

/*
void do_irq()
{
	int irq_num  = REG_READ(ICCIAR_CPU0)&0x3ff;
	
	switch(irq_num)
	{
		case 57:
			   if(flag == 0)
				 {
        //led_off();--->中断函数中 不调外部的函数
				    flag = 1;
				 }else if(flag == 1)
				 {
					//  led_on(); --->中断函数中 不调外部的函数
				    flag = 0;
				 }
				 
				 //清除 中断请求标志
				 REG_WRITE(EXT_INT41_PEND,REG_READ(EXT_INT41_PEND)|(0x1<<1));
				 REG_WRITE(ICDICPR1_CPU0,REG_READ(ICDICPR1_CPU0)|(0x1<<25));
          
         break;				 
		//case 
		default:
			break;
	}
	 REG_WRITE(ICCEOIR,(REG_READ(ICCEOIR)&(~0x3ff))|irq_num);
	
}
*/

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