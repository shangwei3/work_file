#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include "led.h"

MODULE_LICENSE("GPL v2");

struct led_plat_data{
	int pin;
	int mode;
};

enum{
	CON = 0,
	DAT = 4,
};

struct led_device{
	dev_t devno;
	char led_state[20];//led on or led off 
	struct cdev cdev;//通用的字符设备描述
	struct device *dev;
	void __iomem *mbase;
	int pin;
	int mode;
	int level;
};

/*
 *通过测试，发现问题:
 *不论操作的是哪一个设备文件，最终都是在操作最后一个LED
 *
 *原因:
 *这个全局变量pled的值被覆盖了
 *
 *
 *解决:
 *必须找到每个设备文件对应的struct led_device结构体
 */
struct led_device *pled;

struct class *led_class;

void led_on(void)
{
	int regval;

	regval = readl(pled->mbase + DAT);
	regval &= ~(1 << (pled->pin));
	regval |=  (pled->level << pled->pin);
	writel(regval,pled->mbase + DAT);

	return;
}

void led_off(void)
{
	int regval;

	regval = readl(pled->mbase + DAT);
	regval &= ~(1 << (pled->pin));
	//1
	//~1 --- 111111111111111111111111111111111111110
	//!1 --- 0
	//!0 --- 1
	regval |=  (!pled->level) << (pled->pin);
	writel(regval,pled->mbase + DAT);

	return;
}

static int led_open(struct inode *inode, struct file *file)
{
	int regval;
	printk("driver led open success!\n");

	regval  = readl(pled->mbase + CON);
	regval &= ~(0xf << (4 * pled->pin));
	regval |=  ((pled->mode) << (4 * pled->pin));
	writel(regval,pled->mbase + CON);
	
	return 0;
}

int led_release(struct inode *inode, struct file *file)
{
	printk("driver led release success!\n");
	return 0;
}

static long led_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd){
		case LEDIOC_ON:
			led_on();
			printk("led on cmd sucess!\n");
			break;

		case LEDIOC_OFF:
			led_off();
			printk("led off cmd sucess!\n");
			break;

		default:
			printk("Unknown cmd!\n");
			return -EINVAL;
	}

	return 0;
}

static const struct file_operations led_registers_fops = {
	.open = led_open,
	.release = led_release,
	.unlocked_ioctl =	led_ioctl,
	.owner = THIS_MODULE,
};


int register_led_chrdev(void)
{	
	static int i;
	int retval;


	//pled->cdev.ops = &led_registers_fops;
	//让cdev结构体记录操作设备的函数接口
	cdev_init(&pled->cdev,&led_registers_fops);

	retval = alloc_chrdev_region(&pled->devno,0,1,"led_device");
	if(retval){
		printk("Fail to alloc chrdev region\n");
		goto err_register_chrdev_region;
	}

	//添加字符设备
	retval = cdev_add(&pled->cdev,pled->devno, 1);
	if (retval){
		printk("Fail to cdev add\n");
		goto err_cdev_add;
	}

	//自动创建设备文件
	//导出设备号信息
	pled->dev = device_create(led_class,NULL,pled->devno, NULL,"led%d",i++);
	if (IS_ERR(pled->dev)) {
		retval = PTR_ERR(pled->dev);
		goto err_device_create;
	}

	return 0;

err_device_create:
	cdev_del(&pled->cdev);
	
err_cdev_add:
	unregister_chrdev_region(pled->devno,1);
	
err_register_chrdev_region:
	return retval;
}


static int led_probe(struct platform_device *pdev)
{
	int retval;
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

	//分配空间
 	pled = kmalloc (sizeof(*pled), GFP_KERNEL);
 	if (!pled){
   		printk("out of memory!");
    	return -ENOMEM;
 	}

	pled->pin  = pd->pin;
	pled->mode = pd->mode;
	pled->level = pdev->id_entry->driver_data;

	//映射寄存器地址
	pled->mbase = ioremap(res->start,resource_size(res));
	if (pled->mbase == NULL) {
		printk("c4: can't remap memory at 0x%lx\n",res->start);
		retval = -EIO;
		goto err_ioremap;
	}

	//2.注册字符设备
	register_led_chrdev();
	
	return 0;

err_ioremap:
	kfree(pled);
	return retval;
}

static int led_remove(struct platform_device *pdev)
{
	printk("led remove success!\n");
	
	iounmap(pled->mbase);
	device_destroy(led_class,pled->devno);
	
	cdev_del(&pled->cdev);
	unregister_chrdev_region(pled->devno,1);
	kfree(pled);

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
	//1.创建一个类 /sys/class/类名的目录
	led_class = class_create(THIS_MODULE, "led");
	if (IS_ERR(led_class)) {
		printk("couldn't create class\n");
		return  PTR_ERR(led_class);
	}

	return platform_driver_register(&led_driver);
}

void led_driver_exit(void)
{
	platform_driver_unregister(&led_driver);
	class_destroy(led_class);
}

module_init(led_driver_init);
module_exit(led_driver_exit);
