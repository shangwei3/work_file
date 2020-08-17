
int data = 0x10;

//实现一个函数 ，实现加法操作，结果通过全局变量在汇编中获取，保存r5


//函数定义
int max(int x,int y)
{
	return x>y?x:y;
}

extern int src;

int myadd(int x,int y)
{
	 src = 1; 
	 int sum = 0;
	// myadd(10,20);
	
	asm volatile (
		   "add r4,%1,%2\n" // x,y -->input 
	     "mov %0,r4\n"    // sum -->output
	    :"=r"(sum)
	    :"r"(x),"r"(y)
	    :"r4"
	);
	return 0;
}




int main()
{
	
	 return 0;
}