		org 	0x7c00
		jmp 	Code16_Start
		nop

%include "FAT12.inc"
;===============================================================================


Code16_Start:
		mov ax, cs
		mov ds, ax
		mov es, ax
		mov ss, ax
		mov sp, Stack_Top
		
		call Code16_CleanScreen
		
		mov bp, Disp_Str_Booting
		mov dx, 0x0100
		call Code16_DispStr
		
		;软驱复位
			;INT 13H
			;功能 00H
			;功能描述:磁盘系统复位
			;入口参数:AH=00H
			;DL=驱动器,00H~7FH:软盘;80H~0FFH:硬盘
			;出口参数:CF=0——操作成功,AH=00H,否则,AH=状态代码
		xor ah, ah
		xor dl, dl
		int 0x13
		
		
		; 下面准备开始载入loader文件
		; 载入的目标内存地址为es:bx = Loader_BaseAddr:Loader_Offset
		mov ax, Loader_BaseAddr
		mov es, ax
		mov bx, Loader_Offset
		
		xor ecx, ecx
		mov cx, RootDir_Sectors
		
		mov ax, RootDir_No1Sector
		mov [ReadSector_No], ax
		
Label_Search_Dir:
		; dl 指向需要读取的扇区个数
		mov dl, 1
		call Code16_ReadSector
		
		; 设置CMP函数的入口参数
		; 入口参数：ds:si 和 es:di指向需要进行比较的两个字符串首地址, 字符串长度=dx
		mov si, Loader_FileName
		mov di, Loader_Offset
		mov dx, Loader_FileName_Length
		
		push cx
		mov cx, 0x10		; 512 / 32 = 16
		
		; 在刚刚读取到的sector中寻找文件头
	Label_Search_Sector:
		call Code16_CMP_String
		cmp ax, 0
		jz	Label_File_Found
		
		; 没有找到，继续寻找下一个文件头
		add di, 0x20
	loop Label_Search_Sector
	
		; 在该扇区中没有找到文件，则返回继续读取下一个扇区
		pop cx
		inc word [ReadSector_No]
loop Label_Search_Dir
		
		; 不存在该文件
		mov bp, Disp_Str_FileNotFound
		mov dx, 0x0200
		call Code16_DispStr
		jmp $
		

; 找到文件头，需要读取FAT表，把整个文件读取到目标地址
Label_File_Found:
		; 目前，es:di指向了该文件头在根目录下的位置
		; 下面需要完成的任务：
		; 1.从文件头取出文件对应的簇号；
		; 2.从FAT表中读取相应扇区到目标地址
		; 3.直到FAT表记录的内容是0xFFF，则读取完成
		
		
		add di, 0x1A		; 指向簇号
		
		; 设置es:bx指向目标地址
		mov ax, Loader_BaseAddr
		mov es, ax
		mov bx, Loader_Offset
		
		; 设置ax，指向文件的第一个簇号，通过计算得出该簇号所在的数据扇区，然后用ReadSector\
		读取相应扇区的内容
		mov ax, [es:di]
		push ax
		
	.loop:
		mov al, '~'
		Call Code16_DispChar				; 打一个点
		pop ax
		push ax
		add ax, Data_No1Sector - 2		; 得到真实的扇区号
		mov dl, 1
		call Code16_ReadSector
		
		pop ax
		call Code16_Get_FATEntry		; 获取下一个簇号
		
		cmp ax, 0x0FFF
		jz Label_LoadFinish
		
		push ax
		add bx, [BPB_BytePerSec]
	jmp .loop
		
Label_LoadFinish:
		mov bp, Disp_Str_LoadFinish
		push dx
		mov dx, 0x200
		call Code16_DispStr
		pop dx
		
		
		jmp Loader_BaseAddr:Loader_Offset




%include "fun16.inc"	
;===============================================================================



; 注意！！！！！
; 此处的定义必须放在程序后边，放在一开始会报告“文件系统错误”

; 常量定义
Disp_Str_Length			equ		11
Disp_Str_Booting:		db		' Booting...'
Disp_Str_FileNotFound:	db		' NO FILE!!!'
Disp_Str_LoadFinish:	db		'Load Finish'

Loader_BaseAddr			equ		0x9000
Loader_Offset			equ		0x200
Loader_PhyAddr			equ		Loader_BaseAddr*0x10
Loader_FileName_Length	equ		11
Loader_FileName:		db		'LOADER  BIN'

RootDir_Sectors			equ		14
RootDir_No1Sector		equ		19
FAT_No1Sector			equ		1
Data_No1Sector			equ		33

Stack_Top			equ		0x7c00

; 变量
ReadSector_No:			dw		0
bOdd:					db		0

times	510-($-$$)	db	0
dw	0xaa55
