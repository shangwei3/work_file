
/*#define GPX1CON 0x11000c20
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
*/
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

#define ADCCON 0x126c0000
#define ADCMUX 0x126c001c
#define ADCDAT 0X126C000c

#define REG_READ(reg_addr) (*(volatile unsigned int*)(reg_addr))
#define REG_WRITE(reg_addr,val) ((*(volatile unsigned int*)(reg_addr))=(val))

int status = 0;
int adcval = 0;

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

void uart2_init()
{
	//1.GPA1_0��GPA1_1����Ϊuart����
	REG_WRITE(GPA1CON,REG_READ(GPA1CON) & ~(0Xff) | (0x22));

	//2.uart clock 100MHZ input ,���ò�����Ϊ115200bps
	REG_WRITE(UBRDIV2,53);
	REG_WRITE(UFRACVAL2,4);

	//3.8data bit��no check�� 1 bit stop
	REG_WRITE(ULCON2,0x3);

	//4.���ͽ��� pollģʽ
	REG_WRITE(UCON2,REG_READ(UCON2) & ~0xf | 0x5);

	//5.�ر�FIFO
	REG_WRITE(UFCON2,REG_READ(UFCON2) & ~0x1);

	//6. û�뵽
#if 0
	// disable AFC
    UMCON[4] = 0b0

    // RTS active
    UMCON[0] = 0b1
#endif

}

void uart2_send_char(char data)
{
	  // write fifo
	  REG_WRITE(UTXH2, data);

	  // wait finish
	  while (!(REG_READ(UTRSTAT2) & 0x2));
}

unsigned char uart2_receive_char()
{
	unsigned char data;
	//WAIT data ready
	while(!(REG_READ(UTRSTAT2) & 0X1));
	data = (unsigned char)(REG_READ(URXH2) & 0xff);
	return data;
}

void put_string(char * str)
{
	while(*str != '\0')
		uart2_send_char(*str++);
}

void get_string(char * st)
{
	char da;
	int i=1;
	while(i<6){
		da = uart2_receive_char();
		//if(da == '\0')
			//break;
		*st++ = da;
		i++;
	}

}


void adc_init()
{
	//enable prescaler
	REG_WRITE(ADCCON,REG_READ(ADCCON) | (1 << 14));
	//prescaler 256
	REG_WRITE(ADCCON,REG_READ(ADCCON) & ~(0Xff << 6) | (0xff << 6));
	//���þ���
	REG_WRITE(ADCCON,REG_READ(ADCCON) | (1 << 16));

	REG_WRITE(ADCMUX,REG_READ(ADCMUX) & ~(0xf) | (0x3));

	REG_WRITE(ADCCON,REG_READ(ADCCON) & ~(1 << 2));

	REG_WRITE(ADCCON,REG_READ(ADCCON) | ( 1 << 1));

	adcval = REG_READ(ADCDAT);

}

int adc_read()
{
	int val;
	while(!(REG_READ(ADCCON) & (1 << 15)));
	val = REG_READ(ADCDAT) & 0xfff;
	return val;
}


/*-------------------------MAIN FUNCTION------------------------------*/
/**********************************************************************
 * @brief		Main program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int main(void)
{
	char str[10] = {0};
#if 0
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

#endif
	//����GPX2_7Ϊ���ģʽ,GPX2CON���4λ����Ϊ0x1
	REG_WRITE(GPX2CON,REG_READ(GPX2CON) & (~(0xf << 28)) | (0x1 << 28));
	//����GPX2DAT bit7Ϊ0������͵�ƽ��led2��
	REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & (~(0x1 << 7)));

	adc_init();

	//���GPX1_1�����ƽ
	//˼·����ѭ������GPX1DAT�Ĵ�����ֵ
	while(1)
	{
		adcval = adc_read();
		mydelay_ms(500);//ȥ��
		uart_printf("adc = %d\n",adcval);
	}
	return 0;
}

void do_irq(void)
{
	/*int irq_num;
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
*/
}
