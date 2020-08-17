.global _start

.macro memcpy src_addr,dest_addr,size

@//实现代码
	sub \src_addr,#4
	sub \dest_addr,#4
loop:
    ldr r4,[\src_addr,#4]! 
    str r4,[\dest_addr,#4]!  
	
	subs \size,#1
	beq stop
	bne loop

.endm

_start:
    ldr r0,=src
    ldr r1,=dest	
	mov r2,#4
	memcpy r0,r1,r2	


stop: 
     b stop
	 
src:   @//相当于定义一个数组
    .word 0x4578   @// 0111 1000
    .word 0x6789   @// 1000 1001
    .word 0x34567890
    .word 0x45678901

dest:
    .word 0xffffffff
	.word 0xffffffff
	.word 0xffffffff
	.word 0xffffffff
	
	 