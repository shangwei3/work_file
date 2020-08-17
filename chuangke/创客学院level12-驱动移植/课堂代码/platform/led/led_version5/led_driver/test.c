#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "led.h"

//./test /dev/fs4412-led.0
int main(int argc, const char *argv[])
{
	int n;
	int fd;
	
	if(argc < 2){
		fprintf(stderr,"Usage : %s /dev/name\n",argv[0]);
		return -1;
	}

	fd = open(argv[1],O_RDWR);
	if(fd < 0){
		perror("Fail to open\n");
		return -1;
	}

	while(1){
		ioctl(fd,LEDIOC_ON);
		sleep(1);
		ioctl(fd,LEDIOC_OFF);
		sleep(1);
	}


	close(fd);
	
	return 0;
}
