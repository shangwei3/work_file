.global _start

_start:
    ldr r0,=src  @// ��src�ı�ǩ��������ĵ�ַ����r0
    ldr r1,src	 @// ��src��Ӧ�ĵ�ַ�е����ݸ���r1
    adr r2,src
	@adr r3,=src  @//����������ָ��
	
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
	
	 