; 这个文件中的代码都是32位模式下运行的
; 基本作为被C程序调用的子程序
; 一般默认用堆栈进行参数传递



;====================================================================================
; 必须从这里声明时全局变量，然后在kernel中include这个文件，
; 这样才能在C中调用这些函数
global MemCpy
global DispStr



;=====================================================================================
; MemCpy
; void Code32_MemCpy(目标地址，原地址， int size)
; 数据存储在堆栈中
MemCpy:
		push esi
		push edi
		push ecx

		mov edi, [esp+16]
		mov esi, [esp+20]
		mov ecx, [esp+24]

		cld			; 正向传输

	.loop:
		lodsb			; al = ds:si, si++
		stosb			; ds:di = al, di++
	loop .loop

		pop ecx
		pop edi
		pop esi

		ret


;===============================================================================================
; void DispStr(char * string, int pos, int size)
; 显示一个字符串
DispStr:
		push esi
		push edi
		push ecx

		mov esi, [esp+16]
		mov edi, [esp+20]
		mov ecx, [esp+24]

		mov ah, 00001100b

		cld

	.loop:
		lodsb
		mov [gs:edi], ax
		add edi, 2
	loop .loop

		pop ecx
		pop edi
		pop esi

		ret



;=========================================================================================




