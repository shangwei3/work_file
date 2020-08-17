#define DM9000_Tacs	(0x1)	/* 0clk         address set-up		*/
#define DM9000_Tcos	(0x1)	/* 4clk         chip selection set-up	*/
#define DM9000_Tacc	(0x5)	/* 14clk        access cycle		*/
#define DM9000_Tcoh	(0x1)	/* 1clk         chip selection hold	*/
#define DM9000_Tah	(0xC)	/* 4clk         address holding time	*/
#define DM9000_Tacp	(0x9)	/* 6clk         page mode access cycle	*/
#define DM9000_PMC	(0x1)	/* normal(1data)page mode configuration	*/
#define SROM_BC1_VAL	((DM9000_Tacs << 28) \
			| (DM9000_Tcos << 24) \
			| (DM9000_Tacc << 16) \
			| (DM9000_Tcoh << 12) \
			| (DM9000_Tah << 8) \
			| (DM9000_Tacp << 4) \
			| (DM9000_PMC))

	.global srom_ctrl_init
srom_ctrl_init:
	/* CSn, WE, OE*/
	ldr	r0, =0x11000120
	ldr	r1, =0x00222222
	str	r1, [r0]

	/* BE, WAIT, RD */
	ldr	r0, =0x11000140
	ldr	r1, =0x00002222
	str	r1, [r0]

	/* ADDR[0-7], pull up */
	ldr	r0, =0x11000180
	ldr	r1, =0x22222222
	str	r1, [r0]
	ldr	r0, =0x11000188
	ldr	r1, =0x0000FFFF
	str	r1, [r0]

	/* DATA[0-7], pull up */
	ldr	r0, =0x110001C0
	ldr	r1, =0x22222222
	str	r1, [r0]
	ldr	r0, =0x110001C8
	ldr	r1, =0x0000FFFF
	str	r1, [r0]

	/* DATA[8-15], pull up */
	ldr	r0, =0x110001E0
	ldr	r1, =0x22222222
	str	r1, [r0]
	ldr	r0, =0x110001E8
	ldr	r1, =0x0000FFFF
	str	r1, [r0]

	/* SROM BANK 1 */
	ldr	r0, =0x12570000
	ldr	r1, [r0]
	orr	r1, r1, #0xF0
	str	r1, [r0]

	ldr	r0, =0x12570008
	ldr	r1, =SROM_BC1_VAL
	str	r1, [r0]

	mov	pc, lr
