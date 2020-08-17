void mpu6050_write_reg(int fd,unsigned char buf,int len)
{	
	struct i2c_msg  msg[1];
	
	msg[0].addr = 0x68;
	msg[0].flags = 0;
	msg[0].len  = len;
	msg[0].buf =  &buf;
}

unsigned char  mpu6050_read_reg(int fd,unsigned char regaddr )
{
	unsigned char regvalue;
	struct i2c_msg  msg[2];
	
	msg[0].addr = 0x68;
	msg[0].flags = 0;
	msg[0].len  = 1;
	msg[0].buf =  &regaddr;
	
	msg[1].addr = 0x68;
	msg[1].flags = I2C_M_RD;
	msg[1].len  = 1;
	msg[1].buf =  &regvalue;
	
	return regvalue;
}