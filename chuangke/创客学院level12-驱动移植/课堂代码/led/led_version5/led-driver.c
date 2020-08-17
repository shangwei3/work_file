/*
1.��ζ�̬��ȡ�豸��,ϣ�������Զ�������е��豸��
2.����Զ������豸�ļ�
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define LED_MAJOR      251
#define LED_BASE_MINOR 0
#define LED_BASE_DEV   MKDEV(LED_MAJOR, LED_BASE_MINOR)

MODULE_LICENSE("GPL v2");

struct led_device{
	dev_t  devno;
	char   state[15];
	struct cdev cdev;
	struct class *led_class;
	struct device *dev;
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

/*@struct file *file -> fd
 *@char  *data       -> user addr
 *@len               -> д���ֽ�
 *@ppos              -> �ļ�ƫ����
 *
 *�û��ռ佫���ݸ��ں˿ռ�
 */
static ssize_t led_write(struct file *file, const char *data,size_t len, loff_t *ppos)
{
	//strcpy  memcpy
	if (copy_from_user(pled->state,data,len)) {
		printk("Fail to copy from user");
		return -EFAULT;
	}

	printk("led write , state : %s\n",pled->state);

	//*ppos += len;

	return len;
}

//���ں˿ռ����ݿ������û��ռ� 
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

	//�����豸����Ϣ
	//1.����һ����
	pled->led_class = class_create(THIS_MODULE, "led");
	if (IS_ERR(pled->led_class)){//�ж����Ƿ����
		printk("Fail to class create\n");
		ret = PTR_ERR(pled->led_class);//��ȡ������
		goto err_class_create;	
	}
	
	//2.����һ���豸
	pled->dev = device_create(pled->led_class,NULL,pled->devno, NULL,"led");
	if (IS_ERR(pled->dev)) {
		printk("Fail to device create\n");
		ret = PTR_ERR(pled->dev);
		goto err_device_create;
	}

	
	return 0;
	
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
	
	device_destroy(pled->led_class,pled->devno);
	class_destroy(pled->led_class);
	cdev_del(&pled->cdev);
	unregister_chrdev_region(pled->devno,1);//�ͷ��豸��
	kfree(pled);
	
	return;
}

module_init(led_init);
module_exit(led_exit);
