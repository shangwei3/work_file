.global _start

_start:
    ldr r0,=src  
    mov r1,#1
	mov r2,#2
	mov r3,#3
	mov r4,#4
	
    stmfa r0!,{r1-r4}  @//压栈
	ldmfa r0!,{r1-r4}  @//出栈
   
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
	
	 