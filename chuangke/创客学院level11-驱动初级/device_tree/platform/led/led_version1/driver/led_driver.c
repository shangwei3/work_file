#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL v2");

static int led_probe(struct platform_device *pdev)
{
	printk("led probe success!\n");
	printk("match device name : %s\n",pdev->id_entry->name);
	printk("match device data : %d\n",pdev->id_entry->driver_data);
	printk("match device data : %d\n",platform_get_device_id(pdev)->driver_data);
	printk("match device name : %s\n",platform_get_device_id(pdev)->name);

	

	return 0;
}

static int led_remove(struct platform_device *pdev)
{
	printk("led remove success!\n");
	return 0;
}


static struct platform_device_id led_platform_ids[] = {
	{ .name = "fs4412-led", .driver_data = 1 },
	{ .name = "origen-led", .driver_data = 0 },
	{ /* sentinel */ }
};

static struct platform_driver led_driver = {
	.probe          = led_probe,
	.remove         = led_remove,
	.driver         = {
		.name   = "led",
		.owner  = THIS_MODULE,
	},
	.id_table = led_platform_ids,
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
