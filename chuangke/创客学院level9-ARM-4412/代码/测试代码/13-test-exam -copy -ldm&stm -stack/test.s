.global _start

_start:
    ldr r0,=src  
    mov r1,#1
	mov r2,#2
	mov r3,#3
	mov r4,#4
	
    stmfa r0!,{r1-r4}  @//ѹջ
	ldmfa r0!,{r1-r4}  @//��ջ
   
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
	
	 