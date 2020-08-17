.text
.global _start
_start:
		mov		r0,#9
		mov		r1,#15
loop:
		cmp		r0,r1
		sublt	r1,r1,r0
		subgt	r0,r0,r1
		bne		loop
stop:
		b		stop
