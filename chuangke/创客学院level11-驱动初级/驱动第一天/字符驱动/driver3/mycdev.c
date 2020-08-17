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

struct cdev_device 
{
	int len;
	char kbuf[1024];
	int major;//主设备号
	int minor;//次设备号
	struct class  *cls;
	struct device *device;
	struct cdev cdev;
};

struct cdev_device *pcd;

static int mcdev_open(struct inode *inode, struct file *file)
{
	printk("Call mycdev_open() ok!\n");
	return 0;
}

ssize_t mycdev_read(struct file *file, char __user *ubuf, size_t size, loff_t *offset)
{
	int ret = 0;

	printk("call mycdev_read()\n");
	
	if(strlen(pcd->kbuf) == 0)
		return 0;
			
	printk("size = %ld\n",size);
	//copy_to_user(void __user * to, const void * from, unsigned long n)
	ret = copy_to_user(ubuf,pcd->kbuf,strlen(pcd->kbuf));
	if(ret != 0){
		printk("Fail to copy_to_user\n");
		return -1;
	}
	
	memset(pcd->kbuf,0,sizeof(pcd->kbuf));
	
	return  pcd->len;
}

ssize_t mycdev_write(struct file *file, const char __user *ubuf, size_t size, loff_t *offset)
{
	int ret;

	printk("call mycdev_write()\n");
	printk("size : %ld\n",size);

	if(size >= sizeof(pcd->kbuf))
		return -1;

	//static inline long copy_from_user(void *to,
	//	const void __user * from, unsigned long n)
	ret = copy_from_user(pcd->kbuf,ubuf,size);
	if(ret != 0){
		printk("Fail to copy_from_user\n");
		return -1;
	}	
	
	pcd->kbuf[size]  = '\0';

	pcd->len = size;

	printk("kbuf : %s\n",pcd->kbuf);

	return pcd->len;
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

	pcd = kzalloc(sizeof(struct cdev_device),GFP_KERNEL);
	if(!pcd){
		printk("Fail to kmalloc()\n");
		return -ENOMEM;
	}
	
	
	//cdev_init(struct cdev *, const struct file_operations *);
	cdev_init(&pcd->cdev,&mycdev_ops);

	//动态分配设备号
	ret = alloc_chrdev_region(&dev_num,0,1,"mycdev");
	if(ret != 0){
		printk("Fail to alloc_register_chrdev_region\n");
		goto err_alloc_register_chrdev_region;
	}
	
	pcd->len   = 0;
	pcd->major =  MAJOR(dev_num);
	pcd->minor =  MINOR(dev_num);

	//cdev_add(struct cdev * p, dev_t dev, unsigned count)
	ret = cdev_add(&pcd->cdev,dev_num,1);
	if(ret != 0){
		printk("Fail to cdev_add\n");
		goto err_cdev_add;
	}

	//自动创建设备节点
	pcd->cls = class_create(THIS_MODULE,"mycdev");
	if(IS_ERR(pcd->cls)){
		printk("Fail to class_create");
		ret = PTR_ERR(pcd->cls);
		goto err_class_create;
	}

	//device_create(struct class * cls, struct device * parent, 
	//dev_t devt, void * drvdata, const char * fmt,...)
	pcd->device = device_create(pcd->cls,NULL,dev_num,NULL,"mycdev");
	if(IS_ERR(pcd->device)){
		printk("Fail to device create");
		ret = PTR_ERR(pcd->device);
		goto err_device_create;
	}
	
	return 0;

err_device_create:
	class_destroy(pcd->cls);

err_class_create:
	cdev_del(&pcd->cdev);

err_cdev_add:
	unregister_chrdev_region(dev_num,1);
	
err_alloc_register_chrdev_region:
	kfree(pcd);
	
	return ret;
}

static void __exit mycdev_exit(void)
{
	dev_t dev_num = MKDEV(pcd->major,pcd->minor);
	device_destroy(pcd->cls, dev_num);
	class_destroy(pcd->cls);
	cdev_del(&pcd->cdev);
	unregister_chrdev_region(dev_num,1);
	return;
}

module_init(mycdev_init);
module_exit(mycdev_exit);
