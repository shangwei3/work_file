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

#define GPBCON 0x11400040
#define I2CCON5 0x138b0000
#define I2CSTAT5 0x138b0004
#define I2CDS5 0x138b000c

//****************************************
//  MPU6050内部地址
//****************************************
#define SMPLRT_DIV      0x19    //陀螺仪采样率，典型值：0x07(125Hz)
#define CONFIG          0x1A    //低通滤波频率，典型值：0x06(5Hz)
#define GYRO_CONFIG     0x1B    //陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define ACCEL_CONFIG    0x1C    //加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define ACCEL_XOUT_H    0x3B
#define ACCEL_XOUT_L    0x3C
#define ACCEL_YOUT_H    0x3D
#define ACCEL_YOUT_L    0x3E
#define ACCEL_ZOUT_H    0x3F
#define ACCEL_ZOUT_L    0x40
#define TEMP_OUT_H      0x41
#define TEMP_OUT_L      0x42
#define GYRO_XOUT_H     0x43
#define GYRO_XOUT_L     0x44
#define GYRO_YOUT_H     0x45
#define GYRO_YOUT_L     0x46
#define GYRO_ZOUT_H     0x47
#define GYRO_ZOUT_L     0x48
#define PWR_MGMT_1      0x6B    //电源管理，典型值：0x00(正常启用)
#define WHO_AM_I        0x75    //IIC地址寄存器(默认数值0x68，只读)
#define SlaveAddress    0xD0    //IIC写入时的地址字节数据，+1为读取


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
	//1.GPA1_0和GPA1_1设置为uart功能
	REG_WRITE(GPA1CON,REG_READ(GPA1CON) & ~(0Xff) | (0x22));

	//2.uart clock 100MHZ input ,设置波特率为115200bps
	REG_WRITE(UBRDIV2,53);
	REG_WRITE(UFRACVAL2,4);

	//3.8data bit，no check， 1 bit stop
	REG_WRITE(ULCON2,0x3);

	//4.发送接收 poll模式
	REG_WRITE(UCON2,REG_READ(UCON2) & ~0xf | 0x5);

	//5.关闭FIFO
	REG_WRITE(UFCON2,REG_READ(UFCON2) & ~0x1);

	//6. 没想到
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

void i2c5_init()
{
	//引脚初始化 I2C SCL5 SDA5
	REG_WRITE(GPBCON,REG_READ(GPBCON) & ~(0Xff << 8) | (0x33 << 8));


	REG_WRITE(I2CSTAT5, 0XD0 );

	//中断使能，清除中断标志
	REG_WRITE(I2CCON5,REG_READ(I2CCON5) & ~(1 << 4) );

}

void iic5_write(char salve_addr,char reg_addr,char data)
{
	//Write slave address to I2CDS before start.
	REG_WRITE(I2CDS5,salve_addr);
	/*ENABLE ACK BIT, PRESCALER:512, ,ENABLE RX/TX */
	REG_WRITE(I2CCON5, 1<<7 | 1<<6 | 1<<5);
	//Write 0xF0 (M/T Start) to I2CSTAT [7:6] = 0b11,[5:4] = 0b11,启动信号发生
	REG_WRITE(I2CSTAT5,(0x3 << 6) | (1<<5) | (1<<4));
	//等待传输结束，在从设备应答之后，传输结束， I2CCON5[4] = 1，中断产生，i2c传输停止
	while(!(REG_READ(I2CCON5) & (1<<4)));

	//将寄存器地址写入I2CDS5,等待被传输
	REG_WRITE(I2CDS5,reg_addr);
	//clear interrupt pending ，I2CCON[4] 写 0，传输继续
	REG_WRITE(I2CCON5,REG_READ(I2CCON5) & ~(0x1 << 4));
	//等待传输结束，在从设备应答之后，传输结束， I2CCON5[4] = 1，中断产生，i2c传输停止
	while(!(REG_READ(I2CCON5) & (1<<4)));

	//将要写入的数据写入I2CDS5,等待被传输
	REG_WRITE(I2CDS5,data);
	//clear interrupt pending ，I2CCON[4] 写 0，传输继续
	REG_WRITE(I2CCON5,REG_READ(I2CCON5) & ~(0x1 << 4));
	//等待传输结束，在从设备应答之后，传输结束， I2CCON5[4] = 1，中断产生，i2c传输停止
	while(!(REG_READ(I2CCON5) & (1<<4)));


	//Write 0xd0 (M/T Start) to I2CSTAT [5:4] = 0b01,停止信号发生
	REG_WRITE(I2CSTAT5,0xd0);
	//clear interrupt pending ，I2CCON[4] 写 0
	REG_WRITE(I2CCON5,REG_READ(I2CCON5) & ~(0x1 << 4));
	//Wait until the stop condition takes effect.
	mydelay_ms(10);
}

void iic5_read(char slave_addr,char reg_addr,char *data)
{
	//Write slave address to I2CDS before start.
	REG_WRITE(I2CDS5,slave_addr);
	/*Master Trans mode ,START ,ENABLE RX/TX ,*/
	REG_WRITE(I2CCON5,REG_READ(I2CCON5) | 1<<7 | 1<<6 | 1<<5);/*ENABLE ACK BIT, PRESCALER:512, ,ENABLE RX/TX */

	//Write 0xF0 (M/T Start) to I2CSTAT [7:6] = 0b11,[5:4] = 0b11,启动信号发生
	REG_WRITE(I2CSTAT5, 0xf0);
	//等待传输结束，在从设备应答之后，传输结束， I2CCON5[4] = 1，中断产生，i2c传输停止
	while(!(REG_READ(I2CCON5) & (1<<4)));

	//将寄存器地址写入I2CDS5,等待被传输
	REG_WRITE(I2CDS5,reg_addr);
	//clear interrupt pending ，I2CCON[5] 写 0，传输继续
	REG_WRITE(I2CCON5,REG_READ(I2CCON5) & ~(0x1 << 4));
	//等待传输结束，在从设备应答之后，传输结束， I2CCON5[4] = 1，中断产生，i2c传输停止
	while(!(REG_READ(I2CCON5) & (1<<4)));

	//Write 0xF0 (M/T Start) to I2CSTAT [5:4] = 0b01,停止信号发生
	REG_WRITE(I2CSTAT5,0xd0);

	//Write slave address to I2CDS before start.bit 0 = 1 for read
	REG_WRITE(I2CDS5,slave_addr | 0x01);
	REG_WRITE(I2CCON5,1<<7 | 1<<6 | 1<<5);/*ENABLE ACK BIT, PRESCALER:512, ,ENABLE RX/TX */
	//Write 0xF0 (M/T Start) to I2CSTAT [7:6] = 0b10,master receive moe
	//[5:4] = 0b11,启动信号发生,使能数据传输
	REG_WRITE(I2CSTAT5,2<<6 | 1<<5 | 1<<4);
	//等待传输结束，在从设备应答之后，传输结束， I2CCON5[4] = 1，中断产生，i2c传输停止
	while(!(REG_READ(I2CCON5) & (1<<4)));

	//I2CCON5[7] = 0 NO ACK.[4] = 0 clear interrupt pending
	REG_WRITE(I2CCON5,REG_READ(I2CCON5) & ~((1<<7)|(1 << 4))); // I2CCON [4]位清0，继续传输，接收数据，
	while(!(REG_READ(I2CCON5) & (1<<4))); // 等待传输结束

	REG_WRITE(I2CSTAT5, 0x90);//停止信号
	//clear interrupt pending ，I2CCON[5] 写 0
	REG_WRITE(I2CCON5,REG_READ(I2CCON5) & ~(1 << 4));

	*data = REG_READ(I2CDS5);

	//Wait until the stop condition takes effect.
	mydelay_ms(10);
}

void MPU6050_Init ()
{
    iic5_write(SlaveAddress, PWR_MGMT_1, 0x00);
    iic5_write(SlaveAddress, SMPLRT_DIV, 0x07);
    iic5_write(SlaveAddress, CONFIG, 0x06);
    iic5_write(SlaveAddress, GYRO_CONFIG, 0x18);
    iic5_write(SlaveAddress, ACCEL_CONFIG, 0x01);
}
int get_data(unsigned char addr)
{
    char data_h, data_l;
    iic5_read(SlaveAddress, addr, &data_h);
    iic5_read(SlaveAddress, addr+1, &data_l);
    return (data_h<<8)|data_l;
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

	i2c5_init();
	mydelay_ms(100);
	uart2_init();
	mydelay_ms(100);
	MPU6050_Init();
	mydelay_ms(100);
	//监测GPX1_1输入电平
	//思路：死循环，读GPX1DAT寄存器的值
	while(1)
	{
		//x
		//led on
		REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) | (0x1 << 7));
		adcval = get_data(GYRO_XOUT_H);

		mydelay_ms(20);
		//adcval = get_data(GYRO_XOUT_H);
		uart_printf(" GYRO--X: %d\n", adcval);
		//led off
		REG_WRITE(GPX2DAT,REG_READ(GPX2DAT) & ~(0x1 << 7));

		mydelay_ms(500);

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
*/
}
