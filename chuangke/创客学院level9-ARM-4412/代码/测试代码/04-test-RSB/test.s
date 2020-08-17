.global _start

_start:

    mov r0,#1
    mov r1,#2
	mov r2,#3
	
	subs r3,r0,r1 @// r3 = r0-r1
	rsb  r4,r0,r1 @// r4 = r1-r0 ·´Ïò¼õ·¨
	
	mul r5,r0,r1  @// r5 = r0 * r1 

	mla r6,r0,r1,r2 @// r6 = r0*r1 + r2 = 1*2 + 3 = 5
	
stop: 
	 b stop
	 