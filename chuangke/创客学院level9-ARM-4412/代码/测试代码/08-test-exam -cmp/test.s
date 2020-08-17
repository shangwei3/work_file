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
	 
	 
	 mrs r2,cpsr     @//读值 
	 
	 mov r0,#1       @//修改
	 bic r2,r0,lsl #7
	 
	                 @//写
	 
	 msr cpsr,r2  
	 

loop:
    mov r0,r1
    
	sub r1,#1 @// for -- (i--)
	cmp r1,#0 @//条件判断 i < 0
	bne loop
    beq stop
stop: 
     b stop
	 