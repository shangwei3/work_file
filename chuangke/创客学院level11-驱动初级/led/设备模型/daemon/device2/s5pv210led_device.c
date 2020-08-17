#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <linux/device.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");

static struct platform_device s5pv210led_device = {	
	.name = "s5pv210-led",
};


int __init led_device_init(void)
{
	int ret;
	ret = platform_device_register(&s5pv210led_device);
	return 0;
}

void __exit led_device_exit(void)
{
	platform_device_unregister(&s5pv210led_device);
	return ;
}

module_init(led_device_init);
module_exit(led_device_exit);
