.global _start


/*.equ GPX2CON,0x11000c40
.equ GPX2DAT,0x11000c44*/

_start:
/*
#设置GPX2_7为输出模式,GPX2CON最高4位设置为0x1
	ldr r0,=GPX2CON
	ldr r1,[r0]  @read GPX2CON ==> R1
	mov r2,#0xf
	bic r1,r1,r2,lsl#28
	mov r2,#1
	orr r1,r1,r2,lsl#28
	str r1,[r0]

while:

	#LED2灭
	ldr r0,=GPX2DAT
	ldr r1,[r0]  @read GPX2DAT ==> R1
	mov r2,#1
	bic r1,r1,r2,lsl#7
	str r1,[r0]

	#延时
	bl delay1s

	#led2亮
	ldr r0,=GPX2DAT
	ldr r1,[r0]  @read GPX2DAT ==> R1
	mov r2,#1
	bic r1,r1,r2,lsl#7
	mov r2,#1
	orr r1,r1,r2,lsl#7
	str r1,[r0]

	#延时
	bl delay1s

	b while


delay1s:
	ldr r2,=1000000000     @
2:
	subs r2,r2,#1
	bne 2b
	mov pc,lr*/

	bl main

stop:
	b stop
