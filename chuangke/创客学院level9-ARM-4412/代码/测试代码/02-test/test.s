.global _start

_start:
    mov r0,#255
    mov r1,r0,lsl #1  @//�߼�����
    mov r1,r0,lsr #1  @//�߼�����
	
	mov r1,r0,asr #1  @//��������
    mov r1,r0,ror #1  @//ѭ������
	
	movs r1,r0,asr #1  @//��������
    movs r1,r0,ror #1  @//ѭ������
	
	
	
	mov r0,#-1
    mov r1,r0,lsl #1  @//�߼�����
    mov r1,r0,lsr #1  @//�߼�����
	
	mov r1,r0,asr #1  @//��������
    mov r1,r0,ror #1  @//ѭ������
	
	movs r1,r0,asr #1  @//��������
    movs r1,r0,ror #1  @//ѭ������
	
stop: 
	 b stop
	 