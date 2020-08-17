#include <linux/module.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CYG");

extern int  show_string(char *string);
extern int show_int(int data);

static int __init hello_init(void)
{
	printk("Hello module!\n");
	show_string("I love driver!\n");
	return 0;
}

static void __exit hello_exit(void)
{
	printk("ByeBye module!\n");
	show_int(888);
	return;
}

module_init(hello_init);
module_exit(hello_exit);