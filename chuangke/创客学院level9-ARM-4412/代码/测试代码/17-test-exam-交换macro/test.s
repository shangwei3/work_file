.global _start


.macro xchg regs,regd

@//ʵ�ֵĴ���

     mov r0,\regs
	 mov \regs,\regd
	 mov \regd,r0

.endm



_start:
    mov r2,#0x1  
	mov r1,#0x2
    xchg r2,r1   

stop: 
     b stop
	 
src:   @//�൱�ڶ���һ������
    .word 0x4578   @// 0111 1000
    .word 0x6789   @// 1000 1001
    .word 0x34567890
    .word 0x45678901

dest:
    .word 0xffffffff
	.word 0xffffffff
	.word 0xffffffff
	.word 0xffffffff
	
	 