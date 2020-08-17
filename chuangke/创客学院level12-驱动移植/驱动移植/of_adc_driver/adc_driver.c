#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/mod_devicetable.h>
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/of.h>
#include "head.h"

#define MAJOR_NUM 251

MODULE_LICENSE("GPL");

enum{ADC_CON=0,ADC_DAT=12,ADC_CLRINT = 0x18,ADC_MUX = 0x1c};

struct adc_device 
{
	void *reg;
	int major_num;
	int irq_num;
	int adc_data;
	int io_channer;
	struct clk *adc_clk;
	struct resource *res;
	struct class *cls;
	struct device *dev;
	struct cdev cdev;
	wait_queue_head_t read_event_wait;
	struct tasklet_struct adc_tasklet;
};

irqreturn_t adc_handler(int irq, void *dev_id)
{
	struct adc_device *padc = dev_id;
	
	//���adc�ж�
	writel(1,padc->reg + ADC_CLRINT);

	//����tasklet
	tasklet_schedule(&padc->adc_tasklet);

	return IRQ_HANDLED;
}

void adc_tasklet_handler(unsigned long data)
{
	struct adc_device *padc = (struct adc_device *)data;
	padc->adc_data = readl(padc->reg + ADC_DAT) & 0xfff;
	//printk("%dmv\n",1800 * padc->adc_data / 0xffff);
	
	//���ѵȴ��Ķ�����
	wake_up_interruptible(&padc->read_event_wait);	
	return;
	
}

int adc_open (struct inode *inode, struct file *file)
{
	int ret;
	struct adc_device *padc = container_of(inode->i_cdev,struct adc_device,cdev);	

	printk("adc_open success!\n");
	file->private_data = padc;

	padc->adc_data = -1;

	ret = request_irq(padc->irq_num,adc_handler,IRQF_TRIGGER_NONE,"adc",padc);
	if (ret){
		if (ret == -EBUSY) {
			 printk("request_irq(): IRQ 0x%xalready in use\n",padc->irq_num);
		} else if (ret == -EINVAL) {
			 printk("request_irq(): IRQ 0x%x not valid\n",padc->irq_num);
	  	} else {
			 printk("request_irq(): IRQ 0x%x failed with %d\n",padc->irq_num, ret);
	    }
		
		return ret;
	}


	clk_enable(padc->adc_clk);//��ʱ���ź�
	
	return 0;
}

ssize_t adc_read(struct file *file, char __user *ubuf, size_t size, loff_t *ppos)
{
	int ret;
	struct adc_device *padc = file->private_data;	


#if 1
	while(padc->adc_data < 0){
		//�ȴ�������
		ret = wait_event_interruptible(padc->read_event_wait,padc->adc_data > 0);
		if(ret < 0){
			return -ERESTARTSYS;
		}
	}
#endif
	

	//put_user
	ret = copy_to_user(ubuf,&padc->adc_data,sizeof(padc->adc_data));
	if(ret){
		printk("Fail to copy_to_user\n");
		return  -EFAULT;
	}

	padc->adc_data = -1;
	
	return sizeof(padc->adc_data);
}

long adc_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int reg_val;
	struct adc_device *padc = file->private_data;

	switch(cmd)
	{
		case  ADC_DEVICE_INIT:
			  //��ʼ��ADC������
			  reg_val = (133 << 6) | (1 << 14) |(1 << 16); 
			  writel(reg_val, padc->reg + ADC_CON);

			  //��ʼ��ͨ��
			  writel(padc->io_channer, padc->reg + ADC_MUX);
			  break;

		case  ADC_DEVICE_START:
			  //ʹ��ADCת��
		      reg_val = readl(padc->reg + ADC_CON);
	          reg_val |=  (0x1 << 0);
	          writel(reg_val,padc->reg + ADC_CON);
			  break;

		case  ADC_DEVICE_STOP:
			  //disable adc clock
			  clk_disable(padc->adc_clk);
			  break;
			  
		default:
			  printk("Unknown cmd\n");
			  return -EINVAL;
	}

	return 0;
}

int adc_release(struct inode *inode, struct file *file)
{
	
	struct adc_device *padc = container_of(inode->i_cdev,struct adc_device,cdev);	

	printk("adc_release success!\n");

	//�ͷ��жϺ���Դ
	free_irq(padc->irq_num, padc);

	//disable adc clock
	clk_disable(padc->adc_clk);

	return 0;
}

struct file_operations adc_fops = {
	.owner          = THIS_MODULE,
	.open           = adc_open,
	.read           = adc_read,
	.unlocked_ioctl = adc_unlocked_ioctl,
	.release        = adc_release,
};


int register_exynosadc_chrdev(struct platform_device *pdev)
{
	int ret;
	dev_t dev_num;
	struct adc_device *padc = platform_get_drvdata(pdev);

	//��ʼ���ַ��豸
	cdev_init(&padc->cdev,&adc_fops);

	//dev_num = MAJOR_NUM << 20 | 0;
	dev_num = MKDEV(MAJOR_NUM,0);
	
	//����һ���豸��
	ret = register_chrdev_region(dev_num,1,pdev->name);
	if (ret < 0) {

		//��̬�����豸��
		ret = alloc_chrdev_region(&dev_num,0,1,pdev->name);
		if(ret){
			printk("Fail to alloc_chrdev_region\n");
			goto err_alloc_chrdev_region;
		}
	}

	//��¼���豸��
	padc->major_num = MAJOR(dev_num);

	//����ַ��豸
	ret = cdev_add(&padc->cdev,dev_num,1);
	if (ret < 0) {
		printk("cannot add character device");
		goto err_cdev_add;
	}

	//������ 
	padc->cls = class_create(THIS_MODULE,"adc");
	if (IS_ERR(padc->cls)) {
		printk("Fail to class_create\n");
		ret = PTR_ERR(padc->cls);
		goto  err_class_create;
	}

	//�����´����豸(��Ӧ�ò㵼���豸����Ϣ)
	padc->dev = device_create(padc->cls,NULL,dev_num,NULL,"exynos4412-adc");
	if (IS_ERR(padc->dev)) {
		printk("Fail to device_create\n");
		ret = PTR_ERR(padc->dev);
		goto  err_device_create;
	}


	return 0;

err_device_create:
	class_destroy(padc->cls);
	
err_class_create:
	cdev_del(&padc->cdev);
	
err_cdev_add:	
	//�ͷ��豸��
	unregister_chrdev_region(dev_num,1);
	
err_alloc_chrdev_region:
	kfree(padc);

	return ret;
}


int exynosadc_probe(struct platform_device *pdev)
{
	int ret;
	int io_resource_size;
	struct adc_device  *padc;

	//����ռ�
	padc = kzalloc(sizeof(struct adc_device),GFP_KERNEL);
	if (padc == NULL){
		printk("Fail to kzalloc\n");
		ret =  -ENOMEM;
		goto err_kzalloc;
	}

	platform_set_drvdata(pdev,padc);

	//�����ж���Դ
	padc->irq_num = platform_get_irq(pdev, 0);
	if(padc->irq_num < 0){
		printk("Fail to platform_get_irq\n");
		ret = padc->irq_num;
		goto err_platform_get_irq;
	}

	printk("interrupt num : %d\n",padc->irq_num);

	//������Դ
	padc->res = platform_get_resource(pdev,IORESOURCE_MEM,0);
	if (!padc->res) {
		printk("Fail to  platform_get_resource\n");
		ret = -ENXIO;
		goto  err_platform_get_resource;
	}

	io_resource_size = resource_size(padc->res);

	//ռ��IO��Դ
	if(!request_mem_region(padc->res->start,io_resource_size,pdev->name))
	{	
		printk("Fail to request_mem_region\n");
		ret = -EBUSY;
		goto  err_request_mem_region;
	}

	
	//��ȡadc clock
	padc->adc_clk = clk_get(&pdev->dev, "adc");
	if (IS_ERR(padc->adc_clk)) {
		printk("failed to get ts_clk\n");
		ret =  PTR_ERR(padc->adc_clk);
		goto err_clk_get;
	}


	//ӳ��Ĵ�����ַ
	padc->reg = ioremap(padc->res->start,io_resource_size);
	if (!padc->reg) {
		printk("faiadc to map registers\n");
		ret = -ENXIO;
		goto err_ioremap;
	}
	
	/*��ȡadc-io-channer*/
	of_property_read_u32(pdev->dev.of_node, "adc-io-channer", &padc->io_channer);
	
	//ע��adc�ַ��豸
	ret = register_exynosadc_chrdev(pdev);
	if(ret < 0){
		goto err_register_exynosadc_chrdev;
	}

	//��ʼ���ȴ�����ͷ
	init_waitqueue_head(&padc->read_event_wait);

	//��ʼ��tasklet
	tasklet_init(&padc->adc_tasklet,adc_tasklet_handler,(long unsigned  int)padc);

	return 0;	

err_register_exynosadc_chrdev:
	iounmap(padc->reg);
	
err_clk_get:	
err_ioremap:
	release_resource(padc->res);
	
err_request_mem_region:
err_platform_get_resource:
err_platform_get_irq:
	kfree(padc);
	
err_kzalloc:
	return ret;
}

int exynosadc_remove(struct platform_device *pdev)
{
	dev_t dev_num;
	struct adc_device *padc = platform_get_drvdata(pdev);

	dev_num= MKDEV(padc->major_num,0);
	device_destroy(padc->cls, dev_num);
	class_destroy(padc->cls);
	cdev_del(&padc->cdev);
	unregister_chrdev_region(dev_num,1);
	iounmap(padc->reg);
	release_resource(padc->res);
	kfree(padc);

	return 0;
}

#if defined(CONFIG_OF)
static const struct of_device_id exynos_adc_dt_ids[] = {
	{ .compatible = "Samsung,exynos4412-adc" },
	{ /* sentinel */ },
};

MODULE_DEVICE_TABLE(of, exynos_adc_dt_ids);
#endif


struct platform_driver exynosadc_driver = {
	.probe    = exynosadc_probe,
	.remove   = exynosadc_remove,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "exynos-adc",
		.of_match_table = of_match_ptr(exynos_adc_dt_ids),
	},
};


int adc_device_init(void)
{
	return platform_driver_register(&exynosadc_driver);
}

void adc_device_exit(void)
{
	return platform_driver_unregister(&exynosadc_driver);
}

module_init(adc_device_init);
module_exit(adc_device_exit);
