#include <stdio.h>
#include <head.h>

#define LOG_TAG "TEST"

#include <log/log.h>

int main()
{
	int a = 10;
	int b = 20;
	int c;

	c = add(a,b);
	ALOGE("c = %d\n",c);

	c = sub(a,b);
	ALOGE("c = %d\n",c);

	return 0;
}
