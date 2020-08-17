#include <linux/module.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CYG");

static int add(int a,int b)
{
	return (a + b);
}
EXPORT_SYMBOL_GPL(add);

static int sub(int a,int b)
{
	return (a - b);
}
EXPORT_SYMBOL_GPL(sub);

int module_b_init(void)
{
	printk("export add(),sub()\n");
	return 0;
}

void module_b_exit(void)
{
	printk("Over!\n");
	return;
}

module_init(module_b_init);
module_exit(module_b_exit);
