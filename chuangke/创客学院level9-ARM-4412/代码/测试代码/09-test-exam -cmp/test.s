.global _start

_start:
    ldr r0,=0xff0000ff
	mov r1,#5
	
    @mrs
    @msr cpsr_fc,r0	
    @msr cpsr_c,r0 
	msr cpsr_f,r0
	
stop: 
     b stop
	 


	 