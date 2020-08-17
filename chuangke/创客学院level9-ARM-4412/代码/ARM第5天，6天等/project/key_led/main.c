/*.equ GPX2CON,0x11000c40	   	//第七个引脚
.equ GPX2DAT,0x11000c44
.equ GPX1CON,0x11000c20		    //第0个引脚
.equ GPX1DAT,0x11000c24
.equ GPF3CON,0x114001e0			//第4、 5个引脚
.equ CON3DAT,0x114001e4		*/
#define GPX2CON 0x11000c40
#define GPX2DAT 0x11000c44
#define GPX1CON 0x11000c20
#define GPX1DAT 0x11000c24
#define GPX1PUD 0x11000c28
#define GPF3CON 0x114001E0
#define GPF3DAT 0x114001E4

#define REG_READ(addr) (*(volatile unsigned int *)(addr))
#define REG_WRITE(addr,val) (*(volatile unsigned int *)(addr)=(val))

				

				

/*个LED全亮的代码
REG_WRITE(GPX2CON,REG_READ(GPX2CON)&(~(0XF<<28))|(0X1<<28));
REG_WRITE(GPX2DAT,REG_READ(GPX2DAT)|(0X1<<7));
REG_WRITE(GPX1CON,REG_READ(GPX1CON)&(~(0XF))|(0X1));
REG_WRITE(GPX1DAT,REG_READ(GPX1DAT)|(0X1));
REG_WRITE(GPX3CON,REG_READ(GPX3CON)&(~(0XF<<16))|(0X1<<16));
REG_WRITE(GPX3DAT,REG_READ(GPX3DAT)|(0X1<<4));
REG_WRITE(GPX3CON,REG_READ(GPX3CON)&(~(0XF<<20))|(0X1<<20));
REG_WRITE(GPX3DAT,REG_READ(GPX3DAT)|(0X1<<5));
*/
void delay(volatile unsigned long dly)
{
	for(;dly>0;dly--);
}

int main(void)
{
	REG_WRITE(GPX1CON,REG_READ(GPX1CON)&(~(0XF)<<4));//设置为输入模式
	REG_WRITE(GPX1PUD,REG_READ(GPX1PUD)&(~(0X3)<<2));//禁止上拉下拉
	REG_WRITE(GPX2CON,REG_READ(GPX2CON)&(~(0XF<<28))|(0X1<<28));//设置为输出模式
	REG_WRITE(GPX2DAT,REG_READ(GPX2DAT)|(~(0X1<<7))); // LED2灭
	
	 while(1)
	 {
	 	if(!(REG_READ(GPX1DAT)&(0X1<<1)))
		{
			delay(10);
			if(!(REG_READ(GPX1PUD)&(0X1<<1)))
			{
				REG_WRITE(GPX2DAT,REG_READ(GPX2DAT)|(0X1<<7));
				while(!(REG_READ(GPX1DAT)&(0X1<<1)));
				REG_WRITE(GPX2DAT,REG_READ(GPX2DAT)&(~(0X1<<7)));
			}
		}	
	 }
	 return 0;
}


