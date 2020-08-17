#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mpu6050.h"

int mpu6050_write(int fd,unsigned char regaddr,unsigned char regval)
{
	struct i2c_msg msg[1];
	struct i2c_rdwr_ioctl_data rdwr_arg;
	unsigned char tx_buf[2] = {regaddr,regval};
	

	msg[0].addr  = MPU6050_SLAVE_ADDR;
	msg[0].flags = 0;//写从机
	msg[0].len   = sizeof(tx_buf)/sizeof(tx_buf[0]);
	msg[0].buf   = tx_buf;
	

	rdwr_arg.msgs = msg;
	rdwr_arg.nmsgs = sizeof(msg)/sizeof(msg[0]);

	if(ioctl(fd,I2C_RDWR,&rdwr_arg) < 0){
		perror("Fail to ioctl");
		return -1;
	}

	printf("okk???\n");
	
	return 0;
}

int mpu6050_read(int fd,unsigned char regaddr)
{
	struct i2c_msg msg[2];
	unsigned char rx_buf[1];
	unsigned char tx_buf[1] = {regaddr};
	struct i2c_rdwr_ioctl_data rdwr_arg;

	msg[0].addr  = MPU6050_SLAVE_ADDR;
	msg[0].flags = 0;//写从机
	msg[0].len   = sizeof(tx_buf)/sizeof(tx_buf[0]);//
	msg[0].buf   = tx_buf;

	msg[1].addr  = MPU6050_SLAVE_ADDR;
	msg[1].flags = 1;//读从机
	msg[1].len   = sizeof(rx_buf)/sizeof(rx_buf[0]);//
	msg[1].buf   = rx_buf;

	rdwr_arg.msgs = msg;
	rdwr_arg.nmsgs = sizeof(msg)/sizeof(msg[0]);

	if(ioctl(fd,I2C_RDWR,&rdwr_arg) < 0){
		perror("Fail to ioctl");
		return -1;
	}

	return rx_buf[0];
}

int mpu6050_init(int fd)
{
	//唤醒MPU6050
	mpu6050_write(fd,MPU6050_PWR_MGMT1_REG,0x00);
	usleep(500);

	return 0;
}

float mpu6050_read_temperature(int fd)
{
	short temp;
	unsigned char  buf[2];

	//读取mpu6050内部温度传感器器的值
	buf[0]=mpu6050_read(fd,MPU6050_TEMP_OUTL_REG);//低字节
	buf[1]=mpu6050_read(fd,MPU6050_TEMP_OUTH_REG);//高字节

	//Temperature in degrees C = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53
	temp = *((short *)buf);
	
	return (temp / 340.0 + 36.53);
}

int main(void)
{
	int fd;
	float temp;
	
	fd = open("/dev/i2c-5",O_RDWR);
	if(fd < 0){
		perror("Fail to open");
		exit(EXIT_FAILURE);
	}

	printf("fd = %d\n",fd);

	mpu6050_init(fd);
	
	while(1)
	{
		temp = mpu6050_read_temperature(fd);
		printf("temp : %.2f\n",temp);
		sleep(1);
	}
	
	return 0;
}
