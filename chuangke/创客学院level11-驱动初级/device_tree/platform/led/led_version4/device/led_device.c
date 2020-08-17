#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL v2");

#define LED0_REG_BASE     0x11000c40 
#define LED1_REG_BASE     0x11000c20 
#define LED2_REG_BASE     0x114001e0 
#define LED3_REG_BASE     0x114001e0

#define LED_REG_SIZE      8

struct led_plat_data{
	int pin;
	int mode;
};

struct led_plat_data fs4412_plat_datas[] = {
	[0] = {
		.pin = 7,
		.mode = 0x1,
	},

	[1] = {
		.pin = 0,
		.mode = 0x1,
	},

	[2] = {
		.pin = 4,
		.mode = 0x1,
	},

	[3] = {
		.pin = 5,
		.mode = 0x1,
	},
};


struct resource fs4412_led_resource[] = {
	[0] = {
		.start = LED0_REG_BASE,
		.end   = LED0_REG_BASE + LED_REG_SIZE  -1,
		.flags = IORESOURCE_MEM,
	},

	[1] = {
		.start = LED1_REG_BASE,
		.end   = LED1_REG_BASE + LED_REG_SIZE  -1,
		.flags = IORESOURCE_MEM,
	},

	[2] = {
		.start = LED2_REG_BASE,
		.end   = LED2_REG_BASE + LED_REG_SIZE  -1,
		.flags = IORESOURCE_MEM,
	},

	[3] = {
		.start = LED3_REG_BASE,
		.end   = LED3_REG_BASE + LED_REG_SIZE  -1,
		.flags = IORESOURCE_MEM,
	},

};

void fs4412_led_release(struct device *dev)
{

}

struct platform_device fs4412_led_devices[] = {

	[0]= {	
		.name = "fs4412-led",
		.id   = 0,
		.num_resources = 1,
		.resource = &fs4412_led_resource[0],
		.dev = {
			.release = fs4412_led_release,
			.platform_data = &fs4412_plat_datas[0],
		},
	},

	
	[1]= {	
		.name = "fs4412-led",
		.id   = 1,
		.num_resources = 1,
		.resource = &fs4412_led_resource[1],
		.dev = {
			.release = fs4412_led_release,
			.platform_data = &fs4412_plat_datas[1],
		},
	},


	
	[2]= {	
		.name = "fs4412-led",
		.id   = 2,
		.num_resources = 1,
		.resource = &fs4412_led_resource[2],
		.dev = {
			.release = fs4412_led_release,
			.platform_data = &fs4412_plat_datas[2],
		},
	},

			
	
	[3]= {	
		.name = "fs4412-led",
		.id   = 3,
		.num_resources = 1,
		.resource = &fs4412_led_resource[3],
		.dev = {
			.release = fs4412_led_release,
			.platform_data = &fs4412_plat_datas[3],
		},
	},
	
			
};

int led_device_init(void)
{
	int i = 0;
	
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
