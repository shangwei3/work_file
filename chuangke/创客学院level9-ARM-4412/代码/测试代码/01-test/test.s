.global _start

_start:

@//汇编代码

    mov r0,#1         @//(1)操作数2 为 立即数
	mov r1,r0         @//(2)操作数2 为 寄存器
	mov r2,r1,lsl #1  @//(3)操作数2 为 寄存器移位形式
	mvn r3,#1         @// 取反的值给r3
	
	mov r0,#0xFFFFFFFE
    @mvn r0,#1	

stop: 
	 b stop
	 