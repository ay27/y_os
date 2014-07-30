
// 本文件初始化了8259A芯片，以及硬件中断的处理函数


#include "global.h"

void init_8259A()
{
	/* 初始化8259A的过程：（顺序不能颠倒）
	 * 1.往端口0x20(主片)或0xA0(从片) 写入ICW1
	 * 2.往端口0x21(主片)或0xA1(从片) 写入ICW2
	 * 3.往端口0x21(主片)或0xA1(从片) 写入ICW3
	 * 4.往端口0x21(主片)或0xA1(从片) 写入ICW4
	 * ICW具体看书P92
	 * ICW: Initialization Command Word
	 * OCW: Operation Control Word
	 * */
	out_port(int_m_ctl,		0x11);
	out_port(int_s_ctl,		0x11);
	
	out_port(int_m_ctlmask,	int_vector_IRQ0);
	out_port(int_s_ctlmask,	int_vector_IRQ8);
	
	out_port(int_m_ctlmask,	0x4);
	out_port(int_s_ctlmask, 0x2);
	
	out_port(int_m_ctlmask,	0x1);
	out_port(int_s_ctlmask, 0x1);
	
	// OCW
	// 0xFF表示关闭全部中断
	out_port(int_m_ctlmask,	0xfd);
	out_port(int_s_ctlmask,	0xff);
}


void spurious_irq(dword irq)
{
	print("spurious_irq: ", 14, 30, 0, color_green);
	printNum(irq, 9, 44, 0, color_green);
}
