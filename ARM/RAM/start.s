
.text   @�����
@ע�ͣ����ݰ���ָ��
@mov r0,#0x12    @������
@Mov r1,r0

@��������������������Ĵ���
@ldr αָ�� 
@ldr r1,=0x12345678

@ʲô����������
@ʲô����Ч����

@mvn r0,#0x00000000
@mov r0,#0xffffffff

@ldr r1,=0x12


@��λ����ָ��

@Ͱ����λ��
@�߼����ƣ��޷�����������λ��
@ �߼���λ���Ƴ�λ��������  ����λ��0
@ldr r0,=0x00fffff0
@lsl r1,r0,#4

@�߼�����

@lsr r2,r1,#8

@��������
@�Ƴ�λ��������λ������λ
@ASR r1,r0,#3

@ѭ������
@ror r0,#4

@��������ָ��
@+ add
@mov r0,#0xF0
@mov r1,#0x0f
@add  r2,r1,r0

@adc  ����λ�ļӷ�
@64λ�ӷ�
@a = 0x0000ffff ffff0000
@b = 0x00ffffff 0001ffff
@  = 0x0100ffff 0000ffff
@ldr r1,=0x0000ffff
@ldr r0,=0xffff0000
@ldr r3,=0x00ffffff
@ldr r2,=0x0001ffff

@r4��λ  r5��λ

@adds r4,r0,r2	  @r4=r0+r2
@adc  r5,r1,r3     @r5=r1+r3+C

@����ָ��
@sub

@mov r0,#0xff
@mov r1,#0xf
@sub r2,r0,#0x5

@�������
@RSB
@-250
@mov r0,#1
@rsb r3,r0,#0

@����λ�ļ���   sbc
@a= 0x0000ffff 0000ffff
@b= 0x000fffff ffffffff
@c= a-b? 0xfff0ffff 00010000

@ldr r0,=0x0000ffff
@ldr r1,=0x0000ffff
@ldr r2,=0xffffffff
@ldr r3,=0x000fffff

@subs r4,r0,r2    @r4= r0-r2
@sbc  r5,r1,r3    @r5= r1-r3-!C


@�˷�ָ��
@mul

@mov r0,#10
@mov r1,#20
@mul r2,r0,r1

@mov r0,#0xff
@lsrs r0,#1


@��λ����� and

@mov  r0,#10
@mov  r1,#1
@and  r2,r0,r1

@��λ����� orr

@orr r2,r0,r1 

@��λ��� eor

@eor r2,r0,r1

@λ�������  bic

@bic r1,r0,#0x1

@λ����ָ��

@mov  r0,#10
@tst  r0,#0x9

@��Ȳ���ָ��

@mov  r0,#10
@teq  r0,#0

@�Ƚ�ָ��

@cmp r0,#9
@r0 > c = 1

@cmp r0,#11
@r0 < N = 1

@ָ�������ִ��
@����������cpsr��NZCVλ������ ����ָ���費��Ҫִ��

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

@��תָ��   b�����ص���ת    bl �����ص���ת

@main:
@mov r0,#10
@b func
@mov r3,#20
@func: 
@	mov r1,#0xff
@	mov r2,#0xff
@b main

@bl ָ��  ���ʣ��޸�PC��b bl �������ת
@ �Ƕ���ת+-32M������ת��ֱ���޸�PC

@nop
@nop

@mov r0,#10
@�����ֳ�
@b func
@�ָ��ֳ�
@mov r3,#20
@nop
@@nop

@stop:     @while(1);
@	b stop

@func: 
@	mov r1,#0xff
@	mov r2,#0xff
@	mov pc,lr

@�ڴ����ָ��
@���ڴ����ָ��
@ ldr ȡ�ڴ����ݵ��Ĵ���  str ���Ĵ������ݴ����ڴ�

ldr  r0,=buf1 @ldrαָ��
ldr r1,[r0]			 @ldrָ��  ȡ���ڴ�����
ldr r2,=buf
str r1,[r2]			 @strָ��  д���ڴ�����

stop:
	b stop
	
	.data  @���ݶ�     int buf[3];  int buf1[3]={1,2,3};
buf:
	.space 12

buf1:
	.word 1,2,3
	
.end    @����
