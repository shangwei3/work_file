.global _start

_start:
    ldr r0,=src  
	mov r4,#8
	
	ldr r1,[r0]
	ldr r2,[r0,#4]
	ldr r3,[r0,r4]
	
	ldr r2,[r0,#4]!
	ldr r3,[r0,r4]!
	
	
	ldr r0,=dest
	str r4,[r0,#8]!
	str r4,[r0],#8 @// (1).r0 = r0+8 (2).r4 => [r0]  
	str r4,[r0,#8] @// (1).r0+8      (2).r4 => [r0+8]
  
  
    ldr r0,=src
	
    ldrb  r1,[r0]
    ldrsb r1,[r0]	
	
	ldrb  r1,[r0,#4]
	ldrsb r1,[r0,#4]
	
	
	  ldr r0,=src
	
    ldrh  r1,[r0]
    ldrsh r1,[r0]	
	
	ldrh  r1,[r0,#4]
	ldrsh r1,[r0,#4]
stop: 
     b stop
	 
src:   @//相当于定义一个数组
    .word 0x4578   @// 0111 1000
    .word 0x6789   @// 1000 1001
    .word 0x34567890
    .word 0x45678901

dest:
    .word 0xffffffff
	.word 0xffffffff
	.word 0xffffffff
	.word 0xffffffff
	
	 