#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL v2");

static int led_probe(struct platform_device *pdev)
{
	printk("match: led probe!\n");
	return 0;
}

static int led_remove(struct platform_device *pdev)
{
	printk("detach:led remove!\n");
	return 0;
}

static struct platform_driver led_driver = {
	.probe          = led_probe,
	.remove         = led_remove,
	.driver         = {
		.name   = "fs4412-led",
		.owner  = THIS_MODULE,
	},
};


int led_driver_init(void)
{
	return platform_driver_register(&led_driver);
}

void led_driver_exit(void)
{
	platform_driver_unregister(&led_driver);
}

module_init(led_driver_init);
module_exit(led_driver_exit);
