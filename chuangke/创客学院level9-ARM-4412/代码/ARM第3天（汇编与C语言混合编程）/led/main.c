//�Ĵ�������
#define GPX2CON 0x11000C40
#define GPX2DAT 0x11000C44

//��װ�ԼĴ����Ĳ���(����д)
#define  REG_READ(reg_addr) (*((volatile unsigned int *)(reg_addr)))
#define  REG_WRITE(reg_addr,val) (*(volatile unsigned int *)(reg_addr) = val)

void led_init() {
	//1.��ʼ�� GPIO������---д���õ�ֵ���Ĵ�����(ע��ԼĴ����Ĳ�������--�޸�--д)
	REG_WRITE(GPX2CON,REG_READ(GPX2CON)&(~0xf<<28)|0x1<<28);
	//2.��ʼ�� GPIO���ݼĴ���---��ʼ��ʱ����Ϊ���״̬
	REG_WRITE(GPX2DAT,REG_READ(GPX2DAT)&(~(0x1<<7)));
}

//����
void led_on() {
	REG_WRITE(GPX2DAT,REG_READ(GPX2DAT)&(~(0x1<<7))|1<<7);
}
//�ص�
void led_off() {
	REG_WRITE(GPX2DAT,REG_READ(GPX2DAT)&(~(0x1<<7)));
}
//��ʱ

void delay(int time) {
	int i = 0, j = 0;
	while (time--) {
		for (; i < 10; i++) {
			for (; j < 5000; j++)
				;
		}
	}
}
void main() {
	//1.Ӳ����ʼ��
	led_init();
	//2.����ʵ��
	while (1) {
		//1.led on
		led_off();
		//2.keep a while
		delay(50000);
		//3.led off
		led_on();
		//4.keep a while
		delay(50000);
	}
}
