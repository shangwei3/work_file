.global _start

_start:
    mov r0,#255
    mov r1,r0,lsl #1  @//¬ﬂº≠◊Û“∆
    mov r1,r0,lsr #1  @//¬ﬂº≠”““∆
	
	mov r1,r0,asr #1  @//À„ ı”““∆
    mov r1,r0,ror #1  @//—≠ª∑”““∆
	
	movs r1,r0,asr #1  @//À„ ı”““∆
    movs r1,r0,ror #1  @//—≠ª∑”““∆
	
	
	
	mov r0,#-1
    mov r1,r0,lsl #1  @//¬ﬂº≠◊Û“∆
    mov r1,r0,lsr #1  @//¬ﬂº≠”““∆
	
	mov r1,r0,asr #1  @//À„ ı”““∆
    mov r1,r0,ror #1  @//—≠ª∑”““∆
	
	movs r1,r0,asr #1  @//À„ ı”““∆
    movs r1,r0,ror #1  @//—≠ª∑”““∆
	
stop: 
	 b stop
	 