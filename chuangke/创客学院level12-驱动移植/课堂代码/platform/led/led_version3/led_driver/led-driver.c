#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include "led.h"

MODULE_LICENSE("GPL v2");

enum{
	CON = 0,
	DAT = 4,
};


struct led_des{
	int pin;
	int mode;
	int level;
};

struct led_device{
	dev_t  devno;
	char   state[15];
	struct cdev cdev;
	struct led_des *des;
	struct class *led_class;
	struct device *dev;
	void		__iomem *regs;
};

struct led_device *pled;


void led_on(void)
{
	int reg_value;
	int pin = pled->des->pin;
	int level = pled->des->level;

	reg_value = readl(pled->regs + DAT);
	reg_value &= ~(1 << pin);
	reg_value |= (level << pin);
	writel(reg_value, pled->regs + DAT);

	return;
}

void led_off(void)
{
	int reg_value;
	int pin = pled->des->pin;
	int level = pled->des->level;

	reg_value = readl(pled->regs + DAT);
	reg_value &= ~(1 << pin);
	//~0 -> 1111111111111111111111111111111111
	//!0 -> 1 
	reg_value |= (!level << pin);
	writel(reg_value, pled->regs + DAT);

	return;
}


static int led_open(struct inode *inode, struct file *file)
{
	int reg_value;
	int pin = pled->des->pin;
	int mode = pled->des->mode;
	
	printk("led open success!\n");
	
	reg_value = readl(pled->regs + CON);
	reg_value &= ~(0xf << (4 * pin));
	reg_value |=  (mode << (4 * pin));
	writel(reg_value, pled->regs + CON);
	
	return 0;
}

static int led_release(struct inode *inode, struct file *file)
{
	printk("led release!\n");
	led_off();
	return 0;
}


static long led_ioctl(struct file *file,
					unsigned int cmd, unsigned long arg)
{
	switch(cmd){
		case LEDIOC_ON:
			led_on();
			break;

		case LEDIOC_OFF:
			led_off();
			break;

		default:
			printk("unknown cmd\n");
			return -EINVAL;
	}

	return 0;
}


static const struct file_operations led_fops = {
	.owner		= THIS_MODULE,
	.open		= led_open,
	.unlocked_ioctl	= led_ioctl,
	.release	= led_release,
};


int register_led_chrdev(struct led_device *pled)
{
	int ret;

	//pled->cdev.ops = &led_fops;
	cdev_init(&pled->cdev,&led_fops);

	
	//如果失败，则动态获取
	//cat /proc/devices
	ret = alloc_chrdev_region(&pled->devno,0,1,"led2");
	if(ret){
		printk("led:couldn't alloc device number\n");
		goto err_register_chrdev_region;
	}
	

	//添加cdev到系统
	ret = cdev_add(&pled->cdev,pled->devno, 1);
	if (ret){
		printk("led:cdev add failure\n");
		goto err_cdev_add;
	}

	//导出设备号信息
	//1.创建一个类 /sys/class/fs4412-led
	pled->led_class = class_create(THIS_MODULE, "fs4412-led");
	if (IS_ERR(pled->led_class)){//判断是是否出错
		printk("Fail to class create\n");
		ret = PTR_ERR(pled->led_class);//获取错误码
		goto err_class_create;	
	}
	
	//2.创建一个设备 device name, /dev/led2
	pled->dev = device_create(pled->led_class,NULL,pled->devno, NULL,"led2");
	if (IS_ERR(pled->dev)) {
		printk("Fail to device create\n");
		ret = PTR_ERR(pled->dev);
		goto err_device_create;
	}


	return 0;
	
err_device_create:
	class_destroy(pled->led_class);
	
err_class_create:
	cdev_del(&pled->cdev);
	
err_cdev_add:
	unregister_chrdev_region(pled->devno,1);
	
err_register_chrdev_region:
	return ret;	
	
}

//当设备和驱动匹配成功之后，操作系统会自动调用probe函数
//并且把描述设备信息的结构体struct platform_device传递过来
//，这样我们就可以通过platform_device结构体拿到设备的所有信息
static int led_probe(struct platform_device *pdev)
{
	int ret;
	struct resource *res; 
	struct led_des *platdata;
	
		
	printk("match: led probe!\n");
	printk("match: driver data : %ld\n",pdev->id_entry->driver_data);

	//1.获取资源:寄存器资源,平台私有的资源
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res){
		printk("Fail to platform get resource\n");
		return -ENODEV;
	}
	printk("reg phy addr : %#x\n",res->start);

	//platdata = pdev->dev.platform_data;
	platdata = dev_get_platdata(&pdev->dev);
	if(!platdata){
		printk("No device private info!\n");
		return -ENODEV;
	}

	printk("pin:%d , mode:%d , level : %d\n",platdata->pin,platdata->mode,platdata->level);

	
	pled = kmalloc(sizeof(*pled),GFP_KERNEL);
	if (!pled) {
		printk("Fail to kmalloc\n");
		return -ENOMEM;
	}

	pled->des = platdata;

	//映射物理地址
	pled->regs = ioremap(res->start,resource_size(res));
	if (!pled->regs) {
		printk("Unable to map I/O register");
		ret = -ENOMEM;
		goto err_ioremap;
	}
	
	
	//2.注册字符设备
	ret = register_led_chrdev(pled);
	if(ret < 0){
		printk("Fail to register led chrdev\n");
		goto err_register_led_chrdev;
	}
	
	return 0;

err_register_led_chrdev:
	iounmap(pled->regs);

err_ioremap:
	kfree(pled);
	return ret;
}

static int led_remove(struct platform_device *pdev)
{
	printk("detach:led remove!\n");
	
	iounmap(pled->regs);
	device_destroy(pled->led_class,pled->devno);
	class_destroy(pled->led_class);
	cdev_del(&pled->cdev);
	unregister_chrdev_region(pled->devno,1);//释放设备号
	kfree(pled);
	
	return 0;
}

static struct platform_device_id led_driver_ids[] = {
	{
		.name	= "fs4412-led",
		.driver_data	= 1,
	}, {
		.name	= "samsung-led",
		.driver_data	= 0,
	},
	{ },
};

static struct platform_driver led_driver = {
	.probe          = led_probe,
	.remove         = led_remove,
	.id_table       = led_driver_ids,
	.driver         = {
		.name   = "led",
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
