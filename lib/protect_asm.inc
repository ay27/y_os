; 中断和异常处理的汇编入口

extern	exception_handler
extern	spurious_irq

global	divide_error
global	single_step_exception
global 	nmi
global	breakpoint_exception
global	overflow
global	bounds_check
global	inval_opcode
global	copr_not_available
global	double_fault
global	copr_seg_overrun
global	inval_tss
global	segment_not_present
global	stack_exception
global	general_protection
global	page_fault
global	copr_error
	global  hwint00
	global  hwint01
	global  hwint02
	global  hwint03
	global  hwint04
	global  hwint05
	global  hwint06
	global  hwint07
	global  hwint08
	global  hwint09
	global  hwint10
	global  hwint11
	global  hwint12
	global  hwint13
	global  hwint14
	global  hwint15




; 中断和异常 -- 硬件中断，使用8259芯片
; ---------------------------------
%macro  hwint_master    1
        push    %1
        call    spurious_irq
        add     esp, 4
        hlt
%endmacro
; ---------------------------------

ALIGN   16
hwint00:                ; Interrupt routine for irq 0 (the clock).
        ;hwint_master    0
        iretd			; 什么都不干

ALIGN   16
hwint01:                ; Interrupt routine for irq 1 (keyboard)
        hwint_master    1

ALIGN   16
hwint02:                ; Interrupt routine for irq 2 (cascade!)
        hwint_master    2

ALIGN   16
hwint03:                ; Interrupt routine for irq 3 (second serial)
        hwint_master    3

ALIGN   16
hwint04:                ; Interrupt routine for irq 4 (first serial)
        hwint_master    4

ALIGN   16
hwint05:                ; Interrupt routine for irq 5 (XT winchester)
        hwint_master    5

ALIGN   16
hwint06:                ; Interrupt routine for irq 6 (floppy)
        hwint_master    6

ALIGN   16
hwint07:                ; Interrupt routine for irq 7 (printer)
        hwint_master    7

; ---------------------------------
%macro  hwint_slave     1
        push    %1
        call    spurious_irq
        add     esp, 4
        hlt
%endmacro
; ---------------------------------

ALIGN   16
hwint08:                ; Interrupt routine for irq 8 (realtime clock).
        hwint_slave     8

ALIGN   16
hwint09:                ; Interrupt routine for irq 9 (irq 2 redirected)
        hwint_slave     9

ALIGN   16
hwint10:                ; Interrupt routine for irq 10
        hwint_slave     10

ALIGN   16
hwint11:                ; Interrupt routine for irq 11
        hwint_slave     11

ALIGN   16
hwint12:                ; Interrupt routine for irq 12
        hwint_slave     12

ALIGN   16
hwint13:                ; Interrupt routine for irq 13 (FPU exception)
        hwint_slave     13

ALIGN   16
hwint14:                ; Interrupt routine for irq 14 (AT winchester)
        hwint_slave     14

ALIGN   16
hwint15:                ; Interrupt routine for irq 15
        hwint_slave     15







; 异常，软件中断
divide_error:
	push	0xFFFFFFFF		; no error code
	push	0				; vector_no = 0
	jmp		exception

single_step_exception:
	push	0xFFFFFFFF
	push	1
	jmp		exception

nmi:
	push	0xFFFFFFFF
	push	2
	jmp		exception
	
breakpoint_exception:
	push	0xFFFFFFFF
	push	3
	jmp		exception

overflow:
	push	0xFFFFFFFF
	push	4
	jmp		exception
	
bounds_check:
	push	0xFFFFFFFF
	push	5
	jmp		exception
	
inval_opcode:
	push	0xFFFFFFFF
	push	6
	jmp		exception

copr_not_available:
	push	0xFFFFFFFF
	push	7
	jmp		exception

double_fault:
	push	8
	jmp		exception
	
copr_seg_overrun:
	push	0xFFFFFFFF
	push	9
	jmp		exception

inval_tss:
	push	10
	jmp		exception

segment_not_present:
	push	11
	jmp		exception
	
stack_exception:
	push	12
	jmp		exception
	
general_protection:
	push	13
	jmp		exception
	
page_fault:
	push	14
	jmp		exception
	
copr_error:
	push	0xFFFFFFFF
	push	16
	jmp		exception


exception:
	call	exception_handler
	add		esp, 4*2
	jmp $
