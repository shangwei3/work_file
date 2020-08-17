#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>

//像系统中注册LED设备的信息
//1.GPIO REG ADDR
//2.PIN

MODULE_LICENSE("GPL v2");

#define LED_REG_BASE   0x11000C40 
#define LED_REG_SIZE   8

struct led_des{
	int pin;
	int mode;
	int level;
};

struct led_des fs4412_led_platdata = {
	.pin = 7,
	.mode = 1,
	.level = 1,
};

struct resource fs4412_led_resource[] = {
	[0] = {
		.start = LED_REG_BASE,
		.end   = LED_REG_BASE + LED_REG_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
};

void fs4412_led_release(struct device *dev)
{
	return;
}
	

struct platform_device fs4412_led_device = {
	.name =  "samsung-led",
	.id   = -1,
	.num_resources = ARRAY_SIZE(fs4412_led_resource),
	.resource  = fs4412_led_resource,
	.dev = {
		.release = fs4412_led_release,
		.platform_data = &fs4412_led_platdata,
	},
};


int led_device_init(void)
{
	return platform_device_register(&fs4412_led_device);
}

void led_device_exit(void)
{
	platform_device_unregister(&fs4412_led_device);
	return;
}

module_init(led_device_init);
module_exit(led_device_exit);
