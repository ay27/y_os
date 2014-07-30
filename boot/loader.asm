
		org		0x200				; 伪指令，指示编译器程序将装载到0x200位置，最大的作\
		用是使得常量和变量能放在正确的位置
		jmp	short	Code16_Start

%include "protect.inc"
;===============================================================================
; GDT
; descriptor(段界限32， 段基址32， 段属性16)
GDT_desc_Init:	Descriptor	0,		0, 		0
GDT_desc_Run:	Descriptor	0x0fffff, 0, DA_CR | DA_32
GDT_desc_Data:	Descriptor	0x0fffff, 0, DA_DRW | DA_32
GDT_desc_Video:	Descriptor	0x0ffff, 0x0B8000, DA_DRW | DA_DPL3

GDT_Length		equ		$ - GDT_desc_Init
GDT_PTR			dw		GDT_Length - 1
				dd		Loader_PhyAddr + GDT_desc_Init
				
Selector_Run		equ		GDT_desc_Run - GDT_desc_Init
Selector_Data		equ		GDT_desc_Data - GDT_desc_Init
Selector_Video		equ		GDT_desc_Video - GDT_desc_Init


[section	.s16]
[bits	16]
Code16_Start:
		
		mov ax, cs
		mov ds, ax
		mov es, ax
		mov ss, ax
		mov sp, 0x100
		
		mov al, '!'
		call Code16_DispChar
		
		; es:bx指向kernel将被转载的位置
		mov ax, Kernel_BaseAddr
		mov es, ax
		mov bx, Kernel_Offset
		
		xor ecx, ecx
		mov cx, RootDir_Sectors
		
		mov ax, RootDir_No1Sector
		mov [ReadSector_No], ax
		
Label_Search_Dir:
		mov dl, 1
		call Code16_ReadSector
		
		mov si, Kernel_FileName
		mov di, Kernel_Offset
		mov dx, Kernel_FileName_Length
		
		push cx
		mov cx, 0x10
		
	Label_Search_Sector:
		call Code16_CMP_String
		cmp ax, 0
		jz Label_File_Found
		
		add di, 0x20
	loop Label_Search_Sector
	
		pop cx
		inc word [ReadSector_No]
loop Label_Search_Dir
		
		; 没有找到文件
		mov dx, 0x0500
		mov bp, Disp_Str_FileNotFound
		call Code16_DispStr

		jmp $
		
Label_File_Found:
		
		add di, 0x1A
		
		mov ax, Kernel_BaseAddr
		mov es, ax
		mov bx, Kernel_Offset
		
		mov ax, [es:di]
		push ax
		
	.loop:
		mov al, '~'
		call Code16_DispChar
		
		pop ax
		push ax
		add ax, Data_No1Sector-2
		mov dl, 1
		call Code16_ReadSector
		
		pop ax
		call Code16_Get_FATEntry
		
		cmp ax, 0xFFF
		jz Label_LoadFinish
		
		push ax
		add bx, [BPB_BytePerSec]
	jmp .loop
	
Label_LoadFinish:
		mov bp, Disp_Str_LoadFinish
		push dx
		mov dx, 0x0500
		call Code16_DispStr
		pop dx
		
		; 关闭软驱
		call Code16_KillMotor
		
		lgdt	[GDT_PTR]
		
		cli
		
		in al, 0x92
		or al, 2
		out 0x92, al
		
		mov eax, cr0
		or eax, 1
		mov cr0, eax
		
		jmp dword Selector_Run:(Loader_PhyAddr+Label_PM_Start)
		

Code16_KillMotor:
		push dx
		mov dx, 0x3F2
		mov al, 0
		out dx, al
		pop dx
		ret		

%include "FAT12.inc"
%include "fun16.inc"


;===============================================================================		

Disp_Str_Length			equ		11
Disp_Str_Loading:		db		'IN LOADER  '
Disp_Str_FileNotFound:	db		' NO FILE!!!'
Disp_Str_LoadFinish:	db		'Load Finish'

Loader_BaseAddr			equ		0x9000
Loader_Offset			equ		0x200
Loader_PhyAddr			equ		Loader_BaseAddr*0x10

Kernel_BaseAddr			equ		0x8000
Kernel_Offset			equ		0
Kernel_PhyAddr			equ		Kernel_BaseAddr*0x10
Kernel_FileName:		db		'KERNEL  BIN'
Kernel_FileName_Length	equ		11
Kernel_EntryPoint		equ		0x30400

RootDir_Sectors			equ		14
RootDir_No1Sector		equ		19
FAT_No1Sector			equ		1
Data_No1Sector			equ		33

; 变量
ReadSector_No:			dw		0
bOdd:					db		0

;===============================================================================
; 32位模式

[section	.s32]
ALIGN	32
[bits	32]
Label_PM_Start:
		mov ax, Selector_Video
		mov gs, ax
		
		mov ax, Selector_Data
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov ss, ax
		mov esp, TopOfStack
		
		mov edi, (80*3+0)*2
		mov ah, 0001010b
		mov al, 'P'
		mov [gs:edi], ax
		
		call Code32_InitKernel
		
		jmp Selector_Run:Kernel_EntryPoint


		
;===============================================================================

; 将kernel重新放置，按照ELF的program header中的值
Code32_InitKernel:
		
		mov cx, [Kernel_PhyAddr + 0x2C]		; 指示有多少个program header
		
		movzx ecx, cx
		mov esi, [Kernel_PhyAddr + 0x1C]	; 指示program header在文件中的偏移量
		add esi, Kernel_PhyAddr				; 真实的物理地址
		
	.loop:
		mov eax, [esi+0]
		cmp eax, 0
		jz .Nothing			; 如果p_type=0,则跳过
		
		; 下面开始设置MemCpy函数的入口地址
		; void MemCpy(P_Vaddr(目标地址), 原地址, size)
		mov eax, [esi+0x10]
		cmp eax, 0
		jz .Nothing
		
		push dword [esi+0x10]
		
		mov eax, [esi + 0x4]
		add eax, Kernel_PhyAddr
		push eax
		
		push dword [esi+ 0x8]
		
		call MemCpy
		
		; 抛弃刚刚入栈的几个参数
		pop eax
		pop eax
		pop eax
		
	.Nothing:
		add esi, 0x20
	loop .loop
	
		ret
		
%include "fun32.inc"

StackSpace:		times	1024	db		0
TopOfStack		equ		Loader_PhyAddr+$
