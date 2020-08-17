all:
	arm-none-eabi-gcc -O0 -c -o gcd.o gcd.S
	arm-none-eabi-ld	gcd.o -Ttext 0x20000 -o gcd.elf
	arm-none-eabi-objcopy gcd.elf -O binary -S gcd.bin
	arm-none-eabi-objdump -D gcd.elf > gcd.dis
clean:
	rm -rf *.o *.elf *.dis *.bin