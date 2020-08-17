#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL v2");

struct led_plat_data{
	int pin;
	int mode;
};

static int led_probe(struct platform_device *pdev)
{
	struct resource *res;
	struct led_plat_data *pd;
	
	printk("led probe success!\n");

	//1.获取资源
	res = platform_get_resource(pdev,IORESOURCE_MEM, 0);
	if (!res){
		printk("Fail to platform get resouce");
		return -ENODEV;
	}

	printk("reg base : %#x\n",res->start);

	//pd = pdev->dev.platform_data;
	pd =  dev_get_platdata(&pdev->dev);
	printk("pin  : %d\n",pd->pin);
	printk("mode : %#x\n",pd->mode);

	//2.注册字符设备
	
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
