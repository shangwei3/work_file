#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "led.h"

MODULE_LICENSE("GPL v2");

#define LED_MAJOR 250

#define LED_PHY_ADDR 0x11000C40
#define LED_PHY_SIZE 8
#define LED_CON  0
#define LED_DAT  4


struct led_dev
{
	void __iomem *reg;
	dev_t  devno;
	struct cdev cdev;
	struct class *cls;
	struct device *device;
};

struct led_dev *pled;

static int led_open(struct inode *inode, struct file *file)
{
	int regval;
	printk("%s call success!\n",__FUNCTION__);

	regval = readl(pled->reg + LED_CON);
	regval &= ~(0xf << 28);
	regval |=  (0x1 << 28);
	writel(regval, pled->reg + LED_CON);
	
	return 0;
}

static long led_ioctl(struct file *filp, u_int cmd, u_long arg)
{
	int err = 0;
	int regval = readl(pled->reg + LED_DAT);

	printk("%s call success!\n",__FUNCTION__);

	
	switch(cmd){

	case LED_IOC_ON:
		regval |= (1 << 7);
		break;

	case LED_IOC_OFF:
		regval &= ~(1 << 7);
		break;

	default:
		err = -EINVAL;
	}

	writel(regval,pled->reg + LED_DAT);

	return err;
}


static int led_close(struct inode *inode, struct file *file)
{
	int regval = readl(pled->reg + LED_DAT);
	
	printk("%s call success!\n",__FUNCTION__);
	regval &= ~(1 << 7);
	writel(regval,pled->reg);
	
	return 0;
}

static const struct file_operations led_fops = {
	.owner		= THIS_MODULE,
	.open		= led_open,
	.unlocked_ioctl	= led_ioctl,
	.release	= led_close,
};

int led_init(void)
{
	int ret;
	
	pled = kmalloc(sizeof(struct led_dev),GFP_KERNEL);
	if (!pled) {
		printk("Fail to allocate scan memory\n");
		return -ENOMEM;
	}

	cdev_init(&pled->cdev,&led_fops);

	ret = register_chrdev_region(MKDEV(LED_MAJOR,0),1,"led");
	if (ret) {
		ret = alloc_chrdev_region(&pled->devno,0,1,"led");
		if(ret){
			printk("Fail to alloc chrdev region");
			goto err_register_chrdev_region;
		}
		
	}else{
		pled->devno = MKDEV(LED_MAJOR,0);
	}
	
	ret = cdev_add(&pled->cdev,pled->devno,1);
	if(ret){
		printk("unable register character device\n");
		goto err_cdev_add;
	}

	//创建一个类
	pled->cls = class_create(THIS_MODULE, "led");
	if (IS_ERR(pled->cls)) {
		printk("failed to allocate class\n");
		ret = PTR_ERR(pled->cls);
		goto err_class_create;
	}

	//创建设备
	pled->device = device_create(pled->cls,NULL, pled->devno,NULL,"led");
	if (IS_ERR(pled->device)) {
		printk("device_create failed\n");
		ret = PTR_ERR(pled->device);
		goto err_device_create;
	}

	//映射寄存器地址
	pled->reg = ioremap(LED_PHY_ADDR,LED_PHY_SIZE);
	if (!pled->reg) {
		printk("Unable to map I/O register\n");
		goto err_ioremap;
	}

	
	return 0;

err_ioremap:
	device_destroy(pled->cls,pled->devno);

err_device_create:
	class_destroy(pled->cls);

err_class_create:
	cdev_del(&pled->cdev);

err_cdev_add:
	unregister_chrdev_region(pled->devno,1);
	
err_register_chrdev_region:
	kfree(pled);
	return ret;
	
}

void led_exit(void)
{
	iounmap(pled->reg);
	device_destroy(pled->cls,pled->devno);
	class_destroy(pled->cls);
	cdev_del(&pled->cdev);
	unregister_chrdev_region(pled->devno,1);
	kfree(pled);
	return;
}


module_init(led_init);
module_exit(led_exit);
