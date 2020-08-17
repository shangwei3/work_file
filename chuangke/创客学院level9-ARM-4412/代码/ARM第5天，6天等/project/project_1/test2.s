.global _start

_start:
	mrs r0,cpsr
	mov r1,#1
	orr r0,r0,r1 ,lsl #7 @//中国南海
	msr cpsr,r0;

	mrs r0,cpsr
	mov r1,#1
	bic r0,r0,r1 ,lsl #7 @//中国南海
	msr cpsr,r0;

	mov r1,#0xff
	str r1,[r0],#8
	str r1,[r0,#8]!
stop:
	b stop


