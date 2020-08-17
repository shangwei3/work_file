#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

static int hello_init(void)
{
	printk("Hello init\n");
	return 0;
}

static void hello_exit(void);
{
	printk("Hello exit\n");
	return;
}

module_init(hello_init);
module_exit(hello_exit);
