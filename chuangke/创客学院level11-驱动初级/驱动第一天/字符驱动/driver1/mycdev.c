#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define MAJOR_NUM  223

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CYG");

struct mycdev 
{
	int major;//主设备号
	int minor;//次设备号
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
	return 0;
}

ssize_t mycdev_write(struct file *file, const char __user *ubuf, size_t size, loff_t *offset)
{
	return 0;
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
	
	//cdev_init(struct cdev *, const struct file_operations *);
	cdev_init(&global_cdev.cdev,&mycdev_ops);
	
	//register_chrdev_region(dev_t, unsigned, const char *)
	ret = register_chrdev_region(MKDEV(MAJOR_NUM,0),1,"mycdev");
	if(ret != 0){
		printk("Fail to register_chrdev_region\n");
		goto err_register_chrdev_region;
	}

	//cdev_add(struct cdev * p, dev_t dev, unsigned count)
	ret = cdev_add(&global_cdev.cdev,MKDEV(MAJOR_NUM,0),1);
	if(ret != 0){
		printk("Fail to cdev_add\n");
		goto err_cdev_add;
	}
	
	
	return 0;

err_cdev_add:
	unregister_chrdev_region(MKDEV(MAJOR_NUM,0),1);
	
err_register_chrdev_region:
	return ret;
}

static void __exit mycdev_exit(void)
{
	cdev_del(&global_cdev.cdev);
	unregister_chrdev_region(MKDEV(MAJOR_NUM,0),1);
	return;
}

module_init(mycdev_init);
module_exit(mycdev_exit);