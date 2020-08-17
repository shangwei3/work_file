.global _start

_start:

    mov r0,#1
    mov r1,#2
	mov r2,#0xFFFFFFFF
	
	subs r3,r0,r1 @// r3 = r0-r1
	rsb  r4,r0,r1 @// r4 = r1-r0 反向减法
	
	mul r5,r0,r1  @// r5 = r0 * r1 
	
	mla r6,r0,r1,r2 @// r6 = r0*r1 + r2 = 1*2 + 3 = 5
	
	umull r7,r8,r2,r1 @//(r8,r7) = r2 * r1
	umlal r7,r8,r2,r1 @//(r8,r7) = (r8,r7)+r2 * r1 = 0x1 ffff fffe 
	                  @//                          = 0x3 ffff fffc
	smull r7,r8,r2,r1 @//(r8,r7) = r2 * r1 最高位符号位扩展 
	                  @//  = 0xffff ffff ffff fffe 
	smlal r7,r8,r2,r1 @//(r8,r7) = (r8,r7)+r2 * r1 = 0xffff ffff ffff fffc
	                  @//                            
	                  @//                          			  
	
stop: 
	 b stop
	 