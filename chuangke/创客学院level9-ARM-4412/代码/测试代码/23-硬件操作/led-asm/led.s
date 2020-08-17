.global _start

.equ GPX2CON , 0x11000C40 @//
.equ GPX2DAT , 0x11000C44 @//
_start:
     @//设置对应的引脚为输出
	 ldr r0,=GPX2CON
	 ldr r1,[r0] @//读
	 
	 mov r2,#0xf @//修改
     bic r1,r1,r2,lsl #28
	 mov r3,#0x1
     orr r1,r1,r3,lsl #28
                 @//写
	 str r1,[r0]
    
    
while:	
  	 @//输出指定的电平
     @//关
	 ldr r0,=GPX2DAT
	 ldr r1,[r0]
	 mov r2,#0x1 @//修改
     bic r1,r1,r2,lsl #7
	 @mov r3,#0x1
     @orr r1,r1,r3,lsl #28
                 @//写
	 str r1,[r0]
	 
	 @//延时
	 bl delays
	 @//开
	 ldr r0,=GPX2DAT
	 ldr r1,[r0]
	 mov r2,#0x1 @//修改
     bic r1,r1,r2,lsl #7
	 mov r3,#0x1
     orr r1,r1,r3,lsl #7
                 @//写
	 str r1,[r0]

     @//延时
      bl delays
      b while     


stop:
     b stop
	 
delays:
    ldr r4,=100000000
loop:	
	subs r4,#1
	bne loop 
	mov pc,lr @//回到之前的跳转位置
	