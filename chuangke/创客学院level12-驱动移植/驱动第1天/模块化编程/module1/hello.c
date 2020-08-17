#include <linux/module.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CYG");

static int __init hello_init(void)
{
	printk("Hello module!\n");
	return 0;
}

static void __exit hello_exit(void)
{
	printk("ByeBye module!\n");
	return;
}

module_init(hello_init);
module_exit(hello_exit);