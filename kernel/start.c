
#include "global.h"


void cstart()
{
	
	// 将旧的GDT通过gdt_ptr复制到新的位置
	memcpy(&gdt, (byte*)(*((dword*)(&gdt_ptr[2]))), *((word*)(&gdt_ptr[0]))+1);
	// 设置重新加载GDT的参数，即新的gdt_ptr，0-15：limit，16-27:base
	word* gdt_limit = (word*)(&gdt_ptr[0]);
	dword* gdt_base = (dword*)(&gdt_ptr[2]);
	*gdt_limit = GDT_SIZE * sizeof(descriptor) - 1;
	*gdt_base  = (dword)&gdt;
	
	word* idt_limit = (word*)(&idt_ptr[0]);
	dword* idt_base = (dword*)(&idt_ptr[2]);
	*idt_limit = IDT_SIZE * sizeof(gate) - 1;
	*idt_base  = (dword)&idt;
	
	// 初始化中断和异常
	init_prot();
	
	/*debug();
	byte temp[10];
	num2str((dword*)gdt_ptr[2], temp);
	print("gdt_ptr[2]\n", 11, 0, 10, color_blue);
	printNum((dword*)gdt_ptr[2], 12, 10, color_red);

	
	num2str(&gdt_ptr, temp);
	print(temp, sizeof(temp)-1, 0, 11, color_green);
	
	*/
	debug();
	
}



