.global _start

_start:
    mov r0,#2
	mov r1,#5
	
	@cmp r0,r1 
	@cmn r0,#1 @// r0 - (-1)
	
	@tst r0,#2 @// 10
	
	@addeq r0,r0,r1 
	@subne r0,r0,r1 
     



	@cmp r0,r1
	
	@addcs r0,r1
	@subcc r0,r1
	 
	 
	 mrs r2,cpsr     @//��ֵ 
	 
	 mov r0,#1       @//�޸�
	 bic r2,r0,lsl #7
	 
	                 @//д
	 
	 msr cpsr,r2  
	 

loop:
    mov r0,r1
    
	sub r1,#1 @// for -- (i--)
	cmp r1,#0 @//�����ж� i < 0
	bne loop
    beq stop
stop: 
     b stop
	 