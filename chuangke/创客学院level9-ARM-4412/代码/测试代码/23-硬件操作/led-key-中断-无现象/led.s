.global _start

_start:
@//(1)ʵ���ж�������
@//(2)ʵ��reset �쳣
@// a.����arm �� �ж��쳣�������λ��
@// b.�����ж�(irq)
@// c.����ջ��ָ�� ��ת��main����
@//(3)ʵ��irq   �쳣
@// a.����һ�� ���ص�ַ
@// b.�����ֳ�(ѹջ)
@// c.bl irq ---->ִ���жϷ������
@// d.�ж�ִ����ɣ��ָ��ֳ�
  b reset
  ldr pc,_undefined_instruction
  ldr pc,_software_interrupt
  ldr pc,_prefetch_abort
  ldr pc,_data_abort
  ldr pc,_not_used
  ldr pc,_irq
  ldr pc,_fiq @// 1.���ȼ� 2,˽�мĴ��� 3.�������е�λ��
  
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
@//(2)ʵ��reset �쳣
@// a.����arm �� �ж��쳣�������λ��
@// b.�����ж�(irq)
@// c.����ջ��ָ�� ��ת��main����
@//(3)ʵ��irq   �쳣
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
	
	bl do_irq @//�жϴ������
	
	ldmfd sp!,{r0-r12,pc}^
	
    
stop:
     b stop
	 

	