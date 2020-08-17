#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define LED_DEVICE_MAJOR 250

MODULE_LICENSE("GPL v2");

//1.针对字符设备的通用描述
struct led_device
{
    struct cdev cdev;
};

struct led_device *pled;

static int led_open(struct inode *inode, struct file *file)
{
    printk("led open is success\n");
        return 0;
}

static int led_release(struct inode *inode, struct file *file)
{
    printk("led release is success\n");
        return 0;
}


static const struct file_operations led_registers_fops = {
    .open = led_open,
        //.read = seq_read,
        //.llseek = seq_lseek,
        .release = led_release,
        .owner = THIS_MODULE,
};

int led_init(void)
{
    int retval;
        
        printk("led init\n");
        
        pled = kmalloc(sizeof(*pled), GFP_KERNEL);
        
        if (!pled)
        {
            printk("out of memory");
                return -ENOMEM;
        }
    
        //2.提供硬件设备的函数接口
        cdev_init(&pled->cdev, &led_registers_fops);
        
        //3.申请一个空闲的设备号
        retval = register_chrdev_region(MKDEV(LED_DEVICE_MAJOR,0), 
            1, "led_driver");
        
        if (retval)
        {
            printk("fail to register chrdev region for led driver\n");
                goto err_register_chrdev_region;
        }
    
        //4.添加字符设备
        
        retval = cdev_add(&pled->cdev, MKDEV(LED_DEVICE_MAJOR,0), 1);

        if (retval < 0)
        {
            pr_warn("cannot add cdev\n");
                goto err_cdev_add;
        }
    
        return 0;
        
        err_cdev_add:
        //删除设备
        unregister_chrdev_region(MKDEV(LED_DEVICE_MAJOR,0), 1);		
        
        err_register_chrdev_region:
        //释放pled空间
        kfree(pled);
        return(retval);	
}

//----------------------------------------------------------------------
void led_exit()
{
    cdev_del(&pled->cdev);
    unregister_chrdev_region(MKDEV(LED_DEVICE_MAJOR,0), 1);
    kfree(pled);
    printk("led exit");
        return ;
        
}

module_init(led_init);
module_exit(led_exit);


















