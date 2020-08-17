
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

	//����GPX2_7Ϊ���ģʽ,GPX2CON���4λ����Ϊ0x1
	REG_WRITE(GPX2CON,REG_READ(GPX2CON) & (~(0xf << 28)) | (0x1 << 28));
	//����GPX2DAT bit7Ϊ0������͵�ƽ��led2��
	REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1 << 7)));

	//ʵ��led2��˸
	//˼·����ѭ��������led2������1s��Ȼ�󱣳���1s��Ȼ������1s������
	while(1)
	{
		//��
		REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1<<7)) | (0x1<<7));
		//��ʱ��������1s
		mydelay_ms(1000);
		//��
		REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1<<7)));
		//��ʱ��������1s
		mydelay_ms(1000);
	}
	return 0;
}
