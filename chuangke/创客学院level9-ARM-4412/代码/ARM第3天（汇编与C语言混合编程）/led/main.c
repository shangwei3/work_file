//寄存器定义
#define GPX2CON 0x11000C40
#define GPX2DAT 0x11000C44

//封装对寄存器的操作(读、写)
#define  REG_READ(reg_addr) (*((volatile unsigned int *)(reg_addr)))
#define  REG_WRITE(reg_addr,val) (*(volatile unsigned int *)(reg_addr) = val)

void led_init() {
	//1.初始化 GPIO控制器---写配置的值到寄存器中(注意对寄存器的操作：读--修改--写)
	REG_WRITE(GPX2CON,REG_READ(GPX2CON)&(~0xf<<28)|0x1<<28);
	//2.初始化 GPIO数据寄存器---初始化时，灯为灭的状态
	REG_WRITE(GPX2DAT,REG_READ(GPX2DAT)&(~(0x1<<7)));
}

//开灯
void led_on() {
	REG_WRITE(GPX2DAT,REG_READ(GPX2DAT)&(~(0x1<<7))|1<<7);
}
//关灯
void led_off() {
	REG_WRITE(GPX2DAT,REG_READ(GPX2DAT)&(~(0x1<<7)));
}
//延时

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
	//1.硬件初始化
	led_init();
	//2.功能实现
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
