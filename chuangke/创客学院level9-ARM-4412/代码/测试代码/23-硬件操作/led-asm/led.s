.global _start

.equ GPX2CON , 0x11000C40 @//
.equ GPX2DAT , 0x11000C44 @//
_start:
     @//���ö�Ӧ������Ϊ���
	 ldr r0,=GPX2CON
	 ldr r1,[r0] @//��
	 
	 mov r2,#0xf @//�޸�
     bic r1,r1,r2,lsl #28
	 mov r3,#0x1
     orr r1,r1,r3,lsl #28
                 @//д
	 str r1,[r0]
    
    
while:	
  	 @//���ָ���ĵ�ƽ
     @//��
	 ldr r0,=GPX2DAT
	 ldr r1,[r0]
	 mov r2,#0x1 @//�޸�
     bic r1,r1,r2,lsl #7
	 @mov r3,#0x1
     @orr r1,r1,r3,lsl #28
                 @//д
	 str r1,[r0]
	 
	 @//��ʱ
	 bl delays
	 @//��
	 ldr r0,=GPX2DAT
	 ldr r1,[r0]
	 mov r2,#0x1 @//�޸�
     bic r1,r1,r2,lsl #7
	 mov r3,#0x1
     orr r1,r1,r3,lsl #7
                 @//д
	 str r1,[r0]

     @//��ʱ
      bl delays
      b while     


stop:
     b stop
	 
delays:
    ldr r4,=100000000
loop:	
	subs r4,#1
	bne loop 
	mov pc,lr @//�ص�֮ǰ����תλ��
	