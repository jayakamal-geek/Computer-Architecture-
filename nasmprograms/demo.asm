%include "io.mac"

section .data
myvariable db 'Hello World, welcome to assembly language programming',0
section .bss

section .text
global _start
_start:
	PutStr myvariable
	nwln

	mov eax,1
	xor ebx,ebx
	int 0x80
	
