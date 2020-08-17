#ifdef MODULE
#include <linux/module.h>
#include <linux/version.h>
#else
#define MOD_INC_USE_COUNT
#define MOD_DEC_USE_COUNT
#endif
//#define __KERNEL__

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
//#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
//#include <linux/mm.h>
//#include <linux/poll.h>
//#include <linux/slab.h>
//#include <linux/ioport.h>
#include <asm/uaccess.h>
//#include <asm/io.h>
#include <mach/regs-gpio.h>
//#include <linux/fcntl.h>

MODULE_LICENSE ("GPL");

#define LED_ON 0x4800
#define LED_OFF 0x4801

#define GPG3CON *(volatile unsigned int *)S5PC100_GPG3_BASE
#define GPG3DAT *(volatile unsigned int *)(S5PC100_GPG3_BASE + 0x4)

static int major = 252;
static int minor = 0;
static dev_t devno;
static struct cdev cdev;

static int led_ioctl(struct inode *s_node,struct file *s_file,unsigned int cmd,unsigned long arg)
{
   switch(cmd)
   {
   case LED_ON:
		GPG3DAT |= 0x0f;
		break;
   case LED_OFF:
	 	GPG3DAT &= ~0x0f;
		break;
   }

   return 0;
}

static struct file_operations led_fops=
{
  .ioctl = led_ioctl
};

static void char_reg_setup_cdev(void)
{
	int error;

	cdev_init(&cdev, &led_fops);
	cdev.owner = THIS_MODULE;
	error = cdev_add(&cdev, devno, 1);
	if (error)
		printk (KERN_NOTICE "Error %d adding char_reg_setup_cdev\n", error);

	return;
}

static int __init led_init(void)
{
	int result;
  
	devno = MKDEV(major, minor);
	result = register_chrdev_region (devno, 1 , "led");
	if (result < 0)
	{
		printk (KERN_WARNING "gpio : can't get major number %d\n", major);
		return result;
	}
    
	char_reg_setup_cdev();
	GPG3CON &= ~0xffff;
	GPG3CON |= 0x1111;
	printk (KERN_INFO "led driver registered\n");

	return 0;
}
    
static void __exit led_exit(void)
{
	cdev_del(&cdev);
	unregister_chrdev_region(devno, 1);
	printk (KERN_INFO "led driver unregistered\n");

	return;
}

module_init(led_init);
module_exit(led_exit);
