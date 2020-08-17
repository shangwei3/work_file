
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

	//����GPX1_1Ϊ���ģʽ,GPX1CON [7:4]λ����Ϊ0x0
	REG_WRITE(GPX1CON,REG_READ(GPX1CON) & (~(0xf << 4)));
	//����GPX1PUD bit[3:2]Ϊ0��GPX1_1�ڲ�����������ֹ
	REG_WRITE(GPX1PUD,REG_READ(GPX1PUD) & (~(0x3 << 2)));

	//����GPX2_7Ϊ���ģʽ,GPX2CON���4λ����Ϊ0x1
	REG_WRITE(GPX2CON,REG_READ(GPX2CON) & (~(0xf << 28)) | (0x1 << 28));
	//����GPX2DAT bit7Ϊ0������͵�ƽ��led2��
	REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1 << 7)));

	//���GPX1_1�����ƽ
	//˼·����ѭ������GPX1DAT�Ĵ�����ֵ
	while(1)
	{
		if(!(REG_READ(GPX1DAT) & (0x1<<1)) )//GPX1DAT bit1 Ϊ�͵�ƽ������KEY2����
		{
			mydelay_ms(10);//ȥ��
			if(!(REG_READ(GPX1DAT) & (0x1<<1)) )//GPX1DAT bit1 Ϊ�͵�ƽ������KEY2����
			{

				//��
				REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1<<7)) | (0x1<<7));
				while(!(REG_READ(GPX1DAT) & (0x1<<1)));//�ȴ���������
				//��
				REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1<<7)));
			}
		}
	}
	return 0;
}
