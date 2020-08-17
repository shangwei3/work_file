#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL v2");

int led_init(void)
{
	printk("led init\n");
	return 0;
}

void led_exit(void)
{
	printk("led exit\n");
	return;
}

module_init(led_init);
module_exit(led_exit);