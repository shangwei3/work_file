led:led.s
	arm-none-eabi-gcc led.s -c -g -O0 -o led.o
	arm-none-eabi-ld led.o -Ttext 0x40008000 -o led.elf
	arm-none-eabi-objcopy -O binary led.elf led.bin