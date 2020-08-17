#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, const char *argv[])
{
	int fd;
	int n;
	char led_state[20];
	char *led_on = "led on";

	fd = open("/dev/led",O_RDWR);
	if(fd < 0){
		perror("Fail to open");
		return -1;
	}
	
	n = write(fd,led_on,strlen(led_on) + 1);
	printf("write %d bytes!\n",n);
	
	n = read(fd,led_state,sizeof(led_state));
	printf("Read %d bytes : %s\n",n,led_state);

	close(fd);
	
	return 0;
}
