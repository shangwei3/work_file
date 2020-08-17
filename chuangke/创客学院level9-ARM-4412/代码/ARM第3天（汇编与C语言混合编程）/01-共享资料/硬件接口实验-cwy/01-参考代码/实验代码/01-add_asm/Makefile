all:
	arm-none-eabi-gcc -O0 -g -c -o add_asm.o add_asm.s
	arm-none-eabi-ld add_asm.o -Ttext 0x34000 -o add_asm.elf
	arm-none-eabi-objcopy -O binary -S add_asm.elf add_asm.bin 
	arm-none-eabi-objdump -D add_asm.elf > add_asm.dis
clean:
	rm rf *.o *.elf *.dis *.bin	