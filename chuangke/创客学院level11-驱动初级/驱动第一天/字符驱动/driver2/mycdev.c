#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#define MAJOR_NUM  223

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CYG");

struct mycdev 
{
	int major;//主设备号
	int minor;//次设备号
	struct class  *cls;
	struct device *device;
	struct cdev cdev;
};

struct mycdev global_cdev;

static int mcdev_open(struct inode *inode, struct file *file)
{
	printk("Call mycdev_open() ok!\n");
	return 0;
}

ssize_t mycdev_read(struct file *file, char __user *ubuf, size_t size, loff_t *offset)
{
	return size;
}

ssize_t mycdev_write(struct file *file, const char __user *ubuf, size_t size, loff_t *offset)
{
	return size;
}

struct file_operations mycdev_ops = {
	.owner = THIS_MODULE,
	.open  = mcdev_open,
	.write = mycdev_write,
	.read  = mycdev_read,
};


static int __init mycdev_init(void)
{
	int ret;
	dev_t dev_num;
	
	//cdev_init(struct cdev *, const struct file_operations *);
	cdev_init(&global_cdev.cdev,&mycdev_ops);

	//动态分配设备号
	ret = alloc_chrdev_region(&dev_num,0,1,"mycdev");
	if(ret != 0){
		printk("Fail to alloc_register_chrdev_region\n");
		goto err_alloc_register_chrdev_region;
	}

	global_cdev.major =  MAJOR(dev_num);
	global_cdev.minor =  MINOR(dev_num);

	//cdev_add(struct cdev * p, dev_t dev, unsigned count)
	ret = cdev_add(&global_cdev.cdev,dev_num,1);
	if(ret != 0){
		printk("Fail to cdev_add\n");
		goto err_cdev_add;
	}

	//自动创建设备节点
	global_cdev.cls = class_create(THIS_MODULE,"mycdev");
	if(IS_ERR(global_cdev.cls)){
		printk("Fail to class_create");
		ret = PTR_ERR(global_cdev.cls);
		goto err_class_create;
	}

	//device_create(struct class * cls, struct device * parent, 
	//dev_t devt, void * drvdata, const char * fmt,...)
	global_cdev.device = device_create(global_cdev.cls,NULL,dev_num,NULL,"mycdev");
	if(IS_ERR(global_cdev.device)){
		printk("Fail to device create");
		ret = PTR_ERR(global_cdev.device);
		goto err_device_create;
	}
	
	return 0;

err_device_create:
	class_destroy(global_cdev.cls);

err_class_create:
	cdev_del(&global_cdev.cdev);

err_cdev_add:
	unregister_chrdev_region(dev_num,1);
	
err_alloc_register_chrdev_region:
	return ret;
}

static void __exit mycdev_exit(void)
{
	dev_t dev_num = MKDEV(global_cdev.major,global_cdev.minor);
	device_destroy(global_cdev.cls, dev_num);
	class_destroy(global_cdev.cls);
	cdev_del(&global_cdev.cdev);
	unregister_chrdev_region(dev_num,1);
	return;
}

module_init(mycdev_init);
module_exit(mycdev_exit);
