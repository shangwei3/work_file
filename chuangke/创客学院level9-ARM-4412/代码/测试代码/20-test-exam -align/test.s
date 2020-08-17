.global _start

_start:
     mov r0,#1
	 .align 4   @// 指定16字节对齐
	            @//只会影响到一条指令

2:@//向前 front 
     mov r1,#2
	 mov r2,#3
	 
	 b 2f
	 b 2b
2: @// back --向后(代码执行的方向)
    add r1,r2
	
stop: 
     b stop
	 
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
	
	 