.global _start

_start:
    ldr r0,=src  
    @ldm r0,{r1-r4}

    @ldmia r0,{r1-r4}
    @ldmib r0,{r1-r4}
	
	@ldmda r0,{r1-r4}
    @ldmdb r0,{r1-r4}
	
	ldmfa r0!,{r1-r4} @//������(��ֵַ ���ٵķ���)
    ldmfd r0,{r1-r4} @//���ݼ�(��ֵַ ���ӵķ���)
	
    ldmea r0,{r1-r4}
    ldmed r0,{r1-r4}
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
	
	 