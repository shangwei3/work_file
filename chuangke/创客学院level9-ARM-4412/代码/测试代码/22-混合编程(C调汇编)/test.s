.global _start

.global src


_start:
   ldr r0,=src
   mov r0,#1
   mov r1,#2
   bl myadd
stop: 
     b stop
	 
src:                 @//�൱�ڶ���һ������
    .word 0x4578     @// 0111 1000
    .word 0x6789     @// 1000 1001
    .word 0x34567890
    .word 0x45678901

dest:
    .word 0xffffffff
	.word 0xffffffff
	.word 0xffffffff
	.word 0xffffffff



		