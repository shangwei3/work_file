#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main(int argc, const char *argv[])
{
    int fd;
    fd = open("/dev/led",O_RDWR);
    if(fd<0)
    {
        perror("Fail to open\n");
        return -1;
    }


    close(fd);
    return 0;
}
