#include "reg.h"
#include "val.h"

	.globl uart_asm_init
uart_asm_init:
	/* set GPIO to enable UART */
	/* GPIO setting for UART for UART0/1/2/3 */

	ldr	r0, =0x11400000
	ldr	r1, =0x22222222
	str	r1, [r0]
	ldr	r0, =0x11400020
	ldr	r1, =0x222222
	str	r1, [r0]

	ldr	r0, =ELFIN_CLOCK_BASE
	ldr	r1, =CLK_SRC_PERIL0_VAL
	ldr	r2, =CLK_SRC_PERIL0_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =CLK_DIV_PERIL0_VAL
	ldr	r2, =CLK_DIV_PERIL0_OFFSET
	str	r1, [r0, r2]

	ldr	r0, =ELFIN_UART_CONSOLE_BASE
	ldr	r1, =0x111
	str	r1, [r0, #UFCON_OFFSET]

	mov	r1, #0x3
	str	r1, [r0, #ULCON_OFFSET]

	ldr	r1, =0x3c5
	str	r1, [r0, #UCON_OFFSET]

	ldr	r1, =UART_UBRDIV_VAL
	str	r1, [r0, #UBRDIV_OFFSET]

	ldr	r1, =UART_UDIVSLOT_VAL
	str	r1, [r0, #UDIVSLOT_OFFSET]

	mov	pc, lr

	.global puts
puts:
	ldr 	r1, =ELFIN_UART_CONSOLE_BASE
next:
	ldrb	r2, [r0], #1
	teq	r2, #0
	beq	out
	str	r2, [r1, #UTXH_OFFSET]

	mov	r2, #0x20000
wait:
	subs	r2, r2, #1
	bne	wait
	b	next
out:
	mov	pc, lr
