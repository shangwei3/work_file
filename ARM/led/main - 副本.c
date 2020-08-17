#define REG(x)	( *( (volatile unsigned int *) (x) ) )
#define GPX2CON REG(0x11000c40)
#define GPX2DAT REG(0x11000c44)
#define GPX2PUD REG(0x11000c48)

#define GPX1CON REG(0x11000c20)
#define GPX1DAT REG(0x11000c24)
#define GPX1PUD REG(0x11000c28)
int main()
{

	GPX2CON = (GPX2CON & ~(0xf<<28)) | (1<<28);
	GPX2DAT |= 1<<7;
	GPX2PUD &= ~(3<<14);
	GPX1CON |=  1;
	GPX1DAT |= 1<<7;
	GPX1PUD &= ~(3<<14);
	return 0;
}
