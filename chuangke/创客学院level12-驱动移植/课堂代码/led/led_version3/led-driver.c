/*
1.��ζ�̬��ȡ�豸��,ϣ�������Զ�������е��豸��
2.����Զ������豸�ļ�
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#define LED_MAJOR      251
#define LED_BASE_MINOR 0
#define LED_BASE_DEV   MKDEV(LED_MAJOR, LED_BASE_MINOR)

MODULE_LICENSE("GPL v2");

struct led_device{
	dev_t  devno;
	struct cdev cdev;
};

struct led_device *pled;

static int led_open(struct inode *inode, struct file *file)
{
	printk("led open success!\n");
	return 0;
}

static int led_release(struct inode *inode, struct file *file)
{
	printk("led release!\n");
	return 0;
}


static const struct file_operations led_fops = {
	.owner		= THIS_MODULE,
	.open		= led_open,
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


	//��̬ע���豸��
	ret = register_chrdev_region(LED_BASE_DEV,1,"led");
	if (ret) {
		printk("led:couldn't register device number\n");
		//���ʧ�ܣ���̬��ȡ
		ret = alloc_chrdev_region(&pled->devno,0,1,"led");
		if(ret){
			printk("led:couldn't alloc device number\n");
			goto err_register_chrdev_region;
		}
		
	}else{
		pled->devno = LED_BASE_DEV;
	}

	//���cdev��ϵͳ
	ret = cdev_add(&pled->cdev,pled->devno, 1);
	if (ret){
		printk("led:cdev add failure\n");
		goto err_cdev_add;
	}
	
	return 0;
	
err_cdev_add:
	unregister_chrdev_region(pled->devno,1);
	
err_register_chrdev_region:
	kfree(pled);
	return ret;
	
}

void led_exit(void)
{
	printk("led exit\n");

	cdev_del(&pled->cdev);
	unregister_chrdev_region(pled->devno,1);//�ͷ��豸��
	kfree(pled);
	
	return;
}

module_init(led_init);
module_exit(led_exit);
