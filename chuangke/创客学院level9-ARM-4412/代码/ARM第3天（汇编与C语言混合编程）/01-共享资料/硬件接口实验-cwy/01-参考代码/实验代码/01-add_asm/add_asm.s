
.globl	_start
_start:
	mov r0,#9
	mov r1,#15
	add r1,r1,r0
stop:
	b stop
