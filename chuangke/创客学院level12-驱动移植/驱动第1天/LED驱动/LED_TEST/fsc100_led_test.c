#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define LED_ON 0x4800
#define LED_OFF 0x4801

int main(void)
{
   int fd;

   if((fd = open("/dev/led", O_RDWR)) == -1)
   {
      perror("open error");
      exit(-1);
   }
   printf("open led ok\n");

   while(1)
   {  
	   ioctl(fd, LED_ON, NULL); //GPG3 1
	   usleep(500000);
       ioctl(fd, LED_OFF, NULL);  //GPG3 0
	   usleep(500000);
   }
   close(fd);
   
   return 0;
}
