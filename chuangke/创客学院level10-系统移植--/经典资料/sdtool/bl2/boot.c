#define U_BOOT_START_BLOCK	(49)

#define BL1_BLOCKS		(8 * 1024 / 512)
#define BL2_BLOCKS		(16 * 1024 / 512)
#define U_BOOT_BLOCKS		((512 - 16 - 8) * 1024 / 512 - 1)

#define BL1_SIZE		(BL1_BLOCKS * 512)
#define BL2_SIZE		(BL2_BLOCKS * 512)
#define U_BOOT_SIZE		(U_BOOT_BLOCKS * 512)

#define U_BOOT_LOAD_ADDR	(0x43E00000)
#define SD_COPY_FUNC_ENTRY	(0x02020030)
#define EMMC_COPY_FUNC_ENTRY	(0x02020044)
#define END_BOOT_OP_ADDR	(0x02020048)

extern void puts(char *str);
typedef int (*sd_copy_func_t)(unsigned int start, unsigned int blocks, void *dst);
typedef int (*emmc_copy_func_t)(unsigned int start, void *dst);

void boot(int isemmc)
{
	int ret;

	if (isemmc) {
		emmc_copy_func_t copy = (emmc_copy_func_t)(*(unsigned int *)EMMC_COPY_FUNC_ENTRY);

		puts("Copying from eMMC...\r\n");
		ret = copy(U_BOOT_BLOCKS, (void *)(U_BOOT_LOAD_ADDR));
	} else {
		sd_copy_func_t copy = (sd_copy_func_t)(*(unsigned int *)SD_COPY_FUNC_ENTRY);

		puts("Copying from SD...\r\n");
		ret = copy(U_BOOT_START_BLOCK, U_BOOT_BLOCKS, (void *)U_BOOT_LOAD_ADDR);
	}

	if (ret == 1) {
		if (isemmc)
			((void (*)(void))(*(unsigned int*)(END_BOOT_OP_ADDR)))();
		
		/* jump to u-boot */
		puts("\r\nBootting the U-Boot...\r\n");
		((void (*)(void))(U_BOOT_LOAD_ADDR))();
	} else {
		puts("\r\nCopy U-Boot to SDRAM failed\r\n");
		while (1);
	}
}
