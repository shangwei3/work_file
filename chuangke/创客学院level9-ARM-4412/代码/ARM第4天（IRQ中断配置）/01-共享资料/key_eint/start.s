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

@��λ�쳣����
reset:
	@����arm core�쳣�������λ����_sart label������ʵ����0x40008000
	ldr r0,=_start
	mcr p15,0,r0,c12,c0,0

	@ʹ��irq�쳣
	mrs r0,cpsr
	mov r1,#1
	bic r0,r0,r1,lsl#7
	msr cpsr,r0

	@����ջ��Ϊc����������׼��
	ldr sp,=0x02060000
	bl main   @����c����main������ʼִ��

irq_handler:
	ldr sp,=0x02059000  @����irqģʽջ,ÿ��ģʽջ����ͬ
	stmfd sp!,{r0-r12,lr}@�����쳣ģʽ����ؼĴ�����ֵѹ��ջ�У����棬����lr��������ǲ����ж�ǰ���쳣ģʽpc��ֵ

	bl do_irq  @�����жϴ����Ӻ��� do_irqִ��

	@�жϴ������֮�󣬻ָ����쳣ģʽ����ؼĴ�����Դ
	ldm sp!,{r0-r12,pc}^  @��ʾ ��ָ��ִ�����,pc��ת֮ǰ,��spsr��ֵ�Զ�������cpsr��
	@���лָ��Ĵ�������pc�����ǽ��жϴ���֮ǰ��lrֵ�ָ���pc��Ҳ���Ǹ���pc�������Ϳ��Լ���ִ�з��쳣ģʽ��ָ��
	@ ^�������Զ��ؽ�spsr��ֵ�ָ���spsr������֪�������쳣֮ǰ��Ӳ�����Զ�������쳣ģʽ�µ�cpsr���浽spsr
	@��ô���쳣�����꣬�ָ�spsr-->cpsr����ô����CPU��ȫ�ָ������쳣״̬����������ģʽλ��Ҳ��Ϊ���쳣ģʽ


stop:
	b stop
