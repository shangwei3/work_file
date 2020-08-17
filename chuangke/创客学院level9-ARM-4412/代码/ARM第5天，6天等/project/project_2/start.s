.global _start

.equ GPX2CON , 0x11000c40
.equ GPX2DAT , 0x11000c44
_start:
	

	@mov r0,#1
	@orr r1,r1,r0,lsl #7

main:
	ldr r0,=GPX2CON
	ldr r1,[r0]
	mov r2,#0xf
	bic r1,r1,r2,lsl #28
	mov r2,#1
	orr r1,r1,r2,lsl #28
	str r1,[r0]

	ldr r0,=GPX2DAT
	ldr r1,[r0]
	mov r2,#0xff
	bic r1,r1,r2
	mov r2,#0x1
	orr r1,r1,r2,lsl #7
	str r1,[r0]

	ldr r2,=0x10000000
delay1:
	subs r2,r2,#1
	@sub r2,r2,#0x10000000
	bne	delay1 

	ldr r0,=GPX2DAT
	ldr r1,[r0]
	mov r2,#0xff
	bic r1,r1,r2
	str r1,[r0]

	ldr r2,=0x10000000
delay2:
	subs r2,r2,#1
	@sub r2,r2,#0x1000000
	bne	delay2

	b main

stop:
	b stop


