		;org		0

%include "sconst.inc"

Selector_Kernel_cs		equ		8

extern	gdt_ptr
extern  idt_ptr
extern 	cstart
extern	kernel_main
extern	p_proc_ready
extern	tss

[section	.bss]
StackSpace		resb	2*1024
Stack_Top:

global _start
global restart

[section	.text]
_start:
		mov esp, Stack_Top
		
		xchg bx, bx
		
		mov edi, (80*6+0)*2
		mov al, 'K'
		mov ah, 0001001b
		mov [gs:edi], ax
		
		sgdt	[gdt_ptr]
		call	cstart
		
		lgdt	[gdt_ptr]
		lidt	[idt_ptr]
		
		xchg bx, bx
		jmp		Selector_Kernel_cs:cinit
		
cinit:
		
		;ud2
		;jmp	0x40:0
		;sti
		;call TestA
		;call kernel_main
		xor eax, eax
		mov ax, SELECTOR_TSS
		ltr ax
		jmp kernel_main
		
		hlt

restart:
	mov	esp, [p_proc_ready]
	lldt	[esp + P_LDT_SEL]
	lea	eax, [esp + P_STACKTOP]
	mov	dword [tss + TSS3_S_SP0], eax

	pop	gs
	pop	fs
	pop	es
	pop	ds
	popad

	add	esp, 4

	iretd

