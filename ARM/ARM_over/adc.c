#include "exynos_4412.h"

void adc_init(void)
{
	
	ADCCON |= 1<<16;//配置转换位数
	ADCCON |= 1<<14;//配置二预分频使能
	ADCCON = (ADCCON & ~(0xff << 6 )) | (19<<6);//配置预分频为１９ 
	ADCCON &= ~(3<<1);//配置为普通工作模式
	ADCMUX = 3;//配置ＡＤＣ输入通道
}

unsigned short get_adc_data(void)
{
	unsigned short data;
	ADCCON  |= 1;//1、启动ADC装换
	while(!(ADCCON & (1<<15)));//2、等待装换完成
	data = ADCDAT & 0xfff;//3、获得结果
	return  data;
}


