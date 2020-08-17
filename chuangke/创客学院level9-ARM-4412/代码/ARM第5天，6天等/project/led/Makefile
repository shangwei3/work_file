led:
	arm-none-eabi-gcc -g -c -O0 led.S -o led.o
	arm-none-eabi-gcc -g -c -O0 main.c -o main.o
	arm-none-eabi-ld led.o main.o  -Ttext 0x40008000 -o led.elf
	arm-none-eabi-objcopy -O binary led.elf led.bin
clean:
	rm *.elf *.o