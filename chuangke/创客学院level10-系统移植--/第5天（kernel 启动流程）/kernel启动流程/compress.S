
		.section ".start", #alloc, #execinstr
/*
 * sort out different calling conventions
 */
		.align
start:
		.type	start,#function
		.rept	8
		mov	r0, r0
		.endr

解析如下:

.type 用来指定一个符号的类型是函数类型或者是对象
类型，对象类型一般是数据，格式如下:
.type 符号，类型描述[function，object]

例如1:
.type a,@object 

a:
	.word 100 


.rept 8
 指令 
.endr   

表示，重复rept到endr之间的指令8次 

注意:这里重复了8次mov指令，也就是占用了32字节,用来存放ARM的异常向量表的
---------------------------------------------------------------------
		b	1f
		.word	0x016f2818		@ Magic numbers to help the loader
		.word	start			@ absolute load/run zImage address
		.word	_edata			@ zImage end address
1:		mov	r7, r1			    @ save architecture ID
		mov	r8, r2			    @ save atags pointer

解析如下: 
0x016f2818 这个数字是bootloader和zImage之间规定好的一个数字，用于bootloader 
判断它所跳到的地址是否是zImage镜像 

start 由连接生成arch/arm/boot/compressed/vmlinux.lds决定 
所以start的值为0,_edata为zImage的结束地址 
-----------------------------------------------------------------------------------
		/*
		 * Booting from Angel - need to enter SVC mode and disable
		 * FIQs/IRQs (numeric definitions from angel arm.h source).
		 * We only do this if we were in user mode on entry.
		 */
		mrs	r2, cpsr		@ get current mode
		tst	r2, #3			@ not user?
		bne	not_angel
		mov	r0, #0x17		@ angel_SWIreason_EnterSVC
 ARM(		swi	0x123456	)	@ angel_SWI_ARM
 THUMB(		svc	0xab		)	@ angel_SWI_THUMB
not_angel:
		mrs	r2, cpsr		@ turn off interrupts to
		orr	r2, r2, #0xc0		@ prevent angel from running
		msr	cpsr_c, r2

解析如下:
我们的开发板上是通过u_boot来引导Linux 内核的，在启动Linux 内核的时候,u_boot已经将 
ARM设置为SVC模式了，注意看上面的注释部分，如果是使用Angel引导Linux 内核，此时ARM 
可能在user模式，所以需要从user模式切换到svc模式，这里采用的方法是swi指令，产生
软中断异常，软中断异常一旦产生就会自动进入SVC模式了。
其中ARM(),THUMB()这两个宏是在arch/arm/include/asm/unified.h中定义，主要是为了
兼容ARM，THUMB指令的,不要去纠结它，没什么意义，我们这里从u_boot引导Linux内核，可以
直接跳到no_angle标签,干的事情也很简单，将中断异常关闭了。
----------------------------------------------------------------------------------

		/*
		 * some architecture specific code can be inserted
		 * by the linker here, but it should preserve r7, r8, and r9.
		 */

		.text
		adr	r0, LC0
 		ldmia	r0, {r1, r2, r3, r4, r5, r6, r11, ip, sp} 
		subs	r0, r0, r1		@ calculate the delta offset
		
						@ if delta is zero, we are
		beq	not_relocated		@ running at the address we
						@ were linked at.

解析如下：
	    adr指令是根据pc的值来计算LC0所在的地址,通过反汇编vmlinux我们会发现，最终 
		这条指令被翻译成60:add r0,pc,#208;0xd0 
		
		我们是将zImage加载到0x20008000这个地址运行的，所以pc是从20008000这个地址
		取指令执行的，我们知道当这条指令执行的时候，pc的值应该为
		0x20008000 + 0x60 + 8 = 0x20008068,所以r0 = pc + 0xd0 =0x20008138 

在arch/arm/boot/compressed/head.S:310行有如下定义 
		.align	2
		.type	LC0, #object
LC0:		
	    .word	LC0		    	@ r1
通过反汇编vmlinux我们可以知道LC0这个标签的值为0x138,所以r1的值为0x138,所以此时 
subs r0,r0,r1 的结果不会影响ARM核的CPSR的z位，r0的值为0x20008000 。
-------------------------------------------------------------------------------------
		/*
		 * We're running at a different address.  We need to fix
		 * up various pointers:
		 *   r5 - zImage base address (_start)
		 *   r6 - size of decompressed image
		 *   r11 - GOT start
		 *   ip - GOT end
		 */
		add	r5, r5, r0
		add	r11, r11, r0
		add	ip, ip, r0 

		/*
		 * If we're running fully PIC === CONFIG_ZBOOT_ROM = n,
		 * we need to fix up pointers into the BSS region.
		 *   r2 - BSS start
		 *   r3 - BSS end
		 *   sp - stack pointer
		 */
		add	r2, r2, r0
		add	r3, r3, r0
		add	sp, sp, r0

解析如下: 

在arch/arm/boot/compressed/head.S:310行有如下定义 
		.align	2
		.type	LC0, #object
LC0:		
	    .word	LC0		    	@ r1
		.word	__bss_start		@ r2
		.word	_end			@ r3
		.word	zreladdr		@ r4
		.word	_start			@ r5
		.word	_image_size		@ r6
		.word	_got_start		@ r11
		.word	_got_end		@ ip
		.word	user_stack+4096		@ sp 
	
通过前面的分析，我们已经确定了r0的值为0x20008000，即你加载zImage所在的内存
地址。上面那些add指令就是算出不同的段在内存中实际地址，保存在不同的寄存器中。

这里我们重点分析一下:r4 = zreladdr,sp = user_stack + 4096  

如果大家注意观察大家就会发现，arch/arm/boot/compressed/vmlinux在生成
的时候，有如下信息: 
arm-cortex_a8-linux-gnueabi-ld -EL    --defsym zreladdr=0x20008000 --defsym
params_phys=0x20000100  ....  -o arch/arm/boot/compressed/vmlinux 

zreladdr和params_phys的值最终由arch/arm/mach-$(SOC)/Makefile.boot决定,这里使用 
的SOC是s5pc100,打开arch/arm/mach-s5pc100/Makefile.boot大家可以看到如下信息:

  zreladdr-y :=0x20008000 
params_phys-y:=0x20000100 

所以在这里,r4的值为20008000 


在arch/arm/boot/compressed/head.S:1072行，有如下内容 
		.align
		.section ".stack", "w"
user_stack:	.space	4096

根据arch/arm/boot/compressed/vmlinux.lds 我们可以知道user_stack标签位于bss段后面,
.space 4096表示分配4k的空间，可以知道,此时我们的栈大小为4K
-----------------------------------------------------------------------------------
		/*
		 * Relocate all entries in the GOT table.
		 */
1:		ldr	r1, [r11, #0]		@ relocate entries in the GOT
		add	r1, r1, r0		@ table.  This fixes up the
		str	r1, [r11], #4		@ C references.
		cmp	r11, ip
		blo	1b


分析如下: 
	  将got段中所有的数据都加上0x20008000,为什么需要这样做请参考我写的<got段与-fpic详解>
----------------------------------------------------------------------------------
not_relocated:	
		mov	r0, #0

1:		str	r0, [r2], #4		@ clear bss
		str	r0, [r2], #4
		str	r0, [r2], #4
		str	r0, [r2], #4
		cmp	r2, r3
		blo	1b 

解析如下: 
通过前面分析，我们已经知道r2中保存的是bss段的启示地址,所以 
这段代码完成的功能为:将BSS段清0 ,其中lo为无符号小于 
-----------------------------------------------------------------------------------
		/*
		 * The C runtime environment should now be setup
		 * sufficiently.  Turn the cache on, set up some
		 * pointers, and start decompressing.
		 */
		bl	cache_on

		mov	r1, sp			@ malloc space above stack
		add	r2, sp, #0x10000	@ 64k max

解析如下: 
	   打开cache,设置C语言运行环境,r1为堆的起始地址,r2为堆的结束地址 
-----------------------------------------------------------------------------------
/*
 * Check to see if we will overwrite ourselves.
 *   r4 = final kernel address
 *   r5 = start of this image
 *   r6 = size of decompressed image
 *   r2 = end of malloc space (and therefore this image)
 * We basically want:
 *   r4 >= r2 -> OK
 *   r4 + image length <= r5 -> OK
 */
		cmp	r4, r2
		bhs	wont_overwrite [hs:无符号大于等于]
		add	r0, r4, r6
		cmp	r0, r5
		bls	wont_overwrite [ls:无符号小于等于]

解析如下:  
r4 记录了zImage解压后，内核的运行地址，这里为0x20008000
r5 记录了zImage加载到内存中，运行的起始地址，我们是直接将zImage加载到内存地址  
0x20008000运行的，所以这里为0x20008000 
r6 记录了解压后内核的大小,它的计算规则可以从arc/arm/boot/compressed/vmlinux.lds 
中知道  
r2 记录了堆的结束地址:0x20008000 + ... + 4k  

这里的代码，主要是探测解压后的Linux 内核是否会覆盖掉当前运行的代码。
|                |
|                |
|----------------|堆的结束地址 [r2]
|    堆64k       |   
|----------------|
|    栈4k        |
|----------------|
|                |
|  压缩的内核    |
|  当前运行的代码|      
|                |
|                |
|----------------|0x20008000 zImage的加载地址 [r4,r5]
|                |  

我们可以知道，如果不想被覆盖自己，必须满足以下条件:
解压后的Linux内核所在地址(即最终Linux内核的运行地址) >= 堆的结束地址[r2]
或 
解压后的Linux内核所在地址(即最终Linux内核的运行地址) + 解压后的Linux内核大小 <= zImage的加载地址 

通过分析，我们知道，这里r4 < r2  ,r4 + r6 >r5,所以如果直接解压是会覆盖自己的,
那怎么办?只能先解压多其它空闲内存，然后再搬移到指定的地址运行了。 
--------------------------------------------------------------------------------------------------
		mov	r5, r2			@ decompress after malloc space
		mov	r0, r5
		mov	r3, r7
		bl	decompress_kernel

解析如下：
r0:堆结束的地址 
r1:堆起始的地址 
r2:堆结束的地址  
r3:机器ID 

unsigned long decompress_kernel(
		unsigned long output_start, //r0  
		unsigned long free_mem_ptr_p,//r1 
		unsigned long free_mem_ptr_end_p,//r2 
		int arch_id//r3 
){
	unsigned char *tmp;
	
	//解压后内核存放的地址
	output_data		= (unsigned char *)output_start;
	//堆的起始地址
	free_mem_ptr		= free_mem_ptr_p;
	//堆的结束地址  
	free_mem_end_ptr	= free_mem_ptr_end_p;
	//机器ID 
	__machine_arch_type	= arch_id;

	arch_decomp_setup();
	

	tmp = (unsigned char *) (((unsigned long)input_data_end) - 4);
	output_ptr = get_unaligned_le32(tmp);
		
	//打印开始解压信息
	putstr("Uncompressing Linux...");
	
	//解压 
	do_decompress(input_data, input_data_end - input_data,
			output_data, error);
	
	//打印解压结束信息 
	putstr(" done, booting the kernel.\n");
	
	return output_ptr;
}

总结:最终将Linux内核解压在了堆区上面  
------------------------------------------------------------------------------------------
		add	r0, r0, #127 + 128	@ alignment + stack
		bic	r0, r0, #127		@ align the kernel length

分析如下:
r0为decompress_kernel()函数的返回值，它的返回值最终为Linux内核解压后的长度,
这里的第一条指令完成的功能是在解压后的Linux内核后面预留128字节的栈空间,
第二条指令使最终r0的值为128字节对齐 

|                | 
|                |   
|                |
|----------------|---- 
|  128byte       |  |
|                |  |
|    +           |  |   
|                |  r0 
| 解压后的内核   |  |
|                |  |
|                |  |
|----------------|<-------r5 
|    堆64k       |   
|----------------|
|    栈4k        |
|----------------|
|                |
|  压缩的内核    |
|  当前运行的代码|      
|                |
|----------------|0x20008000 zImage的加载地址 
|                |  

------------------------------------------------------------------------------------------
/*
 * r0     = decompressed kernel length
 * r1-r3  = unused
 * r4     = kernel execution address
 * r5     = decompressed kernel start
 * r7     = architecture ID
 * r8     = atags pointer
 * r9-r12,r14 = corrupted
 */
		add	r1, r5, r0		@ end of decompressed kernel
		adr	r2, reloc_start
		ldr	r3, LC1
		add	r3, r2, r3
1:		ldmia	r2!, {r9 - r12, r14}	@ copy relocation code
		stmia	r1!, {r9 - r12, r14}
		ldmia	r2!, {r9 - r12, r14}
		stmia	r1!, {r9 - r12, r14}
		cmp	r2, r3
		blo	1b

解析如下: 
	  r1 = r5 + r0 = 解压后内核存放的地址 + 内核大小 
	  r2 = 当前reloc_start标签所在的地址 
	  r3 = *LC1 
LC1:		.word	reloc_end - reloc_start
	  所以r3 为重定位代码段的大小 
	  r3 = r2 + r3 =重定位代码段的结束地址  
	  接下来的指令就是将重定位的代码段搬移到解压的Linux内核后面 

|                |
|----------------|<---r1 
|                | 
| 重定位代码段   |   
|                |
|----------------|---- 
|  128byte       |  |
|                |  |
|    +           |  |   
|                |  r0 
| 解压后的内核   |  |
|                |  |
|                |  |
|----------------|<-------r5 
|    堆64k       |   
|----------------|
|    栈4k        |
|----------------|
|                |
|  压缩的内核    |
|  当前运行的代码|      
|                |
|----------------|0x20008000 zImage的加载地址 
|                |  


思考:为什么要搬移重定位代码段呢?
答案:重定位代码段的功能是将Linux内核搬移到内核实际指定的地址运行
，搬过来的内核可能会将重定位代码段覆盖，所以安全的做法就是将重定位
代码段先搬移走，然后在执行重定位代码段，将Linux内核搬移到内核实际指定的地址 


-------------------------------------------------------------------------------------
		mov	sp, r1
		add	sp, sp, #128		@ relocate the stack

		bl	cache_clean_flush
 		add	pc, r5, r0		    @ call relocation code

解析如下: 
|                |           
|----------------|<---sp  
|    128byte     |
|----------------|<---r1  
|                | 
| 重定位代码段   |   
|                |
|----------------|<---pc  
|  128byte       |  |
|                |  |
|    +           |  |   
|                |  r0 
| 解压后的内核   |  |
|                |  |
|                |  |
|----------------|------->r5 
|    堆64k       |   
|----------------|
|    栈4k        |
|----------------|
|                |
|  压缩的内核    |
|  当前运行的代码|      
|                |
|----------------|0x20008000 zImage的加载地址 
|                |  

-----------------------------------------------------------------------


/*
 * All code following this line is relocatable.  It is relocated by
 * the above code to the end of the decompressed kernel image and
 * executed there.  During this time, we have no stacks.
 *
 * r0     = decompressed kernel length
 * r1-r3  = unused
 * r4     = kernel execution address
 * r5     = decompressed kernel start
 * r7     = architecture ID
 * r8     = atags pointer
 * r9-r12,r14 = corrupted
 */
		.align	5
reloc_start:	
		add	r9, r5, r0
		sub	r9, r9, #128		@ do not copy the stack
		mov	r1, r4 

分析如下: 
|                |           
|----------------|<---sp  
|    128byte     |
|----------------|  
|                | 
| 重定位代码段   |   
|                |
|----------------|<---pc  
|  128byte       |  |
|                |  |
|    +           |<-|----r9    
|                |  r0 
| 解压后的内核   |  |
|                |  |
|                |  |
|----------------|------->r5 
|    堆64k       |   
|----------------|
|    栈4k        |
|----------------|
|                |
|  压缩的内核    |
|  当前运行的代码|      
|                |
|----------------|0x20008000 [r1/r4] 
|                |  

--------------------------------------------------------------------------------

1:
		.rept	4
		ldmia	r5!, {r0, r2, r3, r10 - r12, r14}	@ relocate kernel
		stmia	r1!, {r0, r2, r3, r10 - r12, r14}
		.endr

		cmp	r5, r9
		blo	1b

分析如下: 

|                |           
|----------------|<------------sp  
|    128byte     |
|----------------|  
|                |<------------pc 
| 重定位代码段   |   
|                |
|----------------|   
|                |   
|                |<------------r1
|   ......       | 
|                |
|                |
|   解压后的内核 |
|                |
|                |
|----------------|0x20008000 [r4]  
|                |
----------------------------------------------------------------------

		mov	sp, r1
		add	sp, sp, #128		@ relocate the stack

call_kernel:	
        bl	cache_clean_flush
		bl	cache_off
		mov	r0, #0			@ must be zero
		mov	r1, r7			@ restore architecture number
		mov	r2, r8			@ restore atags pointer
		mov	pc, r4			@ call kernel

分析如下: 

|                |           
|----------------| 
|    128byte     |
|----------------|  
|                | 
| 重定位代码段   |   
|                |
|----------------|
|                |
|                |
|----------------|<----------sp 
|                |
|     128byte    | 
|----------------|
|                |
|   ......       | 
|                |
|                |
|   解压后的内核 |
|                |
|                |
|----------------|0x20008000 [r4,pc]  
|                |

嗯，自解压过程结束，最终执行解压后的Linux内核。
