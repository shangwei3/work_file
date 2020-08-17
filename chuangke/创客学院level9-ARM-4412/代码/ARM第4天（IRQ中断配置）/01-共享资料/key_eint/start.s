.global _start

_start:
	b reset
	ldr pc,_undefined_instruction
	ldr pc,_software_interrupt
	ldr pc,_prefetch_abort
	ldr pc,_data_abort
	ldr pc,_not_used
	ldr pc,_irq
	ldr pc,_fiq

_undefined_instruction:
	.word _undefined_instruction
_software_interrupt:
	.word _software_interrupt
_prefetch_abort:
	.word _prefetch_abort
_data_abort:
	.word _data_abort
_not_used:
	.word _not_used
_irq:
	.word irq_handler
_fiq:
	.word _fiq

@复位异常处理
reset:
	@告诉arm core异常向量表的位置在_sart label处，其实就是0x40008000
	ldr r0,=_start
	mcr p15,0,r0,c12,c0,0

	@使能irq异常
	mrs r0,cpsr
	mov r1,#1
	bic r0,r0,r1,lsl#7
	msr cpsr,r0

	@设置栈，为c语言运行做准备
	ldr sp,=0x02060000
	bl main   @调到c语言main函数开始执行

irq_handler:
	ldr sp,=0x02059000  @设置irq模式栈,每种模式栈不相同
	stmfd sp!,{r0-r12,lr}@将非异常模式的相关寄存器的值压入栈中，保存，包括lr，保存的是产生中断前非异常模式pc的值

	bl do_irq  @调到中断处理子函数 do_irq执行

	@中断处理完成之后，恢复非异常模式的相关寄存器资源
	ldm sp!,{r0-r12,pc}^  @表示 当指令执行完毕,pc跳转之前,将spsr的值自动拷贝到cpsr中
	@其中恢复寄存器中有pc，就是将中断处理之前的lr值恢复给pc，也即是赋给pc，这样就可以继续执行非异常模式的指令
	@ ^作用是自动回将spsr的值恢复到spsr，我们知道产生异常之前，硬件会自动保存非异常模式下的cpsr保存到spsr
	@那么当异常处理完，恢复spsr-->cpsr，那么整个CPU完全恢复到非异常状态，包括其中模式位，也变为非异常模式


stop:
	b stop
