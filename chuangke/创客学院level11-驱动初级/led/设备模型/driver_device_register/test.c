#include <stdio.h>

struct test
{
	int a;
	int b;
	int c;
};

int main(int argc, const char *argv[])
{
	int b_offset;

	b_offset = (unsigned long )&(((struct test *)0)->b);

	printf("b_offset = %d\n",b_offset);
	
	return 0;
}
