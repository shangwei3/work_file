all:
	arm-none-eabi-gcc   -g -c -o sum.o sum.s
	arm-none-eabi-ld	sum.o -Ttext 0x34000 -o sum.elf
	arm-none-eabi-objdump -D sum.elf > sum.dis
clean:
	rm -rf *.o *.elf *.dis *.bin