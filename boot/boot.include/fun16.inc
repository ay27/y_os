		
;===============================================================================



	;INT 10H
	;功能 06H 和 07H
	;功能描述:初始化屏幕或滚屏
	;入口参数:AH=06H——向上滚屏,07H——向下滚屏
	;AL=滚动行数(0——清窗口)
	;BH=空白区域的缺省属性
	;(CH、CL)=窗口的左上角位置(Y 坐标,X 坐标)
	;(DH、DL)=窗口的右下角位置(Y 坐标,X 坐标)
	;出口参数:无
Code16_CleanScreen:

		push ax
		push bx
		push cx
		push dx
		
		mov ax, 0x0600
		mov bx, 0x700
		mov cx, 0
		mov dx, 0x184f
		int 0x10
		
		pop dx
		pop cx
		pop bx
		pop ax
		
		ret

;===============================================================================
	;INT 10H
	;功能 13H
	;功能描述:在 Teletype 模式下显示字符串
	;入口参数:AH=13H
	;BH=页码
	;BL=属性(若 AL=00H 或 01H)
	;CX=显示字符串长度
	;(DH、DL)=坐标(行、列)
	;ES:BP=显示字符串的地址 AL=显示输出方式0——字符串中只含显示字符,其显示属性在 BL 中。显示后,光标位置不变
	;1——字符串中只含显示字符,其显示属性在 BL 中。显示后,光标位置改变
	;2——字符串中含显示字符和显示属性。显示后,光标位置不变
	;3——字符串中含显示字符和显示属性。显示后,光标位置改变
	;出口参数:无
	
; 入口参数：ds:bp指向字符串首地址, dx显示的位置
Code16_DispStr:

		push ax
		push bx
		push cx
		push dx
		push es
		
		
		mov ax, ds
		mov es, ax
		
		mov cx, Disp_Str_Length
		mov bx, 00001100b
		mov ax, 0x1301
		int 0x10

		pop es
		pop dx
		pop cx
		pop bx
		pop ax
		
		ret
		
;===============================================================================
	;INT 13H
	;功能 02H
	;功能描述:读扇区
	;入口参数:AH=02H
	;AL=扇区数CH=柱面
	;CL=扇区
	;DH=磁头
	;DL=驱动器,00H~7FH:软盘;80H~0FFH:硬盘
	;ES:BX=缓冲区的地址
	;出口参数:CF=0——操作成功,AH=00H,AL=传输的扇区数,否则,AH=状态代码
	
	; 入口参数：ax=需要读取的扇区号，dl=需要读取的扇区个数
	; 出口参数：将读到的数据存储到es:bx中
Code16_ReadSector:
		; 由扇区号求得扇区在磁盘中的位置：
		; 设扇区号为X
		; X / 每磁道扇区数 = Y ...... Z
		; 柱面 = Y>>1	磁头号 = Y&1
		; 起始扇区号 = Z+1
		
		push ax
		push bx
		push cx
		push dx
		
		push bx
		push dx
		
		mov bl, [BPB_SecPerTrk]
		div bl						; ax/bl=al......ah
									; al=Y		ah=Z
		
		inc ah
		mov cl, ah
		
		mov dh, al
		and dh, 1
		
		shr al, 1
		mov ch, al
		
		mov dl, [BS_DrvNum]
		
		pop bx
		mov al, bl
		mov ah, 2
		
		pop bx
		
	.loop:
		int 0x13
		jc .loop
		
		
		pop dx
		pop cx
		pop bx
		pop ax
		
		ret
		
;===============================================================================

; CMP函数，用于比较两个字符串是否相等
; lodsb,  al = ds:si，si++
; 入口参数：ds:si 和 es:di指向需要进行比较的两个字符串首地址, 字符串长度=dx
; 出口参数：ax = 0, 相等
;		  ax = 1, 不相等
Code16_CMP_String:

		push cx
		push di
		push si
		
		cld
		
		mov cx, dx
		
	.loop:
		lodsb					;al = ds:si，si++
		cmp al, byte [es:di]
		jnz .fail
		
		inc di
		loop .loop
		
		; 成功
		mov ax, 0
		jmp .ret
		
	.fail:
		mov ax, 1
		
	.ret:
		pop si
		pop di
		pop cx
		ret
		
;===============================================================================

;INT 10H
;功能 0EH
;功能描述:在 Teletype 模式下显示字符
;入口参数:AH=0EH
;AL=字符
;BH=页码
;BL=前景色(图形模式)
;出口参数:无

; 打一个点, 入口参数：al=字符
Code16_DispChar:

		push ax
		push bx
		
		mov ah, 0xE
		;mov al, '~'
		mov bh, 0
		mov bl, 0001010b
		int 0x10

		pop bx
		pop ax
		
		ret
		
;===============================================================================
; 找到序号为ax的sector在FAT表中的条目，结果放在ax中
; 需要把FAT表读取到es:bx中
; 注意此时的ax只是簇号
Code16_Get_FATEntry:

		push es
		push bx
		push ax
		
		mov ax, Loader_BaseAddr
		sub ax, 0x100				; 在loader后留出4k空间
		mov es, ax
		
		mov byte[bOdd], 0
		
		; 计算ax在FAT中的偏移
		; FAT表中一个项占12位，即3/2个字节
		; ax是簇号，从2开始
		pop ax
		mov bx, 3
		mul bx					; ax*3 = ax
		mov bx, 2
		div bx					; ax / 2 = ax ...... dx
		cmp dx, 0
		jz Label_Even			; 为偶数
		mov byte[bOdd], 1		; 为奇数 
		
	; 现在ax已经是FAT中的偏移量，下面计算在哪个扇区中
	Label_Even:
		xor dx, dx
		mov bx, [BPB_BytePerSec]
		div bx					; ax / bx = ax ......dx
		
		push dx
		
		add ax, FAT_No1Sector
		
		mov bx, 0
		mov dl, 2				; 一次读取两个扇区，避免发生边界错误
		call Code16_ReadSector
		
		pop dx
		
		add bx, dx
		mov ax, [es:bx]
		
		cmp byte [bOdd], 0
		jz Label_Even_2
		shr ax, 4
		
	Label_Even_2:				; 偶数
		and ax, 0xFFF
		
		pop bx
		pop es
		ret
		
;===============================================================================


		
