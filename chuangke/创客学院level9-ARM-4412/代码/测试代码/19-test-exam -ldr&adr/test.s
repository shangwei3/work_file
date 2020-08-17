.global _start

_start:
    ldr r0,=src  @// 把src的标签名所代表的地址给了r0
    ldr r1,src	 @// 把src对应的地址中的内容给了r1
    adr r2,src
	@adr r3,=src  @//不存在这样指令
	
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
	
	 