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


#define GPA1CON 0X11400020
#define UBRDIV2 0x13820028
#define UFRACVAL2 0x1382002c
#define ULCON2 0X13820000
#define UCON2 0X13820004
#define UFCON2 0X13820008
#define UMCON2 0x1382000c
#define UTXH2 0X13820020
#define URXH2 0x13820024

#define UTRSTAT2 0X13820010

#define WTCON 0X10060000
#define WTDAT 0X10060004
#define WTCNT 0X10060008

#define GPD0CON 0x114000A0
#define TCFG0 0x139d0000
#define TCFG1 0x139d0004

#define TCNTB0 0x139d000c
#define TCMPB0 0X139D0010
#define TCON 0X139d0008

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

void wdt_init()
{
	//设置看门狗定时器复位定时时间为10s 100MHZ/256/128 = 3051 HZ
	REG_WRITE(WTCON,REG_READ(WTCON) | (0xff<<8) | (0x3 << 3) );
	REG_WRITE(WTDAT,10*3051);
	REG_WRITE(WTCNT,10*3051);
}

void wdt_start()
{
	REG_WRITE(WTCON,REG_READ(WTCON) | (1<<5) | (1<<0));
}

void data_abort_error_handler()
{
	//启动看门狗，10s后复位
	wdt_start();
	//做些事情，等待复位，比如让灯闪烁
	while(1)
	{
		//亮
		REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1<<7)) | (0x1<<7));
		mydelay_ms(500);
		//灭
		REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1<<7)));
		mydelay_ms(500);
	}

}


void pwm0_init()
{
	//GPD0设置TOUT
	REG_WRITE(GPD0CON,REG_READ(GPD0CON) & ~(0xf) | 0x2);
	//预分频 100MHZ/(199+1) = 500000hz
	REG_WRITE(TCFG0,REG_READ(TCFG0) & ~(0xff) | 199);
	//位分频 500000/2 = 250000hz
	REG_WRITE(TCFG1,REG_READ(TCFG1) & ~(0xf)| 0x1);

	//写TCNTB0 TCMPB0设置初始值
	REG_WRITE(TCNTB0, 10000);
	REG_WRITE(TCMPB0,9000);

	//第一次手动更新需要设置寄存器TCON[1], load TCNT TCMP
	REG_WRITE(TCON,REG_READ(TCON) | (0x1 << 1));

	//清除手动更新位，后面都是auto-reload
	REG_WRITE(TCON,REG_READ(TCON) & ~(0x1 << 1));

}

void start_pwm0_beep()
{
	//timer 0 enable,auto-reload bit set 1
	REG_WRITE(TCON,REG_READ(TCON) | (0x1) | (0x1<<3));
}

void stop_pwm0_beep()
{
	REG_WRITE(TCON,REG_READ(TCON) & ~(0x1));
}
/*-------------------------MAIN FUNCTION------------------------------*/
/**********************************************************************
 * @brief		Main program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int main(void)
{

	int *p;
	int val;

#if 1
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


#endif
	//设置GPX2_7为输出模式,GPX2CON最高4位设置为0x1
	REG_WRITE(GPX2CON,REG_READ(GPX2CON) & (~(0xf << 28)) | (0x1 << 28));
	//设置GPX2DAT bit7为0，输出低电平，led2灭
	REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1 << 7)));

	wdt_init();//看门狗初始化，设置看门狗定时器时间为10s

	pwm0_init();

	start_pwm0_beep();

	while(1)
	{
		mydelay_ms(1000);

		REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1<<7)));

		mydelay_ms(1000);

		REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1<<7)) | (0x1<<7));
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
