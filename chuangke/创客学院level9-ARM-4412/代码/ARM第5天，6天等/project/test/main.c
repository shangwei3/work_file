

int tmp=0;
extern int var;
extern int myadd(int x,int y);

int myadd(int x,int y)
{
	int sum=0;
	//sum=x+y;
	asm volatile(
		"add r4,%1,%2\n"
		"mov %0,r4\n"
		:"=r"(sum)
		:"r"(x),"r"(y)
		:"r4"
	);
	return sum;
}

/*CÓïÑÔ·ÃÎÊ»ã±à±äÁ¿	
int mymax(int x,int y)
{
	tmp=var;
	var=0x30;
	tmp=myadd(1,2);
	return x>y?x:y;
}	*/

/*»ã±à·ÃÎÊCÓïÑÔ´úÂë
int mymax(int x,int y)
{
	return x>y?x:y;
}*/