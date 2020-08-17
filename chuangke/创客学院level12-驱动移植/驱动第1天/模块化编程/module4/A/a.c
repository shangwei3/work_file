#include <linux/module.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CYG");

extern int add(int a,int b);
extern int sub(int a,int b);

int module_a_init(void)
{
	int a = 100,b = 200;

	printk("a + b = %d\n",add(a,b));
	return 0;
}

void module_a_exit(void)
{
	int a = 100,b = 200;
	printk("a - b = %d\n",sub(a,b));
}

module_init(module_a_init);
module_exit(module_a_exit);