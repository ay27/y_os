
; 本文件定义了一些在C中需要用到的汇编函数

global debug
global out_port
global in_port


;=======================================================================
; 用于方便调试的一个函数
debug:
	xchg bx, bx
	ret
	
	
	
;=======================================================================
; 用于处理端口输入输出的两个函数
; void out_port(word port, byte value)
out_port:
	
	mov edx, [esp+4]
	mov al, [esp+8]
	out dx, al
	nop			; a little delay
	nop
	ret
	
; byte in_port(word port)
in_port:
	
	mov edx, [esp+4]
	xor eax, eax
	in  al, dx
	nop
	nop
	ret
	
	
;=======================================================================
