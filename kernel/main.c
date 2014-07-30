// 本文件是操作系统的入口，也是第一个进程的入口

#include "global.h"


void kernel_main()
{
	byte str1[] = "------kernel main------";
	// 清屏
	clean_screen();
	print(str1, sizeof(str1)-1, 0, 0, color_red);
	
	
	PROCESS* p_proc = proc_table;
	
	p_proc->ldt_sel = SELECTOR_LDT_FIRST;
	memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS>>3], sizeof(descriptor));
	p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5;	// change the DPL
	memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS>>3], sizeof(descriptor));
	p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5; // change the DPL
	
	p_proc->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
	p_proc->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
	p_proc->regs.es = p_proc->regs.fs = p_proc->regs.ss = p_proc->regs.ds;
	p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | RPL_TASK;
	p_proc->regs.eip = (dword) TestA;
	p_proc->regs.esp = (dword) task_stack + STACK_SIZE_TOTAL;
	p_proc->regs.eflags = 0x1202;
	
	p_proc_ready = proc_table;
	restart();
	for (;;) {}
}




void TestA()
{
	byte str1[] = "------kernel main------";
	// 清屏
	clean_screen();
	print(str1, sizeof(str1)-1, 0, 0, color_red);
	
	dword i=0;
	byte color = 0x7C;		// 灰底高亮红字
	while(1)
	{
		print("A", 1, i++, 1, color);
		printNum(i, 3, i, 1, color);
		i+=3;
		print(".", 1, i++, 1, color);
		delay(1000);
	}
	
}
