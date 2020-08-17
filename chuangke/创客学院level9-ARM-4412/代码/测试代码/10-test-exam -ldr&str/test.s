.global _start

_start:
    ldr r0,=src
    ldr r1,=dest	
	mov r2,#4
		
	sub r0,#4
	sub r1,#4
loop:
    ldr r4,[r0,#4]! @//获取数据更新基地址寄存器
    str r4,[r1,#4]!  
	
	subs r2,#1
	beq stop
	bne loop
	
	
stop: 
     b stop
	 
src:                 @//相当于定义一个数组
    .word 0x4578     @// 0111 1000
    .word 0x6789     @// 1000 1001
    .word 0x34567890
    .word 0x45678901

dest:
    .word 0xffffffff
	.word 0xffffffff
	.word 0xffffffff
	.word 0xffffffff
	
	
asm volatile ("asm code":output:input:changed );

	 