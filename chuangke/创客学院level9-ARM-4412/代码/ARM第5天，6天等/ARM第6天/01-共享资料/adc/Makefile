#version 1.0
#author: luoxl@farsight.com.cn
#modify time: 2016.4.5

CROSS_COMPLINER = arm-none-eabi-
CC = $(CROSS_COMPLINER)gcc
LD = $(CROSS_COMPLINER)ld
CFLAG = -c -g -O0
OBJS = start.o main.o libprintf.a

#INC = -I "H:\eclipseArm\yagarto\lib\gcc\arm-none-eabi\4.6.2\include"

#LDFLAGS += -static -L\
# "H:\eclipseArm\yagarto\arm-none-eabi\lib" -lc -lm

#LDFLAGS += -static -L\
# "H:\eclipseArm\yagarto\lib\gcc\arm-none-eabi\4.6.2" -lgcc
 
LDFLAGS +=-L ./ -lprintf


all:$(OBJS)
	$(LD) $(OBJS) -Ttext 0x40008000 -o exynos4414.elf
	

libprintf.a:
	make -C ./printf 

%.o:%.c
	$(CC) $(CFLAG)  $< -o $@

%.o:%.s
	$(CC) $(CFLAG) $< -o $@	
	
clean:
	rm -rf *.o *.elf
	make -C ./printf clean
