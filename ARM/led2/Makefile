NAME =led
CFLAGS += -g   -O0  -mabi=apcs-gnu -mfpu=neon -mfloat-abi=softfp  -fno-builtin -nostdinc
CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld
OBJCOPY = arm-none-linux-gnueabi-objcopy

all:
	$(CC)  		$(CFLAGS)  -c start.S -o  start.o
	$(CC)  		$(CFLAGS)  -c main.c  -o  main.o
	$(LD)   		start.o main.o -T map.lds -o $(NAME).elf
	$(OBJCOPY)	-O binary  $(NAME).elf $(NAME).bin

clean:
	rm  *.elf *.bin *.o
