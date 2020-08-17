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
	/********************����GPIO�Ĵ�������**************************/
	//1.����GPX1PUD bit[3:2]Ϊ0��GPX1_1�ڲ�����������ֹ
	REG_WRITE(GPX1PUD,REG_READ(GPX1PUD) & (~(0x3 << 2)));
	//2.��GPX1_1��������Ϊ�ⲿ�жϹ���WAKEUP_INT1[1]	GPX1CON[7:4] = 0xf;
	REG_WRITE(GPX1CON,REG_READ(GPX1CON) & ~(0xf << 4) | (0xf << 4));
	//3.EXT_INT41_CON [6:4]����Ϊ�½��ش��� 0x2
	REG_WRITE(EXT_INT41_CON,REG_READ(EXT_INT41_CON) & ~(0x7 << 4) | (0x2 << 4));
	//4.EXT_INT41_MASK [1] ���� ʹ���ж�
	REG_WRITE(EXT_INT41_MASK,REG_READ(EXT_INT41_MASK) & (~(0x1 << 1)));

	/************************GIC�������Ĵ�������************************************/
	//1.ʹ��GIC
	REG_WRITE(ICDDCR,1);
	//2.ʹ����ص��ж�Դת������Ӧ��CPU�ӿ�
	REG_WRITE(ICDISER1_CPU0,REG_READ(ICDISER1_CPU0) & (~(0x1 << 25)) | (1<<25));
	//3.ѡ��CPU�ӿ� CPU0
	REG_WRITE(ICDIPTR14_CPU0,REG_READ(ICDIPTR14_CPU0) & (~(0xff << 8)) | (1<<8));
	//4.ʹ�������ж��źŵ�CPU0
	REG_WRITE(ICCICR_CPU0,1);
	//5.����CPU0�ܽ����ж����ȼ�ˮƽ
	REG_WRITE(ICCPMR_CPU0,0xff);


	//����GPX2_7Ϊ���ģʽ,GPX2CON���4λ����Ϊ0x1
	REG_WRITE(GPX2CON,REG_READ(GPX2CON) & (~(0xf << 28)) | (0x1 << 28));
	//����GPX2DAT bit7Ϊ0������͵�ƽ��led2��
	REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1 << 7)));

	//���GPX1_1�����ƽ
	//˼·����ѭ������GPX1DAT�Ĵ�����ֵ
	while(1)
	{
		mydelay_ms(10);//ȥ��
		/*if(!(REG_READ(GPX1DAT) & (0x1<<1)) )//GPX1DAT bit1 Ϊ�͵�ƽ������KEY2����
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
			{//��
				REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1<<7)) | (0x1<<7));
				status = 1;
			}else{
				//��
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
