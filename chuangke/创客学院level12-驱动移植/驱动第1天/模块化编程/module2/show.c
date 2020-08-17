#include <linux/module.h>
#include <linux/fs.h>

int  show_string(char *string)
{
	printk("string : %s\n",string);
	return 0;
}

int show_int(int data)
{
	printk("data : %d\n",data);
	return 0;
}