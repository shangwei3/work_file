.global _start


@//1 + 5 * 2^5 - 32 / 2^4 + 8*5
@//
_start:
    mov r0,#1
	mov r1,#5
	
	and r2,r0,r1
	orr r3,r0,r1
	eor r4,r0,r1
	bic r4,r0,r1 @// (~r1) & r0
                 @// 0000 0101 --> ffff  ... 1010
                 @// 1010 & 1 == 0  				 
	
	
	ldr r5,=0xffff0000
	
	mov r6,#1

	bic r7,r5,r6,lsl #30
	
	mov r6,#5
	
	bic r7,r5,r6,lsl #28
stop: 
     b stop
	 