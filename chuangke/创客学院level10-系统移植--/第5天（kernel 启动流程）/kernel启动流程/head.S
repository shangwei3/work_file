Linux 内核解压后启动流程:

ENTRY(stext)
	setmode	PSR_F_BIT | PSR_I_BIT | SVC_MODE, r9 @ ensure svc mode
						@ and irqs disabled
分析如下:
设置ARM核为SVC模式，并且禁止中断 

----------------------------------------------------------------
	mrc	p15, 0, r9, c0, c0		@ get processor id

分析如下: 
读取ARM协处理器cp15的c0寄存器获得CPU ID,存放在r9

CPU ID格式如下:
---------------------------------------------------------------------
厂商编号  | 产品子编号 | ARM 体系版本号 | 产品主编号 |处理器版本号  |
---------------------------------------------------------------------

查手册: cotex_a8_r3p2_trm.pdf 3.2.2节可以获得如下信息 
厂商编号      [31:24] : 0x41 表示ARM公司  
产品子编号    [23:20] : 0x3  
ARM体系版本号 [19:16] : 0x1->ARMv4, ...,0xF ->ARMv7 
产品主编号    [15: 4] : 0xc08->CortexA8 
处理器版本号  [ 3: 0] : 0x2 

所以 r9 : 0x413fc082

-----------------------------------------------------------------

	bl	__lookup_processor_type		@ r5=procinfo r9=cpuid

分析如下: 

/*
 * Read processor ID register (CP#15, CR0), and look up in the linker-built
 * supported processor list.  Note that we can't use the absolute addresses
 * for the __proc_info lists since we aren't running with the MMU on
 * (and therefore, we are not in the correct address space).  We have to
 * calculate the offset.
 *
 *	r9 = cpuid
 * Returns:
 *	r3, r4, r6 corrupted
 *	r5 = proc_info pointer in physical address space
 *	r9 = cpuid (preserved)
 */
__lookup_processor_type:

分析如下: 
请仔细阅读上面的注释部分。  
我们可以知道在32bit的机器上,Linux 内核的虚拟用户空间和内核空间分配如下:

|-----------| 0xffff,ffff  [4G]
| 内核空间  |     
|-----------| 0xc000,0000  [3G]  
|           |
|           |
| 用户空间  |   
|           |   
|           |
|-----------| 0x0000,0000  [0]

可以知道的是，Linux 内核最终在运行的时候，使用的是虚拟地址,当然目前还没有使用
虚拟地址，因为到目前为止还没有开启MMU。但是在生成vmlinux的时候，链接的链接脚本
arch/arm/kernel/vmlinux.lds 中指定的是开启MMU后的虚拟地址，起始地址为0xc0008000。 

思考:为什么要偏移0x8000[32k]?
答案:这段空间用来存放页表[虚拟地址与物理地址之间的对应关系]



	adr	r3, 3f
	ldmia	r3, {r5 - r7}

分析如下:
第一条指令，获取局部标号3所在当前内存的地址。局部标号3定义如下:

/*
 * Look in <asm/procinfo.h> and arch/arm/kernel/arch.[ch] for
 * more information about the __proc_info and __arch_info structures.
 */
	.align	2
3:	.long	__proc_info_begin  
	.long	__proc_info_end   
4:	.long	.                  

所以我们可以确定:
r3 -> 局部标号3当前所在的内存的物理地址
r5 -> __proc_info_begin  [虚拟地址]   
r6 -> __proc_info_end    [虚拟地址]
r7 -> 局部标号4的链接地址[虚拟地址:0xcxxx,xxxx]

查看链接脚本arch/arm/kernel/vmlinux.lds，可以看到如下内容:

  __proc_info_begin = .;
   *(.proc.info.init)
  __proc_info_end = .;

明白了__proc_info_begin和__proc_info_end分别为.proc.info.init段的起始
地址和结束地址。 

这个段是用来干嘛的呢? 

Linux 内核所支持的每一种CPU类型都由结构体proc_info_list来描述,然后将这些
支持的CPU类型的信息编译在.proc.info.init段中存放。
该结构体在文件arch/arm/include/asm/procinfo.h中定义:

/*
 * Note!  struct processor is always defined if we're
 * using MULTI_CPU, otherwise this entry is unused,
 * but still exists.
 *
 * NOTE! The following structure is defined by assembly
 * language, NOT C code.  For more information, check:
 *  arch/arm/mm/proc-*.S and arch/arm/kernel/head.S
 */
struct proc_info_list {
	unsigned int		cpu_val;
	unsigned int		cpu_mask;
	unsigned long		__cpu_mm_mmu_flags;	/* used by head.S */
	unsigned long		__cpu_io_mmu_flags;	/* used by head.S */
	unsigned long		__cpu_flush;		/* used by head.S */
	const char		*arch_name;
	const char		*elf_name;
	unsigned int		elf_hwcap;
	const char		*cpu_name;
	struct processor	*proc;
	struct cpu_tlb_fns	*tlb;
	struct cpu_user_fns	*user;
	struct cpu_cache_fns	*cache;
};

对于cortexA8而言，这个结构体的填充在 arch/arm/mm/proc-v7.S中完成， 这个 
文件的327行，有如下定义:

	.section ".proc.info.init", #alloc, #execinstr
	/*
	 * Match any ARMv7 processor core.
	 */
	.type	__v7_proc_info, #object
__v7_proc_info:
	.long	0x000f0000		@ Required ID value
	.long	0x000f0000		@ Mask for ID
	.long   PMD_TYPE_SECT | PMD_SECT_AP_WRITE |PMD_SECT_AP_READ | PMD_FLAGS
	.long   PMD_TYPE_SECT | PMD_SECT_XN | PMD_SECT_AP_WRITE | PMD_SECT_AP_READ
 	 b	__v7_setup
	.long	cpu_arch_name
	.long	cpu_elf_name
	.long	HWCAP_SWP|HWCAP_HALF|HWCAP_THUMB|HWCAP_FAST_MULT|HWCAP_EDSP
	.long	cpu_v7_name
	.long	v7_processor_functions
	.long	v7wbi_tlb_fns
	.long	v6_user_fns
	.long	v7_cache_fns
	.size	__v7_proc_info, . - __v7_proc_info

需要注意的是，所有支持的CPU类型对应的初始化proc_info_list结构体都存放在
__proc_info_begin和__proc_info_end之间 
	


	add	r3, r3, #8
	sub	r3, r3, r7			@ get offset between virt&phys
	
分析如下: 
通过上面的分析，我们知道r3中存放的是局部标号3所在内存的物理地址，现在将它加上8，
即跳了两条指令，也就是说r3现在保存的是局部标号4所在内存的物理地址。

通过上面的分析，我们知道r7中保存的是局部标号4的链接地址,即虚拟地址，现在进行 
r3 = r3 - r7 => r3 = 4_physaddr - 4_virtaddr 即r3 中保存的是物理地址与虚拟地址
之间的差值。
	
	
	add	r5, r5, r3			@ convert virt addresses to
	add	r6, r6, r3			@ physical address space

分析如下:
通过前面的分析，我们知道r5,r6中保存的是__proc_info_begin和__proc_info_end的虚拟地址 
,现在加上r3 [物理地址与虚拟地址之间的差值]，达到的效果如下
r5 -> __proc_info_begin  [所在内存的物理地址]   
r6 -> __proc_info_end    [所在内存的物理地址]


1:	ldmia	r5, {r3, r4}			@ value, mask
分析如下:
r3 ->cpu_val :0x000f0000 ,r4 ->cpu_mask : 0x000f0000 	


	and	r4, r4, r9			        @ mask wanted bits
分析如下：
还记得r9吗，它保持的是从cp15的c0寄存器中读取的cpuid,它的值为0x413fc082 ,这样
r4 = r4 & r9  = 0x000f0000 & 0x413fc082 = 0x000f0000 ,如果注意观察前面cpuid的
格式，我们就会发现，这里主要获得是ARM体系版本号。

	teq	r3, r4
	beq	2f

分析如下:
	如果r3和r4相等，则跳到局部标号2,这里是相等的。	

	add	r5, r5, #PROC_INFO_SZ		@ sizeof(proc_info_list)
	cmp	r5, r6
	blo	1b

如果不相等，则比较下一个proc_info_list[下一个CPU的信息],看是否支持，指导将.proc.info.init段
中所有的proc_info_list都比较了一遍。

	mov	r5, #0				@ unknown processor
2:	mov	pc, lr

分析如下:
如果没有知道支持的cpuid,则将r5赋值为0，然后返回，如果找到了则直接返回。
-------------------------------------------------------------------------------------------

	movs	r10, r5				@ invalid processor (r5=0)?
	beq	__error_p			@ yes, error 'p'

分析如下: 
如果r5的值为0,movs这条指令执行的结果就会将CPSR的Z位置1，接下来就会执行
beq __error_p,我们来看看它是如何实现的。

/*
 * Exception handling.  Something went wrong and we can't proceed.  We
 * ought to tell the user, but since we don't have any guarantee that
 * we're even running on the right architecture, we do virtually nothing.
 *
 * If CONFIG_DEBUG_LL is set we try to print out something about the error
 * and hope for the best (useful if bootloader fails to pass a proper
 * machine ID for example).
 */
__error_p:
#ifdef CONFIG_DEBUG_LL
	adr	r0, str_p1
	bl	printascii
	mov	r0, r9
	bl	printhex8
	adr	r0, str_p2
	bl	printascii
	b	__error
str_p1:	.asciz	"\nError: unrecognized/unsupported processor variant (0x"
str_p2:	.asciz	").\n"
	.align
#endif 

__error:
1:	mov	r0, r0
	b	1b

分析如下:
如果Linux 内核没有找到它支持的CPU，就会通过串口输出错误信息"nError:
unrecognized/unrecognized processor variant(0x<cpuid>).\n",然后进入死循环 。
如果找到了, r10中就保存了Linux 支持cpu的proc_info_list结构体首地址。

----------------------------------------------------------------------------------------
	bl	__lookup_machine_type		@ r5=machinfo

分析如下:
在bootloader引导Linux 内核的时候，通过r1寄存器将机器ID传递过来了，__lookup_machine_type 
的实现就是去判断Linux内核是否支持当前的开发板,它的实现过程和上面的 __lookup_processor_type
一样。 

/*
 * Lookup machine architecture in the linker-build list of architectures.
 * Note that we can't use the absolute addresses for the __arch_info
 * lists since we aren't running with the MMU on (and therefore, we are
 * not in the correct address space).  We have to calculate the offset.
 *
 *  r1 = machine architecture number
 * Returns:
 *  r3, r4, r6 corrupted
 *  r5 = mach_info pointer in physical address space
 */
__lookup_machine_type:

分析如下:
请注意看上面的注释 
	
	adr	r3, 4b
	ldmia	r3, {r4, r5, r6}
分析如下:
局部标号4定义如下 

4:	.long	.
	.long	__arch_info_begin
	.long	__arch_info_end

通过前面对 __lookup_processor_type的分析，我们可以知道 
r3 -> 4_physaddr 
r4 -> 4_virtaddr 
r5 -> __arch_info_begin(viraddr) 
r6 -> __arch_info_end(viraddr) 


__arch_info_begin 和 __arch_info_end 在 arch/arm/kernel/vmlinux.lds中定义如下:

  __arch_info_begin = .;
   *(.arch.info.init)
  __arch_info_end = .;

有了前面的基础，大家应该知道这是什么意思了。这种做法是Linux内核惯用的手法，
把内核支持的开发板信息用一个结构体描述，然后都链接到.arch.info.init段中。
然后根据bootloader传递过来的机器ID,去.arch.info.init段中去寻找，判断是否支持。 
好了，接下来我们就来看看Linux 内核具体是怎么做的。

每一块开发板的信息，在Linux 内核中用 machine_desc描述，在 arch/arm/include/asm/mach/arch.h 
中定义如下: 

struct machine_desc {
	/*
	 * Note! The first four elements are used
	 * by assembler code in head.S, head-common.S
	 */
	unsigned int		nr;		/* architecture number	*/
	unsigned int		phys_io;	/* start of physical io	*/
	unsigned int		io_pg_offst;	/* byte offset for io 
						 * page tabe entry	*/

	const char		*name;		/* architecture name	*/
	unsigned long		boot_params;	/* tagged list		*/

	void			(*map_io)(void);/* IO mapping function	*/
	void			(*init_irq)(void);
	struct sys_timer	*timer;		/* system tick timer	*/
	void			(*init_machine)(void);
	...
};

我们当前用的是SMDKC100这块开发板，他对应的machine_desc初始化在arch/arm/mach-s5pc100/mach-smdkc100.c
文件中有如下定义:

MACHINE_START(SMDKC100, "SMDKC100")
	/* Maintainer: Byungho Min <bhmin@samsung.com> */
	.phys_io	= S3C_PA_UART & 0xfff00000,
	.io_pg_offst	= (((u32)S3C_VA_UART) >> 18) & 0xfffc,
	.boot_params	= S5P_PA_SDRAM + 0x100,
	.init_irq	= s5pc100_init_irq,
	.map_io		= smdkc100_map_io,
	.init_machine	= smdkc100_machine_init,
	.timer		= &s3c24xx_timer,
MACHINE_END

其中MACHINE_START和MACHINE_END宏在 arch/arm/include/asm/mach/arch.h中定义如下 

/*
 * Set of macros to define architecture features.  This is built into
 * a table by the linker.
 */
#define MACHINE_START(_type,_name)			\
static const struct machine_desc __mach_desc_##_type	\
 __used							\
 __attribute__((__section__(".arch.info.init"))) = {	\
	.nr		= MACH_TYPE_##_type,		\
	.name		= _name,

#define MACHINE_END				\
};


替换一下，效果如下: 
static const struct machine_desc  __mach_desc_SMDKC100   
__used __attribute__((__section__(".arch.info.init"))) = {
	.nr   = MACH_TYPE_SMDKC100  
	.name = "SMDKC100"
	/* Maintainer: Byungho Min <bhmin@samsung.com> */
	.phys_io	= S3C_PA_UART & 0xfff00000,
	.io_pg_offst	= (((u32)S3C_VA_UART) >> 18) & 0xfffc,
	.boot_params	= S5P_PA_SDRAM + 0x100,
	.init_irq	= s5pc100_init_irq,
	.map_io		= smdkc100_map_io,
	.init_machine	= smdkc100_machine_init,
	.timer		= &s3c24xx_timer,
}

嗯，知道这些以后，接着看汇编指令 

	sub	r3, r3, r4			@ get offset between virt&phys
	add	r5, r5, r3			@ convert virt addresses to
	add	r6, r6, r3			@ physical address space

分析如下:
通过前面的分析，我们可以知道 
r3 -> 4_physaddr 
r4 -> 4_virtaddr 
r5 -> __arch_info_begin(viraddr) 
r6 -> __arch_info_end(viraddr) 

执行完上面的指令，可以达到如下效果: 

r3 -> 4_physaddr - 4_virtaddr 
r5 -> __arch_info_begin(physaddr) 
r6 -> __arch_info_end(physaddr) 

接着看汇编指令
	
1:	ldr	r3, [r5, #MACHINFO_TYPE]	@ get machine type
	teq	r3, r1				@ matches loader number?
	beq	2f				@ found
	add	r5, r5, #SIZEOF_MACHINE_DESC	@ next machine_desc
	cmp	r5, r6
	blo	1b
	mov	r5, #0				@ unknown machine
2:	mov	pc, lr

这段指令完成的功能是从 arch.info.init段中读取machinfo结构体的
读取地一个成员[即机器ID],然后跟bootloader传递过来了的机器ID比较
如果相等则返回，如果不相等从读取下一个machinfo,直到arch.info.init
段结束。
--------------------------------------------------------------------------------------------

	movs	r8, r5				@ invalid machine (r5=0)?
	beq	__error_a			@ yes, error 'a'

分析如下:
如果__lookup_machine_type 没有找到支持的机器ID,就会调用 __error_a ,它会通过串口
打印出
Error: unrecognized/unsupported machine ID (r1 = 0x<machine_ID>")
Available machine support:
ID (hex)\tNAME\n"
Please check your kernel config and/or bootloader."
然后进入死循环 。如果找到了,r8中就保存了Linux内核支持机器的machine_desc结构体
的首地址。

如果引导Linux内核的时候，串口输出以上消息，就说明bootloader传递的机器ID和Linux
内核支持的机器ID不统一,解决这个问题的方法很简单，就是修改机器ID,让两边统一即可。

-------------------------------------------------------------------------------------------------

	bl	__vet_atags

分析如下:
主要完成的功能是检查bootloader传递的atag列表的合法性。

/* Determine validity of the r2 atags pointer.  The heuristic[启发试]requires
 * that the pointer be aligned, in the first 16k of physical RAM and
 * that the ATAG_CORE marker is first and present.  Future revisions
 * of this function may be more lenient with the physical address and
 * may also be able to move the ATAGS block if necessary.
 *
 * r8  = machinfo
 *
 * Returns:
 *  r2 either valid atags pointer, or zero
 *  r5, r6 corrupted
 */
__vet_atags:
我们知道 u_boot 给Linux 内核传递的参数是以tag列表的形式进行传递的。
在FSC100中，uboot传递的tags列表格式如下:
        |                     |
        |内容:无              |          
		|类型:ATAG_NONE       |  		
结束tag |大小:0               |   
		|                     |
		-----------------------
        |                     |                  
	    |内容:bootargs环境变量|
		|类型:ATAG_CMDLINE    |
命令行参|大小:tag头+ bootargs |
数tag   |                     |
        -----------------------
		|                     |
内存信息|内容:内存基地址，大小| 	
tag		|类型:ATAG_MEM        |
		|大小:tag_size(tag_mem)
		|                     |
        -----------------------
		|                     |
		|内容:0               |
		|类型:ATAG_CORE       |
起始tag	|大小:tag_size(tag_core)
		|                     |
		|                     |0x20000100

这个tags列表存放在了物理内存地址0x20000100,其首地址用r2保存了。

	tst	r2, #0x3			@ aligned?
	bne	1f

分析如下:
	判断r2保存的tag列表首地址是否是4字节对齐，如果不是则跳到
	具备标号1,然后返回 

	ldr	r5, [r2, #0]			@ is first tag ATAG_CORE?
	cmp	r5, #ATAG_CORE_SIZE
	cmpne	r5, #ATAG_CORE_SIZE_EMPTY
	bne	1f
	读取tag列表中的开始tag,判断大小是否合法，不合法跳到局部标号1
	,然后返回 

	ldr	r5, [r2, #4]
	ldr	r6, =ATAG_CORE
	cmp	r5, r6
	bne	1f
    读取tag列表中的开始tag,判断其类型是否是ATAG_CORE,不是则跳到局部
	标号1,然后返回 

	//tag合法，正常返回
	mov	pc, lr				@ atag pointer is ok
	
	//非法返回，将r2赋值为0 
1:	mov	r2, #0
	mov	pc, lr

---------------------------------------------------------------------------
	bl	__create_page_tables

分析:
我们知道Linux 内核最终在链接的时候指定的是虚拟地址，到目前为止我们一直
使用的都是物理地址，在使用一些符号的时候，由于这些符号都是虚拟地址，我们 
为了操作正确都是先计算出物理地址与虚拟地址之间的差值，然后将符号的虚拟
地址根据差值转成对应的物理地址。每次都这样做显然是很麻烦的事情，我们迫切
需要做好虚拟地址与物理地址之间的映射关系，然后打开MMU,让MMU自动完成虚拟地址与
物理地址之间的转换。

其中虚拟地址与物理地址之间的映射关系也就是构造页表的过程，关于页表和MMU的知识大家
可以参考我写的<裸奔之MMU>。

接着看汇编指令
/*
 * Setup the initial page tables.  We only setup the barest
 * amount which are required to get the kernel running, which
 * generally means mapping in the kernel code.
 *
 * r8  = machinfo
 * r9  = cpuid
 * r10 = procinfo
 *
 * Returns:
 *  r0, r3, r6, r7 corrupted
 *  r4 = physical page table address
 */
__create_page_tables:
	pgtbl	r4				@ page table address

分析如下:
pgtbl 是一个宏，定义如下  
	.macro	pgtbl, rd
	ldr	\rd, =(KERNEL_RAM_PADDR - 0x4000)
	.endm

#define KERNEL_RAM_PADDR	(PHYS_OFFSET + TEXT_OFFSET)

其中PHYS_OFFSET 在arch/arm/mach-s5pc100/include/mach/memory.h中定义如下:
#define PHYS_OFFSET     	UL(0x20000000)

TEXT_OFFSET在arch/arm/Makefile：214行定义如下:
# The byte offset of the kernel image in RAM from the start of RAM.
TEXT_OFFSET := $(textofs-y)

其中textofs-y在arch/arm/Makefile:111行定义如下:	
textofs-y	:= 0x00008000


替换一下，即为:
	ldr r4 =(0x20008000 - 0x4000) = 0x20004000
	
0x20008000这个地址大家是很熟悉的，这是解压后Linux 内核存放的地址, 现在从这个地址
向下偏移0x4000[16K]的空间主要用来存放页表的。这里采用的映射方法是段式映射，每个段
表示的是1M的虚拟地址空间，如果想映射4G地址空间，就需要4096个段，每个段要占用4Byte
物理内存，所以总共需要4096 * 4byte = 16KB空间。

	/*
	 * Clear the 16K level 1 swapper page table
	 */
	mov	r0, r4
	mov	r3, #0
	add	r6, r0, #0x4000
1:	str	r3, [r0], #4
	str	r3, [r0], #4
	str	r3, [r0], #4
	str	r3, [r0], #4
	teq	r0, r6
	bne	1b

分析:
r4为页表所在物理内存空间首地址，r6为页表的结束的地址，很明白可以知道上面这段指令
是将16KB的物理内存空间清0。


	ldr	r7, [r10, #PROCINFO_MM_MMUFLAGS] @ mm_mmuflags
分析如下:
通过上面对__lookup_processor_type的分析，我们可以知道最终r10保存了proc_info_list
结构体首地址，它的内容如下:
	/*
	 * Match any ARMv7 processor core.
	 */
	.type	__v7_proc_info, #object
__v7_proc_info:
	.long	0x000f0000		@ Required ID value
	.long	0x000f0000		@ Mask for ID
	.long   PMD_TYPE_SECT | PMD_SECT_AP_WRITE |PMD_SECT_AP_READ | PMD_FLAGS
	.long   PMD_TYPE_SECT | PMD_SECT_XN | PMD_SECT_AP_WRITE | PMD_SECT_AP_READ
 	 b	__v7_setup
	.long	cpu_arch_name
	.long	cpu_elf_name
	.long	HWCAP_SWP|HWCAP_HALF|HWCAP_THUMB|HWCAP_FAST_MULT|HWCAP_EDSP
	.long	cpu_v7_name
	.long	v7_processor_functions
	.long	v7wbi_tlb_fns
	.long	v6_user_fns
	.long	v7_cache_fns
	.size	__v7_proc_info, . - __v7_proc_info

其中PROCINFO_MM_MMUFLAGS定义如下: 
#define PROCINFO_MM_MMUFLAGS 8 /* offsetof(struct proc_info_list, __cpu_mm_mmu_flags)	@ */

所以r7的之最终为PMD_TYPE_SECT | PMD_SECT_AP_WRITE |PMD_SECT_AP_READ | PMD_FLAGS，
这些宏的含义是设置映射后的物理空间访问权限和页表中存放的描述符类型。


	/*
	 * Create identity mapping for first MB of kernel to
	 * cater for the MMU enable.  This identity mapping
	 * will be removed by paging_init().  We use our current program
	 * counter to determine corresponding section base address.
	 */

分析如下:
上面注释的意思是，将内核的开始的1M空间做直接映射，也就是说虚拟地址和物理地址
是一样的，这样做的目的是为了最终开始MMU的时候能正常执行。这个映射将会被Linux
内核启动的第三阶段pageing_init()函数移除。我们是使用当前的程序计数器获得相应段
的即地址


我们先来了解一下MMU的段式映射原理，已经虚拟地址和物理地址的转换关系。
        31        20                        0
		|-------------------------------------|
viraddr | Index    |         offset           |    
        |---|--------------------|------------|  
			|                    |
			|                    |
  __________|                    |__________________  
 |                                                 |
 |            段式页表                             |
 |      |                        |                 |
 |      |     ....               |                 |
 |      |                        |                 |
 |      |------------------------|                 |            
 |----->|  phy_base |  ....      | 段描述符        |
        |-----|------------------|                 |
        |     |                  |                 |
		|     |...               |                 | 
		|     |                  |                 |
		|     |                  |                 | 
		|-----|------------------|0x20004000       |
        |     |                  |                 |
              |                        ____________|
              |                        |
		31	  V       20               V
		|-------------|------------------------------|
phyaddr |  phy_base   |      offset                  |
        |-------------|------------------------------|

其中段描述符的格式如下:

31	          20                        1      0
|-------------|-------------------------|------|
|  phy_base   |    ap                   | type | 
|-------------|-------------------------|------|

phy_base : 物理内存的基地址的[31-20]位，由于我们使用的是段式映射
所以这个基地址是1M对其的 
ap       : 对应物理空间的访问权限,例如:只读，只写，读写 
type     : 描述符的类型，如段描述符为10 

我们思考一下，MMU如何通过一个虚拟地址找到对应的物理地址呢? 
|        |
|--------|0xc010,0000          
|        |
|  1M    |---------------|      |             |    
|        |               |      |             |
|--------|0xc000,0000    |      |             |
|        |               |      |             |
|        |               |      |             |            
|        |               |      |-------------|0x2010,0000
|        |               |      |             |
|        |               ------>|     1M      |
  viraddr                       |             |
                                |-------------|0x2000,0000 
							    |             |
									phyaddr 
									
如上图所示我们将虚拟地址0xc000,0000开始的1M空间映射到了物理地址0x2000,0000
开始的1M空间。

怎么告诉MMU，我们想这样映射呢?

我们只需要在页表中写上我们的段描述符就可以了。

第一步:获取我们段描述符，在页表中的索引
index = viraddr >> 20 = 0xC000,0000 >> 20 = 0xc00 

第二步:制作段描述符 

phy_base = phyaddr >> 20 = 0x2000,0000 >> 20 = 0x200 
section_desc = (phy_base << 20) | (ap << 2) | (2 << 0)

第三步:在页表中填写段描述符 
*(table_base + index * 4) = section_desc 

好了，做好映射后，我们假设虚拟地址为0xc000,0123 ,我们分析一下MMU是如何找到
对应的物理地址的。

第一步:获取虚拟地址在页表中的索引 
index = viraddr >> 20 = 0xc000,0123 >> 20 = 0xc00 

第二步:获取段描述符 
section_desc = *(table_base + index * 4)

第三步:计算物理地址 
phy_base = section_desc >> 20 = 0x200 
phyaddr  = (0x200 << 20) | (viraddr & 0xfff0,0000)
         =  0x2000,0000  | 0x0000,0123  
         =  0x2000,0123 

	
	mov	r6, pc
	mov	r6, r6, lsr #20			@ start of kernel section
	orr	r3, r7, r6, lsl #20		@ flags + kernel base
	str	r3, [r4, r6, lsl #2]		@ identity mapping

分析如下:
我们的Linux 内核解压后运行在物理地址0x2000,8000,而启示代码(head.S)是小于1M的。
所以此时PC的位于[0x2000,8000 ~ 0x20010,8000]，即r6的值也位于这个区间。 
r6 = r6 >> 20 = 0x200 ,r7中包存的是存储空间的访问权限和描述符类型。所以 
r3 = (r6<<20) | r7 = (phy_base << 20)  | (ap + type),现在可以知道r3中保存的是 
段描述符  
*(r4 + r6 *4) = r3  ,不要忘记了r4中保持开始页表的基地址(0x2000,4000)哦!,所以 
*(0x20004000 + 0x200 * 4) = section_desc 。

这段指令执行的结果如下:     
 
                                           |              |
|            |                             |              |
|			 |		    				   |              |
|            |                             |              | 
|            |                             |    解压后    |
|------------|0x2010,0000                  |--------------|0x2010,0000
|            |                             |    的内核    |
|            |                             |              |
|   1M       |---------------------------> |              | 
|            |                             |______________|0x2000,8000 
|            |                             |     table    |
|            |                             |______________|0x2000,4000    
|            |                             |              |
|------------|0x2000,0000                  |--------------|0x2000,0000 
|            |                             |              |     
| viraddr    |                             |   phy_addr   |
                                           		  
我们可以看到虚拟地址和物理地址是一样的，这样做的目的是为了打开MMU后，能正常执行。


    /* 
	 * Now setup the pagetables for our kernel direct
	 * mapped region.
	 */
	add	r0, r4,  #(KERNEL_START & 0xff000000) >> 18
分析如下: 
KERNEL_START宏定义如下
#define KERNEL_START	KERNEL_RAM_VADDR
#define KERNEL_RAM_VADDR	(PAGE_OFFSET + TEXT_OFFSET)

其中PAGE_OFFSET值为0xc000,0000 ,TEXT_OFFSET值为0x8000 ,所以 
KERNEL_START 值为0xc000,8000 。 
r0 = r4 + (0xc000,8000 & 0xff000000) >> 18 
   = r4 + (0xc000,0000 >> 18) 
   = r4 + 0x3000

我们知道0xc000,0000是虚拟地址，我们如果要做映射话，先要获取其索引值
即index = (0xC000,0000 >> 20),那上面为什么是 >> 18呢? 
不要忘记了，每个段描述符是需要占用4byte的，也就是说最终在存放段描述符
的时候 ,需要按如下方式存放  
*(table_base + index * 4) = section_desc 

嗯，index * 4等价于 index << 2 ，所以大家可以看到Linux内核在做的时候，
屏蔽掉虚拟地址低位后，直接右移了18bit,实际上等价的。



	str	r3, [r0, #(KERNEL_START & 0x00f00000) >> 18]!
分析如下:
通过上面的分析我们可以知道,r3中保存的是物理基地址0x2000,0000开始的1M空间 
的段描述符。
其中KERNEL_START的值为0xc0000,8000 ,所以(KERNEL_START & 0x00f00000) = 0 ,
其中r0 = r4 + 0x3000 ，所以 *(table_base + 0x3000) = r3 。达到的效果如下： 

|            |
|------------|0xc010,0000 
|            | 
|            |
|   1M       |----------------------|
|            |                      | 
|            |                      |
|------------|0xc000,0000           |
|            |                      |      |              |
|            |                      |      |              |
|			 |		    			|	   |              |
|            |                      |      |              | 
|            |                      |      |    解压后    |
|------------|0x2010,0000           |      |--------------|0x2010,0000
|            |                      |      |    的内核    |
|            |                      |      |              |
|   1M       |---------------------------> |              | 
|            |                             |______________|0x2000,8000 
|            |                             |     table    |
|            |                             |______________|0x2000,4000    
|            |                             |              |
|------------|0x2000,0000                  |--------------|0x2000,0000 
|            |                             |              |     
| viraddr    |                             |   phy_addr   |




	ldr	r6, =(KERNEL_END - 1)
分析如下:
宏KERNEL_END定义如下 
#define KERNEL_END	_end
_end在 arch/arm/kernel/vmlinux.lds中定义,它是Linux 内核的结束地址。
这里将其减1的目的是让r6保存Linux 内核最后一个字节的地址。


	add	r0, r0, #4
分析如下:
通过上面的分析我们知道,r0中保存的是虚拟地址0xc000,0000对应的段
描述在页表中存放的地址。这里将r0 = r0 + 4,很明显此时r0中保存的
是下一个页表项的首地址  


	add	r6, r4, r6, lsr #18 
分析如下:
r6 = r4 + (r6 >> 18),其中r4是页表的基地址，r6为Linux 内核的结束地址,所以 
r6的值为Linux内核最后一个1M空间的虚拟地址对应的段描述符在页表中存放的
地址。 

1:	cmp	r0, r6
	add	r3, r3, #1 << 20
	strls	r3, [r0], #4
	bls	1b

分析:
通过前面的分析我们可以知道r3中的内容如下:
31        20
|----------|-------------------------------------------|
|  0x200   |        ap                          type   |
|----------|-------------------------------------------|
现在很明显，上面的代码就是将整个Linux 内核所占用的物理地址空间与对应的
虚拟地址，建立映射关系。最后到到的效果如下:
|            |  
|     ...    |
|------------|0xc030,0000            
|            | 
|            |
|   2M       |--------------------------|
|            |                          |
|            |                          | 
|            |                          |
|------------|0xc010,0000               |
|            |                          |
|            |                          |
|   1M       |----------------------|   |                
|            |                      |   |  |              | 
|            |                      |   |  |--------------|0x2030,0000  
|------------|0xc000,0000           |   |  |              |
|            |                      |   |  |              |
|            |                      |   -->|              |
|	....	 |		    			|	   |              |
|            |                      |      |              | 
|            |                      |      |    解压后    |
|------------|0x2010,0000           |      |--------------|0x2010,0000
|            |                      |      |    的内核    |
|            |                      |      |              |
|   1M       |---------------------------> |              | 
|            |                             |______________|0x2000,8000 
|            |                             |     table    |
|            |                             |______________|0x2000,4000    
|            |                             |              |
|------------|0x2000,0000                  |--------------|0x2000,0000 
|            |                             |              |     
| viraddr    |                             |   phy_addr   |


	/*
	 * Then map first 1MB of ram in case it contains our boot params.
	 */
	add	r0, r4, #PAGE_OFFSET >> 18
	orr	r6, r7, #(PHYS_OFFSET & 0xff000000)
	str	r6, [r0]

分析如下:
其中PAGE_OFFSET的值为0xC0000000,PHYS_OFFSET的之为0x20000000, r7中保存的
是存储空间访问权限和描述符类型。所以上面的汇编指令完成的功能将0xc000,0000
开始的1M空间映射到0x2000,0000开始的1M空间。 

有人会问，上面不是已经映射过了吗?
之所以要映射这段空间的原因是u_boot传递给Linux 内核的参数存放在0x2000100,
我们Linux内核后面去获取参数使用的已经是虚拟地址了，所以需要映射它。
在这里我们之所以会出现两次映射，原因是我们正好将Linux内核解压在了0x20008000,
这个地址正好落在了0x2000,0000开始的1M空间内，如果将Linux内核解压在别的地址运行，
就不会出现两次映射了。


#ifdef CONFIG_DEBUG_LL
	ldr	r7, [r10, #PROCINFO_IO_MMUFLAGS] @ io_mmuflags
	/*
	 * Map in IO space for serial debugging.
	 * This allows debug messages to be output
	 * via a serial console before paging_init.
	 */
	ldr	r3, [r8, #MACHINFO_PGOFFIO]
	add	r0, r4, r3
	rsb	r3, r3, #0x4000			@ PTRS_PER_PGD*sizeof(long)
	cmp	r3, #0x0800			@ limit to 512MB
	movhi	r3, #0x0800
	add	r6, r0, r3
	ldr	r3, [r8, #MACHINFO_PHYSIO]
	orr	r3, r3, r7
1:	str	r3, [r0], #4
	add	r3, r3, #1 << 20
	teq	r0, r6
	bne	1b
#endif
因为需要通过串口打印信息，我们需要将串口对应的寄存器物理地址映射到对应的虚拟地址
空间。

	//返回  
	mov	pc, lr
---------------------------------------------------------------------------------------
	/*
	 * The following calls CPU specific code in a position independent
	 * manner.  See arch/arm/mm/proc-*.S for details.  r10 = base of
	 * xxx_proc_info structure selected by __lookup_machine_type
	 * above.  On return, the CPU will be ready for the MMU to be
	 * turned on, and r0 will hold the CPU control register value.
	 */
	ldr	r13, __switch_data		@ address to jump to after
分析:
__switch_data定义如下 

	.align	2
	.type	__switch_data, %object
__switch_data:
	.long	__mmap_switched

r13 = *(__switch_data) = __mmap_switched 
---------------------------------------------------------------------------------------

	adr	lr, __enable_mmu 		@ return (PIC) address

分析: 
用lr寄存器保存__enable_mmu标签所在的物理内存地址。 
---------------------------------------------------------------------------------------
 	 add	pc, r10, #PROCINFO_INITFUNC	

分析:
 r10中保存的是 proc_info结构体的首地址，内容如下:

	.section ".proc.info.init", #alloc, #execinstr
	/*
	 * Match any ARMv7 processor core.
	 */
	.type	__v7_proc_info, #object
__v7_proc_info:
	.long	0x000f0000		@ Required ID value
	.long	0x000f0000		@ Mask for ID
	.long   PMD_TYPE_SECT | PMD_SECT_AP_WRITE |PMD_SECT_AP_READ | PMD_FLAGS
	.long   PMD_TYPE_SECT | PMD_SECT_XN | PMD_SECT_AP_WRITE | PMD_SECT_AP_READ
 	 b	__v7_setup
	.long	cpu_arch_name
	.long	cpu_elf_name
	.long	HWCAP_SWP|HWCAP_HALF|HWCAP_THUMB|HWCAP_FAST_MULT|HWCAP_EDSP
	.long	cpu_v7_name
	.long	v7_processor_functions
	.long	v7wbi_tlb_fns
	.long	v6_user_fns
	.long	v7_cache_fns
	.size	__v7_proc_info, . - __v7_proc_info

其中，PROCINFO_INITFUNC 的值为16 ,所以接下来执行的指令为b __v7_setup 
--------------------------------------------------------------------------------------------------------

__v7_setup 中干的事情，就不分析了，主要干的事情有:
清除cahe,设置控制MMU的协处理器寄存器的值，将r4寄存器的值写入了cp15的c2寄存器。
最终在找页表所在基地址的时候， MMU会自动从cp15的c2寄存器中读取。

通过前面的分析，我们知道在跳到 __v7_setup执行的时候，我们在lr寄存器中保存了 
__enable_mmu，所以__v7_setup函数返回的时候，就去执行__enable_mmu了。

---------------------------------------------------------------------------------------------------------
/*
 * Setup common bits before finally enabling the MMU.  Essentially
 * this is just loading the page table pointer and domain access
 * registers.
 */
__enable_mmu:
	mov	r5, #(domain_val(DOMAIN_USER, DOMAIN_MANAGER) | \
		      domain_val(DOMAIN_KERNEL, DOMAIN_MANAGER) | \
		      domain_val(DOMAIN_TABLE, DOMAIN_MANAGER) | \
		      domain_val(DOMAIN_IO, DOMAIN_CLIENT))
	mcr	p15, 0, r5, c3, c0, 0		@ load domain access register
	mcr	p15, 0, r4, c2, c0, 0		@ load page table pointer
	b	__turn_mmu_on

设置了一些MMU需要的参数，然后调用了__turn_mmu_on 

/*
 * Enable the MMU.  This completely changes the structure of the visible
 * memory space.  You will not be able to trace execution through this.
 * If you have an enquiry about this, *please* check the linux-arm-kernel
 * mailing list archives BEFORE sending another post to the list.
 *
 *  r0  = cp#15 control register
 *  r13 = *virtual* address to jump to upon completion
 *
 * other registers depend on the function called upon completion
 */
	.align	5
__turn_mmu_on:
	mov	r0, r0
	mcr	p15, 0, r0, c1, c0, 0		@ write control reg
	mrc	p15, 0, r3, c0, c0, 0		@ read id reg
	mov	r3, r3
	mov	r3, r13
	mov	pc, r3

打开MMU以后，最终pc的值为r13保持的值，通过前面的分析我们可以知道，r13中保保存的 
是__mmap_switched,所以接下来跳到__mmap_switched标签取指令执行

/*
 * The following fragment of code is executed with the MMU on in MMU mode,
 * and uses absolute addresses; this is not position independent.
 *
 *  r0  = cp#15 control register
 *  r1  = machine ID
 *  r2  = atags pointer
 *  r9  = processor ID
 */
__mmap_switched:
	adr	r3, __switch_data + 4
分析如下:
__switch_data标签定义如下
	.align	2
	.type	__switch_data, %object
__switch_data:
	.long	__mmap_switched
	.long	__data_loc			@ r4
	.long	_data				@ r5
	.long	__bss_start			@ r6
	.long	_end				@ r7
	.long	processor_id			@ r4
	.long	__machine_arch_type		@ r5
	.long	__atags_pointer			@ r6
	.long	cr_alignment			@ r7
	.long	init_thread_union + THREAD_START_SP @ sp

所以r3为__data_loc所在内存的首地址。

---------------------------------------------------------------------------------------------------------

	ldmia	r3!, {r4, r5, r6, r7}
分析如下:
	r4->__data_loc 
	r5->_data 
	r6->__bss_start 
	r7->_end 
	r3->为processor_id所在内存的地址

---------------------------------------------------------------------------------------------------------
	cmp	r4, r5				@ Copy data segment if needed
1:	cmpne	r5, r6
	ldrne	fp, [r4], #4
	strne	fp, [r5], #4
	bne	1b
分析如下:
打开System.map可以看到__data_loc 和 _data的值是相等的。所以 
这个地方不需要搬移。

---------------------------------------------------------------------------------------------------------

	mov	fp, #0				@ Clear BSS (and zero fp)
1:	cmp	r6, r7
	strcc	fp, [r6],#4
	bcc	1b
分析如下:
r6为bss段的起始地址，r7为bss段结束地址，所以这段代码完成的是将
bss段清0。

---------------------------------------------------------------------------------------------------------

 	ldmia	r3, {r4, r5, r6, r7, sp} 
分析如下:
	r4->&processor_id 
	r5->&__machine_arch_type 
	r6->&__atags_pointer 
	r7->&cr_alignment 
	sp->&init_thread_union + THREAD_START_SP 

init_thread_union 变量在 arch/arm/kernel/init_task.c:27行定义如下:
union thread_union init_thread_union __init_task_data =
	{ INIT_THREAD_INFO(init_task) };

其中__init_task_data定义如下:

/* Attach to the init_task data structure for proper alignment */
#define __init_task_data __attribute__((__section__(".data..init_task")))

替换一下如下:
union thread_union init_thread_union  
__attribute__((__section__(".data..init_task"))){
	INIT_THREAD_INFO(init_task);
}

内核惯用的手法，将init_thread_union这个联合体强制性放在".data..init_task"段。
由链接脚本 arch/arm/kernel/vmlinux.lds可以知道，它位于data段的起始位置,并且要 
求是8K对齐的。

在来看看union thread_union联合体是如何定义的?

union thread_union {
	struct thread_info thread_info;
	unsigned long stack[THREAD_SIZE/sizeof(long)];
};
其中HEAD_SIZE定义如下:
#define THREAD_SIZE		8192  
联合体的大小是最大成员所占的内存大小,struct thread_info 结构体的大小为52byte。
所以整个联合体的大小为8K。
可以看得出这8k空间，要干两件事情。
第一件:作为struct thread_info 结构体空间 
第二件:作为内核栈空间 

联合体共用空间，这样干是不是有问题？
不会,我们来sp指针的值.

sp->&init_thread_union + THREAD_START_SP 
THREAD_START_SP 定义如下:
#define THREAD_START_SP		(THREAD_SIZE - 8)

所以，我们可以确定sp指针的位置 
  |              |
--|--------------| 
^ |              |<-------------sp 
| |      	     |     
| |              |
8K|              |
| |--------------|
V |     52byte   |    
--|--------------|init_thread_union
  |              |

---------------------------------------------------------------------------------------------------------

	str	r9, [r4]			@ Save processor ID
	str	r1, [r5]			@ Save machine type
	str	r2, [r6]			@ Save atags pointer
分析如下:
r9->cpu_id 
r1->machine_id  
r2->atags_pointer 
r4->&processor_id 
r5->&__machine_arch_type 
r6->&__atags_pointer 
这段指令执行结束后，
processor_id = cpu_id 
__machine_arch_type = machine_id 
__atags_pointer = atags_pointer 

---------------------------------------------------------------------------------------------------------
	
	bic	r4, r0, #CR_A			@ Clear 'A' bit
	stmia	r7, {r0, r4}			@ Save control register values
	b	start_kernel 

嗯，接下来跳到start_kernel,这是有去无回的跳转，到此汇编部分分析结束，接下来进入
Linux 内核的C语言代码。

虽然在分析的时候，我们忽略一些细节，但实际上我们只需要知道大概的流程就可以了。我们 
分析启动流程的最大目的，就是让大家更好的了解Linux 。
