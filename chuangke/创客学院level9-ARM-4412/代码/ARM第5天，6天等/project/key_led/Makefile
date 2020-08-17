CROSS_COMPLINER = arm-none-eabi-
CC = $(CROSS_COMPLINER)gcc
LD = $(CROSS_COMPLINER)ld
CFLAG = -c -g -O0
OBJS = led.o main.o

led:
	$(LD) $(OBJS) -Ttext 0x40008000 -o exynos4412.elf
	$(CROSS_COMPLINER)objcopy  -O binary exynos4412.elf key_zhaoyang.bin
	
%.o:%.c
	$(CC) $(CFLAG) $< -o $@

%.o:%.s
	$(CC) $(CFLAG) $< -o $@
	
clean:
	rm -rf *.o *.elf






