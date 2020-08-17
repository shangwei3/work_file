#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <linux/device.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");

enum soc_type {
	TYPE_S5PC100,
	TYPE_S5Pv210,
};

int s5pc100led_probe(struct platform_device *pdev)
{
	printk("driver and device match\n");

	switch(platform_get_device_id(pdev)->driver_data)
	{
		case TYPE_S5PC100:
			printk("The device name : %s\n",platform_get_device_id(pdev)->name);
			break;

		case TYPE_S5Pv210:
			printk("The device name : %s\n",platform_get_device_id(pdev)->name);
			break;
	}
	
	return 0;
}

int s5pc100led_remove(struct platform_device *pdev)
{
	printk("driver and device separation\n");
	return 0;
}

static struct platform_device_id s5p_led_driver_ids[] = {
	[0] = {
		.name = "s5pc100-led",
		.driver_data =TYPE_S5PC100,
	},

	[1] = {
		.name = "s5pv210-led",
		.driver_data =TYPE_S5Pv210,
	},
};

static struct platform_driver s5pc100led_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name  = "s5pcxxx-led",
	},
	.probe    = s5pc100led_probe,
	.remove   = s5pc100led_remove,
	.id_table = s5p_led_driver_ids, 
};

int __init led_module_init(void)
{
	int ret;
	
	ret = platform_driver_register(&s5pc100led_driver);

	return ret;
}

void __exit led_module_exit(void)
{
	platform_driver_unregister(&s5pc100led_driver);
	return ;
}

module_init(led_module_init);
module_exit(led_module_exit);
