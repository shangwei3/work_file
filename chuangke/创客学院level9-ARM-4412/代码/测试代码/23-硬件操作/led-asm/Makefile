all:led.s
	arm-none-eabi-gcc -g -c -O0 led.s -o start.o
	arm-none-eabi-ld start.o  -Ttext 0x40008000 -o start.elf
	arm-none-eabi-objcopy -O binary start.elf start.bin
clean:
	rm *.elf *.o
