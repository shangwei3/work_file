/*
1.如何动态获取设备号,希望操作自动分配空闲的设备号
2.如何自动创建设备文件
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <asm/io.h>

#define LED_MAJOR      251
#define LED_BASE_MINOR 0
#define LED_BASE_DEV   MKDEV(LED_MAJOR, LED_BASE_MINOR)

#define LED_REG_BASE   0x11000C40 
#define LED_REG_SIZE   8

enum{
	CON = 0,
	DAT = 4,
};


MODULE_LICENSE("GPL v2");

struct led_device{
	dev_t  devno;
	char   state[15];
	struct cdev cdev;
	struct class *led_class;
	struct device *dev;
	void		__iomem *regs;
};

struct led_device *pled;

void led_on(void)
{
	int reg_value;

	reg_value = readl(pled->regs + DAT);
	reg_value |= (1 << 7);
	writel(reg_value, pled->regs + DAT);

	return;
}

void led_off(void)
{
	int reg_value;

	reg_value = readl(pled->regs + DAT);
	reg_value &= ~(1 << 7);
	writel(reg_value, pled->regs + DAT);

	return;
}


static int led_open(struct inode *inode, struct file *file)
{
	int reg_value;
	
	printk("led open success!\n");
	
	reg_value = readl(pled->regs + CON);
	reg_value &= ~(0xf << 28);
	reg_value |=  (0x1 << 28);
	writel(reg_value, pled->regs + CON);
	
	return 0;
}

static int led_release(struct inode *inode, struct file *file)
{
	printk("led release!\n");
	led_off();
	return 0;
}


/*@struct file *file -> fd
 *@char  *data       -> user addr
 *@len               -> 写的字节
 *@ppos              -> 文件偏移量
 *
 *用户空间将数据给内核空间
 */
static ssize_t led_write(struct file *file, const char *data,size_t len, loff_t *ppos)
{
	//strcpy  memcpy
	if (copy_from_user(pled->state,data,len)) {
		printk("Fail to copy from user");
		return -EFAULT;
	}

	printk("led write , state : %s\n",pled->state);

	if(strcmp(pled->state,"led on") == 0){
		led_on();//点亮
	}else if(strcmp(pled->state,"led off") == 0){
		led_off();//点灭
	}

	//*ppos += len;

	return len;
}

//将内核空间数据拷贝到用户空间 
ssize_t led_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	int len = strlen(pled->state) + 1;
	
	if (copy_to_user(buf,pled->state,len)){
		printk("Fail to copy to user\n");
		return -EFAULT;
	}

	return  len;
}


static const struct file_operations led_fops = {
	.owner		= THIS_MODULE,
	.open		= led_open,
	.write		= led_write,
	.read       = led_read,
	.release	= led_release,
};


int led_init(void)
{
	int ret;

	
	printk("led init\n");

	pled = kmalloc(sizeof(*pled),GFP_KERNEL);
	if (!pled) {
		printk("Fail to kmalloc\n");
		return -ENOMEM;
	}

	//pled->cdev.ops = &led_fops;
	cdev_init(&pled->cdev,&led_fops);


	//静态注册设备号
	ret = register_chrdev_region(LED_BASE_DEV,1,"led2");
	if (ret) {
		printk("led:couldn't register device number\n");
		//如果失败，则动态获取
		ret = alloc_chrdev_region(&pled->devno,0,1,"led2");
		if(ret){
			printk("led:couldn't alloc device number\n");
			goto err_register_chrdev_region;
		}
		
	}else{
		pled->devno = LED_BASE_DEV;
	}

	//添加cdev到系统
	ret = cdev_add(&pled->cdev,pled->devno, 1);
	if (ret){
		printk("led:cdev add failure\n");
		goto err_cdev_add;
	}

	//导出设备号信息
	//1.创建一个类
	pled->led_class = class_create(THIS_MODULE, "fs4412-led");
	if (IS_ERR(pled->led_class)){//判断是是否出错
		printk("Fail to class create\n");
		ret = PTR_ERR(pled->led_class);//获取错误码
		goto err_class_create;	
	}
	
	//2.创建一个设备
	pled->dev = device_create(pled->led_class,NULL,pled->devno, NULL,"led2");
	if (IS_ERR(pled->dev)) {
		printk("Fail to device create\n");
		ret = PTR_ERR(pled->dev);
		goto err_device_create;
	}

	//映射物理地址
	pled->regs = ioremap(LED_REG_BASE,LED_REG_SIZE);
	if (!pled->regs) {
		printk("Unable to map I/O register");
		goto err_ioremap;
	}

	
	return 0;

err_ioremap:
	device_destroy(pled->led_class,pled->devno);
	
err_device_create:
	class_destroy(pled->led_class);
	
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
	printk("led exit\n");

	iounmap(pled->regs);
	device_destroy(pled->led_class,pled->devno);
	class_destroy(pled->led_class);
	cdev_del(&pled->cdev);
	unregister_chrdev_region(pled->devno,1);//释放设备号
	kfree(pled);
	
	return;
}

module_init(led_init);
module_exit(led_exit);
