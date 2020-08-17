/************************************************************************
 *
 * This is the next part if the initialization sequence: we are now
 * running from RAM and have a "normal" C environment, i. e. global
 * data can be written, BSS has been cleared, the stack size in not
 * that critical any more, etc.
 *
 ************************************************************************
 */

//id        : gd结构体首地址
//dest_addr : 重定向后uboot的起始地址
void board_init_r(gd_t *id, ulong dest_addr)
{
	ulong malloc_start;

	gd->flags |= GD_FLG_RELOC;	/* tell others: relocation done */
	bootstage_mark_name(BOOTSTAGE_ID_START_UBOOT_R, "board_init_r");

	monitor_flash_len = _end_ofs;//记录u_boot大小

	/* Enable caches */
	enable_caches();

	board_init();	/* Setup chipselects : 记录一下给Linux 传递参数的地址*/
	
	/*注册开发板上有的串口设备:将所有的串口设备用链表保存起来*/
	serial_initialize();

	/* The Malloc area is immediately below the monitor copy in DRAM */
	malloc_start = dest_addr - TOTAL_MALLOC_LEN;
	mem_malloc_init (malloc_start, TOTAL_MALLOC_LEN);
	
	power_init_board();

	puts("MMC:   ");
	mmc_initialize(gd->bd);//初始化

	/*从存储介质中读取环境变量的值*/
	env_relocate();
	
	
	//初始化标准输入输出设备
	stdio_init();	/* get the devices list going. */

	jumptable_init();

	/*将串口作为默认的控制台设备:完成输入、输出*/
	console_init_r();	/* fully init console as a device */

	 /* set up exceptions */
	interrupt_init();
	
	/* enable exceptions */
	enable_interrupts();

	puts("Net:   ");
	eth_initialize(gd->bd);

	/* main_loop() can return to retry autoboot, if so just run it again. */
	for (;;) {
		main_loop();
	}
}
