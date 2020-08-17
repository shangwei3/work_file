.global _start
.global myadd

.global var
var:
	.word 0x12345678

_start:

	mov r0,#1
	mov r1,#2
	mov r4,#0xff
	bl myadd


/*C”Ô—‘∑√Œ ª„±‡±‰¡ø	
	mov r0,#1
	mov r1,#2
	bl mymax
	mov r4,r0
	ldr r4,var
myadd:
	add r0,r0,r1
	mov pc,lr  */

/*
ª„±‡∑√Œ C”Ô—‘¥˙¬Î
	   mov r0,#1
	   mov r1,#2
	   bl mymax
	   mov r3,r0
*/

/*
	ldr r0,add_func   
	nop
	nop
	adr r0,add_func   
	nop
	nop 
	ldr r0,=add_func 
	add_func:
		add r0,r0,r1
		mov pc,lr

	  */
main_loop:
	b main_loop
