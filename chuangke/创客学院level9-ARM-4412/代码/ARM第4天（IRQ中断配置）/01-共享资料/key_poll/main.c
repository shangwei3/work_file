
#define GPX1CON 0x11000c20
#define GPX1DAT 0X11000c24
#define GPX1PUD 0x11000c28


#define GPX2CON 0x11000c40
#define GPX2DAT 0x11000c44


#define REG_READ(reg_addr) (*(volatile unsigned int*)(reg_addr))
#define REG_WRITE(reg_addr,val) ((*(volatile unsigned int*)(reg_addr))=(val))

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

	//设置GPX1_1为输出模式,GPX1CON [7:4]位设置为0x0
	REG_WRITE(GPX1CON,REG_READ(GPX1CON) & (~(0xf << 4)));
	//设置GPX1PUD bit[3:2]为0，GPX1_1内部上拉下拉禁止
	REG_WRITE(GPX1PUD,REG_READ(GPX1PUD) & (~(0x3 << 2)));

	//设置GPX2_7为输出模式,GPX2CON最高4位设置为0x1
	REG_WRITE(GPX2CON,REG_READ(GPX2CON) & (~(0xf << 28)) | (0x1 << 28));
	//设置GPX2DAT bit7为0，输出低电平，led2灭
	REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1 << 7)));

	//监测GPX1_1输入电平
	//思路：死循环，读GPX1DAT寄存器的值
	while(1)
	{
		if(!(REG_READ(GPX1DAT) & (0x1<<1)) )//GPX1DAT bit1 为低电平，按键KEY2摁下
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
		}
	}
	return 0;
}
