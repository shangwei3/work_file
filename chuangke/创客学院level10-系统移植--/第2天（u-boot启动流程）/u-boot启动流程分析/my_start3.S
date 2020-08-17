u_boot重定向 

/*
 * Set up intermediate environment (new sp and gd) and call
 * relocate_code(addr_sp, gd, addr_moni). Trick here is that
 * we'll return 'here' but relocated.
 */
	ldr	sp, [r8, #GD_START_ADDR_SP]	/* r8 = gd->start_addr_sp */
	bic	sp, sp, #7	/* 8-byte alignment for ABI compliance */
	ldr	r8, [r8, #GD_BD]		/* r8 = gd->bd */
	sub	r8, r8, #GD_SIZE		/* new GD is below bd */
	
	/*ldr lr,=here*/
	adr	lr, here /*lr = pc +/- offset */
	
	ldr	r0, [r8, #GD_RELOC_OFF]
	add	lr, lr, r0
	ldr	r0, [r8, #GD_START_ADDR_SP]	/* r0 = gd->start_addr_sp */
	mov	r1, r8				/* r1 = gd */
	ldr	r2, [r8, #GD_RELOCADDR]		/* r2 = gd->relocaddr */
	
	b	relocate_code

here:
/* Set up final (full) environment */

	bl	c_runtime_cpu_setup	/* we still call old routine here */

	ldr	r0, =__bss_start	/* this is auto-relocated! */
	ldr	r1, =__bss_end__	/* this is auto-relocated! */
	
	/*
     *asm -> C
	 *
	 *[1]sp设置 
	 *[2]bss段的清0 
	 *
	 */
	mov	r2, #0x00000000		/* prepare zero to clear BSS */

clbss_l:
  cmp	    r0, r1			/* while not at end of BSS */
	strlo	r2, [r0]		/* clear 32-bit BSS word */
	addlo	r0, r0, #4		/* move to next */
	blo	clbss_l

	/* call board_init_r(gd_t *id, ulong dest_addr) */
	mov	r0, r8			/* gd_t */
	ldr	r1, [r8, #GD_RELOCADDR]	/* dest_addr */
	
	/* call board_init_r */
	ldr	pc, =board_init_r	/* this is auto-relocated! */

---------------------------------------------------------------------
arch/arm/cpu/armv7/start.S 
/*
 * void relocate_code (addr_sp, gd, addr_moni)
 *
 * This "function" does not return, instead it continues in RAM
 * after relocating the monitor code.
 *
 */
relocate_code:

	mov	r4, r0	/* save addr_sp */
	mov	r5, r1	/* save addr of gd */
	mov	r6, r2	/* save addr of destination */

	获取_start标签所在内存的地址值:r0 -> 0x43e00000
	adr	r0, _start
	cmp	r0, r6

	moveq	r9, #0		/* no relocation. relocation offset(r9) = 0 */
	beq	relocate_done		/* skip relocation */ 

	mov	r1, r6			/* r1 <- scratch for copy_loop */
	
	r3 = .txt + .rodata + .data + .u-boot-list
	ldr	r3, _image_copy_end_ofs
	
	add	r2, r0, r3		/* r2 <- source end address	    */

copy_loop:
	ldmia	r0!, {r9-r10}		/* copy from source address [r0]    */
	stmia	r1!, {r9-r10}		/* copy to   target address [r1]    */
	cmp	r0, r2			/* until source end address [r2]    */
	blo	copy_loop (lo:无符号小于)

	/*
	 * fix .rel.dyn relocations
	 */
	ldr	r0, _TEXT_BASE		/* r0 <- Text base */
	sub	r9, r6, r0		/* r9 <- relocation offset */


	/*
	 *r10 : .dynsym-offset 
	 *r0  : _start 
	 *r10 = r0 + r10 = .dynsym_start 
	 */
	ldr	r10, _dynsym_start_ofs	/* r10 <- sym table ofs */
	add	r10, r10, r0		/* r10 <- sym table in FLASH */
	
	/*
	 *r2 : .rel.dyn-start-offset 
	 *r0 : _start 
	 *r2 : r2 + r0 = .rel.dyn_start 
	 */
	ldr	r2, _rel_dyn_start_ofs	/* r2 <- rel dyn start ofs */
	add	r2, r2, r0		/* r2 <- rel dyn start in FLASH */	
	
	/*
	 *r3 :.rel.dyn-end-offset
	 *r0:_start 
	 *r3 = r3 + r0 = .rel.dyn_end 
	 */
	ldr	r3, _rel_dyn_end_ofs	/* r3 <- rel dyn end ofs */
	add	r3, r3, r0		/* r3 <- rel dyn end in FLASH */


fixloop:
	/*r0:0x54*/
	ldr	r0, [r2]		/* r0 <- location to fix up, IN FLASH! */
	
	/*r0 = 0x54 + offset*/
	add	r0, r0, r9		/* r0 <- location to fix up in RAM */
	
	ldr	r1, [r2, #4]
	and	r7, r1, #0xff
	cmp	r7, #23			/* relative fixup? */
	beq	fixrel


fixrel:
	/* relative fix: increase location by offset */
	ldr	r1, [r0]
	add	r1, r1, r9
fixnext:
	str	r1, [r0]
	add	r2, r2, #8		/* each rel.dyn entry is 8 bytes */
	cmp	r2, r3
	blo	fixloop

relocate_done:
	bx	lr

_rel_dyn_start_ofs:
	.word __rel_dyn_start - _start
_rel_dyn_end_ofs:
	.word __rel_dyn_end - _start
_dynsym_start_ofs:
	.word __dynsym_start - _start

----------------------------------------------------------------------------------
arch/arm/cpu/armv7/start.S 

c_runtime_cpu_setup:
/*
 * If I-cache is enabled invalidate it
 */
	mcr	p15, 0, r0, c7, c5, 0	@ invalidate icache
	mcr     p15, 0, r0, c7, c10, 4	@ DSB
	mcr     p15, 0, r0, c7, c5, 4	@ ISB

/*
 * Move vector table
 */
	/* Set vector address in CP15 VBAR register */
	ldr     r0, =_start
	add     r0, r0, r9
	mcr     p15, 0, r0, c12, c0, 0  @Set VBAR

	bx	lr

