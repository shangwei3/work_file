#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL v2");

struct led_des{
	int pin;
	int mode;
	int level;
};

//���豸������ƥ��ɹ�֮�󣬲���ϵͳ���Զ�����probe����
//���Ұ������豸��Ϣ�Ľṹ��struct platform_device���ݹ���
//���������ǾͿ���ͨ��platform_device�ṹ���õ��豸��������Ϣ
static int led_probe(struct platform_device *pdev)
{
	struct resource *res; 
	struct led_des *platdata;
		
	printk("match: led probe!\n");
	printk("match: driver data : %ld\n",pdev->id_entry->driver_data);

	//1.��ȡ��Դ:�Ĵ�����Դ,ƽ̨˽�е���Դ
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res){
		printk("Fail to platform get resource\n");
		return -ENODEV;
	}
	printk("reg phy addr : %#x\n",res->start);

	//platdata = pdev->dev.platform_data;
	platdata = dev_get_platdata(&pdev->dev);
	if(!platdata){
		printk("No device private info!\n");
		return -ENODEV;
	}

	printk("pin:%d , mode:%d , level : %d\n",platdata->pin,platdata->mode,platdata->level);
	
	
	//2.ע���ַ��豸
	
	return 0;
}

static int led_remove(struct platform_device *pdev)
{
	printk("detach:led remove!\n");
	return 0;
}

static struct platform_device_id led_driver_ids[] = {
	{
		.name	= "fs4412-led",
		.driver_data	= 1,
	}, {
		.name	= "samsung-led",
		.driver_data	= 0,
	},
	{ },
};

static struct platform_driver led_driver = {
	.probe          = led_probe,
	.remove         = led_remove,
	.id_table       = led_driver_ids,
	.driver         = {
		.name   = "led",
		.owner  = THIS_MODULE,
	},
};


int led_driver_init(void)
{
	return platform_driver_register(&led_driver);
}

void led_driver_exit(void)
{
	platform_driver_unregister(&led_driver);
}

module_init(led_driver_init);
module_exit(led_driver_exit);
