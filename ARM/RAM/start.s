
.text   @代码段
@注释，数据搬移指令
@mov r0,#0x12    @立即数
@Mov r1,r0

@将不是立即数的数放入寄存器
@ldr 伪指令 
@ldr r1,=0x12345678

@什么是立即数？
@什么是有效数？

@mvn r0,#0x00000000
@mov r0,#0xffffffff

@ldr r1,=0x12


@移位操作指令

@桶型移位器
@逻辑左移，无符号数向左移位，
@ 逻辑移位，移除位，丢弃，  不足位补0
@ldr r0,=0x00fffff0
@lsl r1,r0,#4

@逻辑右移

@lsr r2,r1,#8

@算数右移
@移除位丢弃，高位补符号位
@ASR r1,r0,#3

@循环右移
@ror r0,#4

@算数运算指令
@+ add
@mov r0,#0xF0
@mov r1,#0x0f
@add  r2,r1,r0

@adc  带进位的加法
@64位加法
@a = 0x0000ffff ffff0000
@b = 0x00ffffff 0001ffff
@  = 0x0100ffff 0000ffff
@ldr r1,=0x0000ffff
@ldr r0,=0xffff0000
@ldr r3,=0x00ffffff
@ldr r2,=0x0001ffff

@r4低位  r5高位

@adds r4,r0,r2	  @r4=r0+r2
@adc  r5,r1,r3     @r5=r1+r3+C

@减法指令
@sub

@mov r0,#0xff
@mov r1,#0xf
@sub r2,r0,#0x5

@逆向减法
@RSB
@-250
@mov r0,#1
@rsb r3,r0,#0

@带借位的减法   sbc
@a= 0x0000ffff 0000ffff
@b= 0x000fffff ffffffff
@c= a-b? 0xfff0ffff 00010000

@ldr r0,=0x0000ffff
@ldr r1,=0x0000ffff
@ldr r2,=0xffffffff
@ldr r3,=0x000fffff

@subs r4,r0,r2    @r4= r0-r2
@sbc  r5,r1,r3    @r5= r1-r3-!C


@乘法指令
@mul

@mov r0,#10
@mov r1,#20
@mul r2,r0,r1

@mov r0,#0xff
@lsrs r0,#1


@按位与操作 and

@mov  r0,#10
@mov  r1,#1
@and  r2,r0,r1

@按位或操作 orr

@orr r2,r0,r1 

@按位异或 eor

@eor r2,r0,r1

@位清除操作  bic

@bic r1,r0,#0x1

@位测试指令

@mov  r0,#10
@tst  r0,#0x9

@相等测试指令

@mov  r0,#10
@teq  r0,#0

@比较指令

@cmp r0,#9
@r0 > c = 1

@cmp r0,#11
@r0 < N = 1

@指令的条件执行
@本质上是有cpsr的NZCV位来决定 该条指令需不需要执行

@mov r0,#0
@cmp r0,#0
@moveq r0,#10
@mov r1,#10

@if(a < 0)
@	{
@		a = 10;
@		c = 20;	
@	}else
@	{	
@		b = 10;
@	}


@mov r0,#10
@cmp r0,#0
@movlt r0,#10
@movlt r2,#20
@movge r1,#10

@跳转指令   b不返回的跳转    bl 带返回的跳转

@main:
@mov r0,#10
@b func
@mov r3,#20
@func: 
@	mov r1,#0xff
@	mov r2,#0xff
@b main

@bl 指令  本质：修改PC，b bl 是相对跳转
@ 是短跳转+-32M，长跳转，直接修改PC

@nop
@nop

@mov r0,#10
@保存现场
@b func
@恢复现场
@mov r3,#20
@nop
@@nop

@stop:     @while(1);
@	b stop

@func: 
@	mov r1,#0xff
@	mov r2,#0xff
@	mov pc,lr

@内存操作指令
@单内存操作指令
@ ldr 取内存数据到寄存器  str 将寄存器数据存入内存

ldr  r0,=buf1 @ldr伪指令
ldr r1,[r0]			 @ldr指令  取出内存数据
ldr r2,=buf
str r1,[r2]			 @str指令  写入内存数据

stop:
	b stop
	
	.data  @数据段     int buf[3];  int buf1[3]={1,2,3};
buf:
	.space 12

buf1:
	.word 1,2,3
	
.end    @结束
