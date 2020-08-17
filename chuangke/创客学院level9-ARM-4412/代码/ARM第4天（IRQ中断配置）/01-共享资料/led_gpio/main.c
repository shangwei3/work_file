
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

	//设置GPX2_7为输出模式,GPX2CON最高4位设置为0x1
	REG_WRITE(GPX2CON,REG_READ(GPX2CON) & (~(0xf << 28)) | (0x1 << 28));
	//设置GPX2DAT bit7为0，输出低电平，led2灭
	REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1 << 7)));

	//实现led2闪烁
	//思路：死循环，先让led2保持亮1s，然后保持灭1s，然后再亮1s。。。
	while(1)
	{
		//亮
		REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1<<7)) | (0x1<<7));
		//延时，保持亮1s
		mydelay_ms(1000);
		//灭
		REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1<<7)));
		//延时，保持亮1s
		mydelay_ms(1000);
	}
	return 0;
}
