#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL v2");

#define LED_DEVICE_MAJOR 251

struct led_device{
	struct cdev cdev;//通用的字符设备描述
};

struct led_device *pled;

static int led_open(struct inode *inode, struct file *file)
{
	printk("driver led open success!\n");
	return 0;
}

int led_release(struct inode *inode, struct file *file)
{
	printk("driver led release success!\n");
	return 0;
}

static const struct file_operations led_registers_fops = {
	.open = led_open,
	.release = led_release,
	.owner = THIS_MODULE,
};

int led_init(void)
{
	int retval;
	
	
	printk("led init\n");

  	//分配空间
 	pled = kmalloc (sizeof(*pled), GFP_KERNEL);
 	if (!pled){
   		printk("out of memory!");
    	return -ENOMEM;
 	}

	//pled->cdev.ops = &led_registers_fops;
	//让cdev结构体记录操作设备的函数接口
	cdev_init(&pled->cdev,&led_registers_fops);

	//申请设备号
	retval = register_chrdev_region(MKDEV(LED_DEVICE_MAJOR,0),1,"led_device");
	if (retval) {
		printk("Unable to register minors for led_device\n");
		goto err_register_chrdev_region;
	}		

	//添加字符设备
	retval = cdev_add(&pled->cdev,MKDEV(LED_DEVICE_MAJOR,0), 1);
	if (retval){
		printk("Fail to cdev add\n");
		goto err_cdev_add;
	}

	return 0;

err_cdev_add:
	unregister_chrdev_region(MKDEV(LED_DEVICE_MAJOR,0),1);
	
err_register_chrdev_region:
	kfree(pled);
	return retval;
	
}

void led_exit(void)
{
	printk("led exit\n");

	cdev_del(&pled->cdev);
	unregister_chrdev_region(MKDEV(LED_DEVICE_MAJOR,0),1);
	kfree(pled);
	
	return;
}

module_init(led_init);
module_exit(led_exit);
