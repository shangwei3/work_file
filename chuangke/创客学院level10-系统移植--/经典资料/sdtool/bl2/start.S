#include "reg.h"
#include "val.h"

	.global _start
_start:
	/* stack init */
	ldr	r0, stacktop
	mov	sp, r0

	/* */
	bl	ps_hold_on

	/* cache, MMU, TLBs init */
	bl	cpu_init_cp15

	/* system clock init */
	bl	system_clock_init

	/* DDR3 memory controller init */
	bl	mem_ctrl_init

	/* SROM memory controller init */
	bl	srom_ctrl_init

	/* uart init */
	bl	uart_asm_init

	/* get boot mode */
	ldr	r0, =ELFIN_POWER_BASE
	ldr	r1, [r0,#OMR_OFFSET]
	bic	r2, r1, #0xffffffc1

sd_mmc_boot:
	/* SD/MMC BOOT */
	cmp	r2, #0x4
	bne	emmc_4_4_boot
	bl	mmc_ch2_div_set
	mov	r0, #0
	b	boot

emmc_4_4_boot:
	/* eMMC 4.4 BOOT */
	cmp	r2, #0x8
	cmpne	r2, #0x28
	bne	stop
	bl	mmc_ch4_div_set
	mov	r0, #1
	b	boot

stop:
	b	stop

/*************************************************************************
 *
 * ps_hold_on
 *
 * . 
 *
 *************************************************************************/
ps_hold_on:
	ldr	r0, =0x11000C00

	ldr	r1, [r0]
	bic	r1, #0xF00
	orr	r1, #0x100
	str	r1, [r0]

	ldr	r1, [r0, #4]
	orr	r1, r1, #4
	str	r1, [r0, #4]

	ldr	r0, =0x1002330C
	ldr	r1, [r0]
	orr	r1, r1, #0x300
	str	r1, [r0]
	mov	pc, lr

/*************************************************************************
 *
 * cpu_init_cp15
 *
 * Setup CP15 registers (cache, MMU, TLBs). 
 *
 *************************************************************************/
cpu_init_cp15:
	/*
	 * Invalidate L1 I/D
	 */
	mov	r0, #0			@ set up for MCR
	mcr	p15, 0, r0, c8, c7, 0	@ invalidate TLBs
	mcr	p15, 0, r0, c7, c5, 0	@ invalidate icache
	mcr	p15, 0, r0, c7, c5, 6	@ invalidate BP array
	mcr     p15, 0, r0, c7, c10, 4	@ DSB
	mcr     p15, 0, r0, c7, c5, 4	@ ISB

	/*
	 * disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002000	@ clear bits 13 (--V-)
	bic	r0, r0, #0x00000007	@ clear bits 2:0 (-CAM)
	orr	r0, r0, #0x00000002	@ set bit 1 (--A-) Align
	orr	r0, r0, #0x00000800	@ set bit 11 (Z---) BTB
	orr	r0, r0, #0x00001000	@ set bit 12 (I) I-cache, turn on I-cache
	mcr	p15, 0, r0, c1, c0, 0
	mov	pc, lr

/*************************************************************************
 *
 * mmc_ch2_div_set
 *
 * Setup mmc2 clock divider. 
 *
 *************************************************************************/
mmc_ch2_div_set:
	ldr	r0, =ELFIN_CLOCK_BASE
	ldr	r2, =CLK_DIV_FSYS2_OFFSET
	ldr	r1, [r0, r2]
	orr	r1, r1, #0xF
	orr	r1, r1, #0xF00
	str	r1, [r0, r2]
	mov	pc, lr

/*************************************************************************
 *
 * mmc_ch4_div_set
 *
 * Setup emmc clock divider. 
 *
 *************************************************************************/
mmc_ch4_div_set:
	ldr	r0, =ELFIN_CLOCK_BASE
	ldr	r2, =CLK_DIV_FSYS3_OFFSET
	ldr	r1, [r0, r2]
	bic	r1, r1, #(0xFF << 8)
	bic	r1, r1, #(0xF)
	orr	r1, r1, #(0x9 << 8)
	orr	r1, r1, #0x1
	str	r1, [r0, r2]
	mov	pc, lr

	.globl tzsw_init
tzsw_init:
	mov	ip, lr
	mov	lr, pc
	ldr	pc, =0x02028000
	mov	lr, ip
	mov	pc, lr

	.align	4
stacktop:
	.word	stack + 4096

	.data
stack:
	.space  4096
