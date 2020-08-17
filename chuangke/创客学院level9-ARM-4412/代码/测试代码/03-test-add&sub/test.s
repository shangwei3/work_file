.global _start

_start:

    mov r0,#1
    mov r1,#2
	
	adds r2,r0,r1
	subs r3,r0,r1
	
	@adds r0,r0,r1
	adds r0,r1
	
	

	
stop: 
	 b stop
	 