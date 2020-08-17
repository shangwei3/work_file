#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "led.h"

//./a.out  /dev/led
int main(int argc, const char *argv[])
{
	int fd;
	int ret;

	if(argc < 2){
		fprintf(stderr,"Usage : %s /dev/<device_name> !\n",argv[0]);
		return -1;
	}


	fd = open(argv[1],O_RDWR);
	if(fd < 0){
		fprintf(stderr,"Fail to open %s : %s!\n",argv[1],strerror(errno));
		return -1;
	}

	while(1)
	{
		ret = ioctl(fd,LED_IOC_ON);
		if(ret < 0){
			perror("Fail to ioctl");
			return -1;
		}

		sleep(1);

		ret = ioctl(fd,LED_IOC_OFF);
		if(ret < 0){
			perror("Fail to ioctl");
			return -1;
		}

		sleep(1);
	}

	close(fd);

	return 0;
}
