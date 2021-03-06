#define GPX2CON 0x11000c40	//灯
#define GPX2DAT 0x11000c44

#define GPX1CON 0x11000c20	//按键	INIT
#define GPX1DAT 0x11000c24
#define GPX1PUD 0x11000c28
#define EXT_INT41_CON 0x11000e04
#define EXT_INT41_MASK 0x11000f04
#define EXT_INT41_PEND 0x11000f44

#define ICDDCR 0x10490000		//GIC  
#define ICDISER1_CPU0 0x10490104
#define ICDIPTR14_CPU0 0x10490838
#define ICCICR_CPU0  0x10480000
#define ICCPMR_CPU0  0x10480004
#define ICCIAR_CPU0	 0x1048000c
#define ICDICPR1_CPU0 0x10490284
#define ICCEOIR_CPU0 0x10480010
//#define GPF3CON 0x114001E0
//#define GPF3DAT 0x114001E4

#define REG_READ(addr) (*(volatile unsigned int *)(addr))
#define REG_WRITE(addr,val) (*(volatile unsigned int *)(addr)=(val))

int status=0;
				

void delay(volatile unsigned long dly)
{
	int i,j;
	while(dly--)
	{
		for(i=0;i<5;i++)
			for(j=0;j<514;j++);
	}
}

void do_irq(void)
{
	int irq_num;
	irq_num = (REG_READ(ICCIAR_CPU0)) & 0x3ff;//获取中断号

	switch(irq_num)
	{
		case 57:
			if(status == 0)
			{//亮
				REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1<<7)) | (0x1<<7));
				status = 1;
			}else{
			  //灭
			   REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1<<7)));
			   status = 0;
			}
			//clear pend
			REG_WRITE(EXT_INT41_PEND,REG_READ(EXT_INT41_PEND) | (1 << 1));
			REG_WRITE(ICDICPR1_CPU0,REG_READ(ICDICPR1_CPU0) | (1 << 25));
			break;
		default:
			break;
	}
	//clear cpu0 interrupt
	REG_WRITE(ICCEOIR_CPU0,(REG_READ(ICCEOIR_CPU0) & (~(0x3ff)) | irq_num ));
}

int main(void)
{
	//GPIO INIT
	REG_WRITE(GPX1PUD,REG_READ(GPX1PUD)&(~(0X3<<2)));  //禁止上拉下拉
	REG_WRITE(GPX1CON,REG_READ(GPX1CON)|(0XF<<4));     //设置为外部中断
	REG_WRITE(EXT_INT41_CON,REG_READ(EXT_INT41_CON)&(~(0X7<<4))|(0X2<<4)); //下降沿触发
	REG_WRITE(EXT_INT41_MASK,REG_READ(EXT_INT41_MASK)&(~(1<<1)));			//设置使能中断
	//GIC INIT
	REG_WRITE(ICDDCR,1); //使能GIC
	REG_WRITE(ICDISER1_CPU0,REG_READ(ICDISER1_CPU0)&(~(0X1<<25))|(0X1<<25));	//中断源到分配器
	REG_WRITE(ICDIPTR14_CPU0,REG_READ(ICDIPTR14_CPU0)&(~(0XFF<<8))|1<<8);       //选择CPU接口
	REG_WRITE(ICCICR_CPU0,1);
	REG_WRITE(ICCPMR_CPU0,0xff);
	REG_WRITE(GPX2CON,REG_READ(GPX2CON)&(~(0XF<<28))|(0X1<<28));//设置为输出模式
	REG_WRITE(GPX2DAT,REG_READ(GPX2DAT)|(~(0X1<<7))); // LED2灭
	
	while(1)
	{
	 	delay(10);	
	}
	 return 0;
}


