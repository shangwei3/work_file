.global _start



_start:
     ldr r0,=data
	 @mov r1,[r0] @//(1).立即数(2).寄存器(3).寄存器移位
	 ldr r1,[r0]
	 
	 
	 @//调用函数
	 @//1.准备参数
	 mov r0,#1
	 mov r1,#2
	 
	 @//2.调用函数
	 bl max
	 
	 @//3.接收返回值
	 mov r4,r0
	
@stop: 
@     b stop
	 
src:                 @//相当于定义一个数组
    .word 0x4578     @// 0111 1000
    .word 0x6789     @// 1000 1001
    .word 0x34567890
    .word 0x45678901

dest:
    .word 0xffffffff
	.word 0xffffffff
	.word 0xffffffff
	.word 0xffffffff
		