.text
.global _start
_start:
		LDR R0,=0x11000C40

		MOV R1,#0x10000000
		STR R1,[R0]
		LDR R0,=0x11000C44

		MOV R1,#0x00000080
		STR R1,[R0]

@~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 		LDR R0,=0x114001E0

		MOV R1,#0x100000
		STR R1,[R0]
		LDR R0,=0x11400000

		MOV R1,#0x20    
		STR R1,[R0]
MAIN_LOOP:
		B MAIN_LOOP