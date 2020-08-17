.global _start

_start:
    ldr r0,=src  
	ldr r1,=dest
	ldmia   r0,{r2-r5}  
    stmia   r1,{r2-r5}  
   
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
	
	 