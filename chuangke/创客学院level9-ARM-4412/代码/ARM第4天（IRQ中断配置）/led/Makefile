all:start.S
	arm-none-eabi-gcc -g -c -O0 start.S -o start.o
	arm-none-eabi-gcc -g -c -O0 main.c  -o main.o
	arm-none-eabi-ld start.o main.o -Ttext 0x40008000 -o start.elf
	arm-none-eabi-objcopy -O binary start.elf start.bin
	#arm-none-eabi-objdump -D  start.elf 
clean:
	rm *.elf *.o
