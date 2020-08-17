.global _start

_start:
		mov r0,#1
		mov r1,#2
		add r3,r0,r1
		mvn r4,#1  

stop:
	b stop
