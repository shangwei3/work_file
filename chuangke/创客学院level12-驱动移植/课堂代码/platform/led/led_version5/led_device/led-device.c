#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>

//像系统中注册LED设备的信息
//1.GPIO REG ADDR
//2.PIN

MODULE_LICENSE("GPL v2");

#define LED2_REG_BASE   0x11000C40 
#define LED3_REG_BASE   0x11000C20 
#define LED4_REG_BASE   0x114001e0
#define LED5_REG_BASE   0x114001e0
#define LED_REG_SIZE    8


struct led_des{
	int pin;
	int mode;
	int level;
};

struct led_des fs4412_led_platdatas[] = {
	[0] = {
		.pin = 7,
		.mode = 1,
		.level = 1,
	},

	[1] = {
		.pin = 0,
		.mode = 1,
		.level = 1,
	},

	[2] = {
		.pin = 4,
		.mode = 1,
		.level = 1,
	},

	[3] = {
		.pin = 5,
		.mode = 1,
		.level = 1,
	},
};

struct resource fs4412_led_resources[] = {
	[0] = {
		.start = LED2_REG_BASE,
		.end   = LED2_REG_BASE + LED_REG_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},

	[1] = {
		.start = LED3_REG_BASE,
		.end   = LED3_REG_BASE + LED_REG_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},

	[2] = {
		.start = LED4_REG_BASE,
		.end   = LED4_REG_BASE + LED_REG_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},


	[3] = {
		.start = LED5_REG_BASE,
		.end   = LED5_REG_BASE + LED_REG_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
};

void fs4412_led_release(struct device *dev)
{
	return;
}
	

struct platform_device fs4412_led_devices[] = {
	[0] = {
		.name =  "fs4412-led",
		.id   = 0,
		.num_resources = 1,
		.resource  = &fs4412_led_resources[0],
		.dev = {
			.release = fs4412_led_release,
			.platform_data = &fs4412_led_platdatas[0],
		},
	},

	[1] = {
		.name =  "fs4412-led",
		.id   = 1,
		.num_resources = 1,
		.resource  = &fs4412_led_resources[1],
		.dev = {
			.release = fs4412_led_release,
			.platform_data = &fs4412_led_platdatas[1],
		},
	},


	[2] = {
		.name =  "fs4412-led",
		.id   = 2,
		.num_resources = 1,
		.resource  = &fs4412_led_resources[2],
		.dev = {
			.release = fs4412_led_release,
			.platform_data = &fs4412_led_platdatas[2],
		},
	},


	[3] = {
		.name =  "fs4412-led",
		.id   = 3,
		.num_resources = 1,
		.resource  = &fs4412_led_resources[3],
		.dev = {
			.release = fs4412_led_release,
			.platform_data = &fs4412_led_platdatas[3],
		},
	},
};


int led_device_init(void)
{
	int i;

	for(i = 0;i < ARRAY_SIZE(fs4412_led_devices);i ++){
		platform_device_register(&fs4412_led_devices[i]);
	}

	return 0;
}

void led_device_exit(void)
{
	int i;
	for(i = 0;i < ARRAY_SIZE(fs4412_led_devices);i ++){
		platform_device_unregister(&fs4412_led_devices[i]);
	}
	
	return;
}

module_init(led_device_init);
module_exit(led_device_exit);