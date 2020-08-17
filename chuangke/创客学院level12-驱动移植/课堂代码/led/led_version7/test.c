#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "led.h"

int main(int argc, const char *argv[])
{
	int n;
	int fd;
	char led_state[15];
	char *led_on = "led on";
	char *led_off = "led off";

	fd = open("/dev/led2",O_RDWR);
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
