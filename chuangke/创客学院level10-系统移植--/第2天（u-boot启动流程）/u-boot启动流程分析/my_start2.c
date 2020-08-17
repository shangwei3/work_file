arch/arm/lib/crt0.S 

_main:

/*
 * Set up initial C runtime environment and call board_init_f(0).
 */
	ldr	sp, =(CONFIG_SYS_INIT_SP_ADDR)

	bic	sp, sp, #7	/* 8-byte alignment for ABI compliance */
	sub	sp, #GD_SIZE	/* allocate one GD above SP */
	bic	sp, sp, #7	/* 8-byte alignment for ABI compliance */
	mov	r8, sp		/* GD is above SP */
	mov	r0, #0
	bl	board_init_f

arch/arm/lib/board.c 

typedef struct bd_info {
	unsigned int	bi_baudrate;	/* serial console baudrate */
    ulong	        bi_arch_number;	/* unique id for this board */
    ulong	        bi_boot_params;	/* where this board expects params */
	unsigned long	bi_arm_freq; /* arm frequency */
	unsigned long	bi_dsp_freq; /* dsp core frequency */
	unsigned long	bi_ddr_freq; /* ddr frequency */
    struct				/* RAM configuration */
    {
	ulong start;
	ulong size;
    }			bi_dram[CONFIG_NR_DRAM_BANKS];
} bd_t;

typedef int (init_fnc_t) (void);

gd{
	
	.mon_len = _bss_end_ofs;(u_boot大小)
}

init_fnc_t *init_sequence[] = {
	/*获取cpu id*/
	arch_cpu_init,		/* basic arch cpu dependent setup */
	/*定时器初始化*/
	timer_init,		/* initialize timer */	
	/*环境变量初始化*/
	env_init,		/* initialize environment */
	/*初始化波特率*/
	init_baudrate,		/* initialze baudrate settings */
	/*初始化串口*/
	serial_init,		/* serial communications setup */
	/*控制台一级初始化*/
	console_init_f,		/* stage 1 init of console */
	/*打印u_boot版本信息*/
	display_banner,		/* say that we are here */
	/*打印cpu的信息*/
	print_cpuinfo,		/* display cpu info (and speed) */
	/*记录内存的大小*/
	dram_init,		/* configure available RAM banks */
	NULL,
};

void board_init_f(ulong bootflag)
{
	bd_t *bd;
	init_fnc_t **init_fnc_ptr;
	gd_t *id;
	ulong addr, addr_sp;
	void *new_fdt = NULL;
	size_t fdt_size = 0;

	//#define DECLARE_GLOBAL_DATA_PTR     register volatile gd_t *gd asm ("r8")
	//DECLARE_GLOBAL_DATA_PTR;
	memset((void *)gd, 0, sizeof(gd_t));
	
	//记录uboot大小
	gd->mon_len = _bss_end_ofs;
	
	//调用初始化序列中的每一个函数
	for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
		if ((*init_fnc_ptr)() != 0) {
			hang ();
		}
	}
	
	addr = CONFIG_SYS_SDRAM_BASE + gd->ram_size;

	/* reserve TLB table */
	gd->tlb_size = 4096 * 4;
	addr -= gd->tlb_size;

	/* round down to next 64 kB limit */
	addr &= ~(0x10000 - 1);

	gd->tlb_addr = addr;

	/* round down to next 4 kB limit */
	addr &= ~(4096 - 1);

	/*
	 * reserve memory for U-Boot code, data & bss
	 * round down to next 4 kB limit
	 */
	addr -= gd->mon_len;//给uboot在高内存预留空间
	addr &= ~(4096 - 1);

	/*
	 * reserve memory for malloc() arena
	 */
	addr_sp = addr - TOTAL_MALLOC_LEN;
	
	/*
	 * (permanently) allocate a Board Info struct
	 * and a permanent copy of the "global" data
	 */
	addr_sp -= sizeof (bd_t);
	bd = (bd_t *) addr_sp;
	gd->bd = bd;

	//记录板子的ID
	gd->bd->bi_arch_number = CONFIG_MACH_TYPE; /* board id for Linux */
		
	addr_sp -= sizeof (gd_t);
	
	id = (gd_t *) addr_sp;  

	/* setup stackpointer for exeptions */
	gd->irq_sp = addr_sp;
	
	gd->bd->bi_baudrate = gd->baudrate;
	
	/* Ram ist board specific, so move it to board code ... */
	dram_init_banksize();
	display_dram_config();	/* and display it */

	gd->relocaddr = addr;
	gd->start_addr_sp = addr_sp;
	gd->reloc_off = addr - _TEXT_BASE;

	memcpy(id, (void *)gd, sizeof(gd_t));

	return 0;
}

接下来代码重定向:
		u_boot会将自己从内存的底部搬移到内存顶端，这样做的目的是为了给Linux内核运行
		预留空间,同时也可以保护自己的数据不被破坏。
		
问:u_boot第一阶段，从存储介质中将u_boot搬移到内存的时候，为什么不直接搬移到高端内存呢?
答:可以直接搬移到高端内存，如果u_boot在重定向的时候，发现自己已经在高端内存运行，则不
	 需要重定向，如果不是在高端内存运行，则重定向到高端内存。
	 
