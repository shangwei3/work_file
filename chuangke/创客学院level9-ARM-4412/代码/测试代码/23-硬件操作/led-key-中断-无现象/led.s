.global _start

_start:
@//(1)实现中断向量表
@//(2)实现reset 异常
@// a.告诉arm 核 中断异常向量表的位置
@// b.允许中断(irq)
@// c.设置栈顶指针 跳转到main函数
@//(3)实现irq   异常
@// a.调整一下 返回地址
@// b.保护现场(压栈)
@// c.bl irq ---->执行中断服务程序
@// d.中断执行完成，恢复现场
  b reset
  ldr pc,_undefined_instruction
  ldr pc,_software_interrupt
  ldr pc,_prefetch_abort
  ldr pc,_data_abort
  ldr pc,_not_used
  ldr pc,_irq
  ldr pc,_fiq @// 1.优先级 2,私有寄存器 3.向量表中的位置
  
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
  

reset:
@//(2)实现reset 异常
@// a.告诉arm 核 中断异常向量表的位置
@// b.允许中断(irq)
@// c.设置栈顶指针 跳转到main函数
@//(3)实现irq   异常
   ldr r0,=_start
   mcr p15,0,r0,c12,c0,0
   
   mrs r0,cpsr
   mov r1,#1
   bic r0,r1,lsl #7
   msr cpsr,r0
   
   ldr sp,=0x02060000 
   bl main
   
irq_handler:

    sub lr,#4
	
	ldr sp,=0x02059000
	
	stmfd sp!,{r0-r12,lr}
	
	bl do_irq @//中断处理程序
	
	ldmfd sp!,{r0-r12,pc}^
	
    
stop:
     b stop
	 

	