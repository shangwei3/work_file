.global _start



_start:
     ldr r0,=data
	 @mov r1,[r0] @//(1).������(2).�Ĵ���(3).�Ĵ�����λ
	 ldr r1,[r0]
	 
	 
	 @//���ú���
	 @//1.׼������
	 mov r0,#1
	 mov r1,#2
	 
	 @//2.���ú���
	 bl max
	 
	 @//3.���շ���ֵ
	 mov r4,r0
	
@stop: 
@     b stop
	 
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
		