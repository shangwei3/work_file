#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>

MODULE_LICENSE("GPL v2");

#define LED_DEVICE_MAJOR 251

struct led_device{
	dev_t devno;
	struct class *led_class;
	struct cdev cdev;//通用的字符设备描述
	struct device *dev;
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
	dev_t devno;
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
		//申请失败，则动态注册
		retval = alloc_chrdev_region(&devno,0,1,"led_device");
		if(retval){
			printk("Fail to alloc chrdev region\n");
			goto err_register_chrdev_region;
		}
		
	}else{
		devno = MKDEV(LED_DEVICE_MAJOR,0);
	}	

	pled->devno = devno;

	//添加字符设备
	retval = cdev_add(&pled->cdev,devno, 1);
	if (retval){
		printk("Fail to cdev add\n");
		goto err_cdev_add;
	}

	//自动创建设备文件

	//1.创建一个类 /sys/class/类名的目录
	pled->led_class = class_create(THIS_MODULE, "led");
	if (IS_ERR(pled->led_class)) {
		printk("couldn't create class\n");
		retval =  PTR_ERR(pled->led_class);
		goto err_class_create;
	}

	//导出设备号信息
	pled->dev = device_create(pled->led_class,NULL,pled->devno, NULL,"led");
	if (IS_ERR(pled->dev)) {
		retval = PTR_ERR(pled->dev);
		goto err_device_create;
	}


	return 0;

err_device_create:
	class_destroy(pled->led_class);
	
err_class_create:
	cdev_del(&pled->cdev);
	
err_cdev_add:
	unregister_chrdev_region(devno,1);
	
err_register_chrdev_region:
	kfree(pled);
	return retval;
}

void led_exit(void)
{
	printk("led exit\n");

	device_destroy(pled->led_class,pled->devno);
	class_destroy(pled->led_class);
	
	cdev_del(&pled->cdev);
	unregister_chrdev_region(pled->devno,1);
	kfree(pled);
	
	return;
}

module_init(led_init);
module_exit(led_exit);
