#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL v2");

#define LED_DEVICE_MAJOR 251

struct led_device{
	dev_t devno;
	char led_state[20];//led on or led off 
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

/** read(int fd,void *buf,int size)
 *
 *	seq_read -	->read() method for sequential files.
 *	@file: the file to read from
 *	@buf: the buffer to read to
 *	@size: the maximum number of bytes to read
 *	@ppos: the current position in the file
 *
 *	Ready-made ->f_op->read()
 */
ssize_t led_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	printk("led read success!\n");

	if(size > sizeof(pled->led_state)){
		printk("Fail to led write!\n");
		return -EINVAL;
	}

	if (copy_to_user(buf,pled->led_state,sizeof(pled->led_state))) {
		printk("Failed to copy to user\n");
		return -EFAULT;
	}
	
	
	return sizeof(pled->led_state);
}

//write(int fd,void *buf,int size):用户空间将数据写到内核空间
static ssize_t led_write(struct file *file,const char __user *user_buf,size_t user_len, loff_t *pos)
{
	printk("led write sucess!\n");

	if(user_len > sizeof(pled->led_state)){
		printk("Fail to led write!\n");
		return -EINVAL;
	}
	
	//strncpy(pled->led_state,user_buf,user_len);	
	if (copy_from_user(pled->led_state, user_buf, user_len)) {
		printk("Failed to copy from user\n");
		return -EFAULT;
	}

	printk("led state : %s\n",pled->led_state);

	return user_len;
}

static const struct file_operations led_registers_fops = {
	.open = led_open,
	.release = led_release,
	.read		= led_read,
	.write		= led_write,
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
