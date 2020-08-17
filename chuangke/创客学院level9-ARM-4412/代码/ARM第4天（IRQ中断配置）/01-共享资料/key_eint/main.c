#define GPX1CON 0x11000c20
#define GPX1DAT 0X11000c24
#define GPX1PUD 0x11000c28
#define EXT_INT41_CON 0x11000e04
#define EXT_INT41_MASK 0x11000f04
#define EXT_INT41_PEND 0x11000f44

#define ICDDCR 0x10490000
#define ICDISER1_CPU0 0x10490104
#define ICDIPTR14_CPU0 0x10490838
#define ICCICR_CPU0 0x10480000
#define ICCPMR_CPU0 0x10480004
#define ICCIAR_CPU0 0x1048000c
#define ICDICPR1_CPU0 0x10490284
#define ICCEOIR_CPU0 0x10480010

#define GPX2CON 0x11000c40
#define GPX2DAT 0x11000c44


#define REG_READ(reg_addr) (*(volatile unsigned int*)(reg_addr))
#define REG_WRITE(reg_addr,val) ((*(volatile unsigned int*)(reg_addr))=(val))

int status = 0;

/**********************************************************************
 * @brief		mydelay_ms program body
 * @param[in]	int (ms)
 * @return 		None
 **********************************************************************/
void mydelay_ms(int ms)
{
	int i, j;
	while(ms--)
	{
		for (i = 0; i < 5; i++)
			for (j = 0; j < 514; j++);
	}
}

/*-------------------------MAIN FUNCTION------------------------------*/
/**********************************************************************
 * @brief		Main program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int main(void)
{
	/********************外设GPIO寄存器设置**************************/
	//1.设置GPX1PUD bit[3:2]为0，GPX1_1内部上拉下拉禁止
	REG_WRITE(GPX1PUD,REG_READ(GPX1PUD) & (~(0x3 << 2)));
	//2.将GPX1_1引脚设置为外部中断功能WAKEUP_INT1[1]	GPX1CON[7:4] = 0xf;
	REG_WRITE(GPX1CON,REG_READ(GPX1CON) & ~(0xf << 4) | (0xf << 4));
	//3.EXT_INT41_CON [6:4]设置为下降沿触发 0x2
	REG_WRITE(EXT_INT41_CON,REG_READ(EXT_INT41_CON) & ~(0x7 << 4) | (0x2 << 4));
	//4.EXT_INT41_MASK [1] 设置 使能中断
	REG_WRITE(EXT_INT41_MASK,REG_READ(EXT_INT41_MASK) & (~(0x1 << 1)));

	/************************GIC控制器寄存器设置************************************/
	//1.使能GIC
	REG_WRITE(ICDDCR,1);
	//2.使能相关的中断源转发给对应的CPU接口
	REG_WRITE(ICDISER1_CPU0,REG_READ(ICDISER1_CPU0) & (~(0x1 << 25)) | (1<<25));
	//3.选择CPU接口 CPU0
	REG_WRITE(ICDIPTR14_CPU0,REG_READ(ICDIPTR14_CPU0) & (~(0xff << 8)) | (1<<8));
	//4.使能所有中断信号到CPU0
	REG_WRITE(ICCICR_CPU0,1);
	//5.设置CPU0能接受中断优先级水平
	REG_WRITE(ICCPMR_CPU0,0xff);


	//设置GPX2_7为输出模式,GPX2CON最高4位设置为0x1
	REG_WRITE(GPX2CON,REG_READ(GPX2CON) & (~(0xf << 28)) | (0x1 << 28));
	//设置GPX2DAT bit7为0，输出低电平，led2灭
	REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1 << 7)));

	//监测GPX1_1输入电平
	//思路：死循环，读GPX1DAT寄存器的值
	while(1)
	{
		mydelay_ms(10);//去抖
		/*if(!(REG_READ(GPX1DAT) & (0x1<<1)) )//GPX1DAT bit1 为低电平，按键KEY2摁下
		{
			mydelay_ms(10);//去抖
			if(!(REG_READ(GPX1DAT) & (0x1<<1)) )//GPX1DAT bit1 为低电平，按键KEY2摁下
			{

				//亮
				REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1<<7)) | (0x1<<7));
				while(!(REG_READ(GPX1DAT) & (0x1<<1)));//等待按键起来
				//灭
				REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1<<7)));
			}
		}*/
	}
	return 0;
}

void do_irq(void)
{
	int irq_num;
	irq_num = (REG_READ(ICCIAR_CPU0)) & 0x3ff;

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
