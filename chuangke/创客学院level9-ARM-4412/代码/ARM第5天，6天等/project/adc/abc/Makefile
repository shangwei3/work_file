CC=arm-none-eabi-gcc
AR =arm-none-eabi-ar

CFLAGS=-c -g  

OBJS+=_divsi3.o
OBJS+=_udivsi3.o
OBJS+=_umodsi3.o
OBJS+=printf.o

all:$(OBJS)
	$(AR) -cr ../libprintf.a  *.o	

#…Ë÷√±‡“ÎπÊ‘Ú
printf.o:printf.c
	$(CC) $(CFLAGS) $< -o $@ 
	
%.o:%.S
	$(CC) $(CFLAGS) $< -o $@ 
	
clean:
	rm -rf *.o *.a