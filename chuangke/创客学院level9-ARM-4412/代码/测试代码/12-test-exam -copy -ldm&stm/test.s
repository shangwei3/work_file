.global _start

_start:
    ldr r0,=src  
    @ldm r0,{r1-r4}

    @ldmia r0,{r1-r4}
    @ldmib r0,{r1-r4}
	
	@ldmda r0,{r1-r4}
    @ldmdb r0,{r1-r4}
	
	ldmfa r0!,{r1-r4} @//满递增(地址值 减少的方向)
    ldmfd r0,{r1-r4} @//满递减(地址值 增加的方向)
	
    ldmea r0,{r1-r4}
    ldmed r0,{r1-r4}
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
	
	 