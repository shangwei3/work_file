#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include "head.h"

int main(int argc, const char *argv[])
{
	int fd;
	int n;
	int adc_data;
	
	fd = open("/dev/exynos4412-adc",O_RDWR);
	if(fd < 0){
		perror("Fail to open");
		exit(EXIT_FAILURE);
	}
	
	//初始化
	ioctl(fd,ADC_DEVICE_INIT); //设置ADC控制器的值

	while(1)
	{
		//使能ADC转换
		ioctl(fd,ADC_DEVICE_START);
		
				
		//读取ADC的值:如果AD采集没有完成，此时我们应该让进程阻塞，直到采集数据完成
		n = read(fd,&adc_data,sizeof(adc_data));
		printf("adc_data : %d\n",adc_data);
		printf("%fv\n",1.8 * ((float)adc_data/ 0xfff));
		sleep(1);
	}

	close(fd);
	return 0;
}
