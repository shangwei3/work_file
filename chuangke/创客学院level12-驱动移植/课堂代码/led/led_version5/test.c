#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, const char *argv[])
{
	int n;
	int fd;
	char led_state[15];
	char *led_on = "led on";
	char *led_off = "led off";

	fd = open("/dev/led",O_RDWR);
	if(fd < 0){
		perror("Fail to open\n");
		return -1;
	}

	n = write(fd,led_on,strlen(led_on) + 1);
	if(n < 0){
		perror("Fail to write\n");
		return -1;
	}

	printf("write success!\n");

	n = read(fd,led_state,sizeof(led_state));
	if(n < 0){
		perror("Fail to read\n");
		return -1;
	}
	printf("Read %d bytes : %s\n",n,led_state);

	close(fd);
	
	return 0;
}
