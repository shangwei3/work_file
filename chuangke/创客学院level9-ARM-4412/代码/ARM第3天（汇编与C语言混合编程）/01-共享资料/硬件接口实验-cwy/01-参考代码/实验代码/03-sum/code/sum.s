.text
.global _start
_start:
		mov     r3,r3
		ldr		r0,=myarray
		mov		r4,#0
loop:
		ldr		r1,[r0],#4
		add		r4,r4,r1
		cmp		r1,#0
		bne		loop
stop:
		b		stop
myarray:
		.word	0x11
		.word	0x22
		.word	0x0
		.end
