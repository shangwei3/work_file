#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "led.h"


//./test /dev/led0
//               1
//               2
//               3
int main(int argc, const char *argv[])
{
	int fd;
	int n;
	char led_state[20];
	char *led_on = "led on";
	
	if(argc < 2){
		fprintf(stderr,"Usage : %s /dev/led*\n",argv[0]);
		return -1;
	}

	fd = open(argv[1],O_RDWR);
	if(fd < 0){
		perror("Fail to open");
		return -1;
	}
	
	while(1){
		n = ioctl(fd,LEDIOC_ON);
		if(n < 0){
			perror("Fail to ioctl\n");
			return -1;
		}

		sleep(1);
		
		n = ioctl(fd,LEDIOC_OFF);
		if(n < 0){
			perror("Fail to ioctl\n");
			return -1;
		}

		sleep(1);
	}

	close(fd);
	
	return 0;
}
