.global _start

_start:

@//������

    mov r0,#1         @//(1)������2 Ϊ ������
	mov r1,r0         @//(2)������2 Ϊ �Ĵ���
	mov r2,r1,lsl #1  @//(3)������2 Ϊ �Ĵ�����λ��ʽ
	mvn r3,#1         @// ȡ����ֵ��r3
	
	mov r0,#0xFFFFFFFE
    @mvn r0,#1	

stop: 
	 b stop
	 