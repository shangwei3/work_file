.global _start

_start:
     mov r0,#1
	 .align 4   @// ָ��16�ֽڶ���
	            @//ֻ��Ӱ�쵽һ��ָ��

2:@//��ǰ front 
     mov r1,#2
	 mov r2,#3
	 
	 b 2f
	 b 2b
2: @// back --���(����ִ�еķ���)
    add r1,r2
	
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
	
	 