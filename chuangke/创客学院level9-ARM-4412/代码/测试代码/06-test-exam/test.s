.global _start


@//1 + 5 * 2^5 - 32 / 2^4 + 8*5
@//
_start:
    mov r0,#1
	mov r1,#5
	
	add r0,r1,lsl #5 @// 1 + 5 * 2^5	
	
	mov r2,#32	
	sub r0,r2,lsr #4 @//1 + 5 * 2^5 - 32/2^4
   
    mov r3,#8
	
	mla r0,r3,r1,r0 
	
	
     
	 
	 

stop: 
     b stop
	 