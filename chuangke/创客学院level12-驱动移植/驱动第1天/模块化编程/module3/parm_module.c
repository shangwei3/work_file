#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

static int var1 = 0;
module_param(var1, int, 0644);
MODULE_PARM_DESC(var1, "Get value from user.\n");

static int var2 = 0;
module_param_named(var2_out, var2, int, 0644);
MODULE_PARM_DESC(var2, "Test var2 named var2_out.\n");

static char *string = NULL;
module_param(string,charp, 0444);
MODULE_PARM_DESC(string, "Test module param string.\n");

static char  buffer[10];
module_param_string(buffer, buffer, sizeof(buffer), 0644);
MODULE_PARM_DESC(buffer, "Test module param string buffer.\n");

static int myarry[3];
int num;
module_param_array(myarry,int,&num, 0444);
MODULE_PARM_DESC(myarry, "Test module param arry.\n");


int __init init_parm_module(void)
{
	int i = 0;
	
	printk("-----------------------------\n");
	printk("var1   : %d\n",var1);
	printk("var2   : %d\n",var2);
	printk("string : %s\n",string);
	printk("buffer : %s\n",buffer);

	for(i = 0;i < num;i ++)
	{
		printk("myarry[%d] : %d\n",i,myarry[i]);
	}
	printk("-----------------------------\n");
	
	return 0;
}

void __exit exit_parm_module(void)
{
	printk("exit parm module\n");
	return;
}

module_init(init_parm_module);
module_exit(exit_parm_module);
