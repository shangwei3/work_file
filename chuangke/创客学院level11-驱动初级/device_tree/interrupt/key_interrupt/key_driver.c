#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL v2");

//isr:interrupt service routeline
static irqreturn_t key_isr(int irq, void *devid)
{
	printk("irq : %d\n",irq);
	printk("devid : %p\n",devid);
	
	return IRQ_HANDLED;
}

static int key_probe(struct platform_device *pdev)
{
	int ret;
	struct resource *res;
	
	res = platform_get_resource(pdev,IORESOURCE_IRQ,0);
	if(!res){
		printk("Fail to platform get resource!\n");
		return -ENODEV;
	}

	printk("interrupt num : %d\n",res->start);
	printk("resource flags : %#x\n",res->flags);
	printk("resource name : %s\n",res->name);
	printk("platform_get_irq : %d\n",platform_get_irq(pdev,0));
	

	//×¢²áÖÐ¶Ï
	ret = devm_request_irq(&pdev->dev,res->start,key_isr,IRQF_TRIGGER_MASK & res->flags,res->name,NULL);
	if (ret < 0) {
		printk("failed to install IRQ: %d\n", ret);
		return ret;
	}
	
	return 0;
}

static int key_remove(struct platform_device *pdev)
{
	//×¢ÏúÖÐ¶Ï
	//free_irq(platform_get_irq(pdev,0),NULL);
	return 0;
}

static const struct of_device_id key_of_match[] = {
	{ .compatible = "fs4412-key" },
	{ /* Sentinel */ }
};


static struct platform_driver key_platform_driver = {
	.driver = {
		.name		= "key",
		.owner		= THIS_MODULE,
		.of_match_table	= key_of_match,
	},
	.probe			= key_probe,
	.remove			= key_remove,
};


int key_init(void)
{
   return platform_driver_register(&key_platform_driver);
}

void key_exit(void)
{
   platform_driver_unregister(&key_platform_driver);
}

module_init(key_init);
module_exit(key_exit);