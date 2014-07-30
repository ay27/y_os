
//本文件主要实现中断和异常的初始化，以及软件异常的处理


#include "global.h"

extern gate idt[IDT_SIZE];

// 该函数只用于本文件

static void init_idt_desc(byte vector, 
					int_handler handler, byte privilege);
static void init_descriptor(descriptor* p_desc, dword base, dword limit, word attribute);

// 中断处理函数，链接到protect_asm.inc文件中
void	divide_error();
void	single_step_exception();
void	nmi();
void	breakpoint_exception();
void	overflow();
void	bounds_check();
void	inval_opcode();
void	copr_not_available();
void	double_fault();
void	copr_seg_overrun();
void	inval_tss();
void	segment_not_present();
void	stack_exception();
void	general_protection();
void	page_fault();
void	copr_error();
	void    hwint00();
	void    hwint01();
	void    hwint02();
	void    hwint03();
	void    hwint04();
	void    hwint05();
	void    hwint06();
	void    hwint07();
	void    hwint08();
	void    hwint09();
	void    hwint10();
	void    hwint11();
	void    hwint12();
	void    hwint13();
	void    hwint14();
	void    hwint15();


void init_prot()
{
	init_8259A();
	init_idt_desc(int_vector_divide, divide_error, PRIVILEGE_KRNL);
	init_idt_desc(int_vector_debug, single_step_exception, PRIVILEGE_KRNL);
	init_idt_desc(int_vector_nmi, nmi, PRIVILEGE_KRNL);
	init_idt_desc(int_vector_breakpoint, breakpoint_exception, PRIVILEGE_USER);
	init_idt_desc(int_vector_overflow, overflow, PRIVILEGE_USER);
	init_idt_desc(int_vector_bounds, bounds_check, PRIVILEGE_KRNL);
	init_idt_desc(int_vector_inval_op, inval_opcode, PRIVILEGE_KRNL);
	init_idt_desc(int_vector_coproc_not, copr_not_available, PRIVILEGE_KRNL);
	init_idt_desc(int_vector_double_fault, double_fault, PRIVILEGE_KRNL);
	init_idt_desc(int_vector_coproc_seg, copr_seg_overrun, PRIVILEGE_KRNL);
	init_idt_desc(int_vector_inval_tss, inval_tss, PRIVILEGE_KRNL);
	init_idt_desc(int_vector_seg_not, segment_not_present, PRIVILEGE_KRNL);
	init_idt_desc(int_vector_stack_fault, stack_exception, PRIVILEGE_KRNL);
	init_idt_desc(int_vector_protection, general_protection, PRIVILEGE_KRNL);
	init_idt_desc(int_vector_page_fault, page_fault, PRIVILEGE_KRNL);
	init_idt_desc(int_vector_coproc_err, copr_error, PRIVILEGE_KRNL);
	
		init_idt_desc(int_vector_IRQ0+0, hwint00, PRIVILEGE_KRNL);
		init_idt_desc(int_vector_IRQ0+1, hwint01, PRIVILEGE_KRNL);
		init_idt_desc(int_vector_IRQ0+2, hwint02, PRIVILEGE_KRNL);
		init_idt_desc(int_vector_IRQ0+3, hwint03, PRIVILEGE_KRNL);
		init_idt_desc(int_vector_IRQ0+4, hwint04, PRIVILEGE_KRNL);
		init_idt_desc(int_vector_IRQ0+5, hwint05, PRIVILEGE_KRNL);
		init_idt_desc(int_vector_IRQ0+6, hwint06, PRIVILEGE_KRNL);
		init_idt_desc(int_vector_IRQ0+7, hwint07, PRIVILEGE_KRNL);
		
			init_idt_desc(int_vector_IRQ8+0, hwint08, PRIVILEGE_KRNL);
			init_idt_desc(int_vector_IRQ8+1, hwint09, PRIVILEGE_KRNL);
			init_idt_desc(int_vector_IRQ8+2, hwint10, PRIVILEGE_KRNL);
			init_idt_desc(int_vector_IRQ8+3, hwint11, PRIVILEGE_KRNL);
			init_idt_desc(int_vector_IRQ8+4, hwint12, PRIVILEGE_KRNL);
			init_idt_desc(int_vector_IRQ8+5, hwint13, PRIVILEGE_KRNL);
			init_idt_desc(int_vector_IRQ8+6, hwint14, PRIVILEGE_KRNL);
			init_idt_desc(int_vector_IRQ8+7, hwint15, PRIVILEGE_KRNL);
			
	// 填充GDT中的TSS描述符
	memset(&tss, 0, sizeof(tss));
	tss.ss0 = SELECTOR_KERNEL_DS;
	init_descriptor(&gdt[INDEX_TSS], vir2phys(seg2phys(SELECTOR_KERNEL_DS), &tss),
		sizeof(tss)-1, DA_386TSS);
	tss.iobase = sizeof(tss);	// 没有io位图
	
	// 填充GDT中的LDT描述符
	init_descriptor(&gdt[INDEX_LDT_FIRST], vir2phys(seg2phys(SELECTOR_KERNEL_DS), proc_table[0].ldts),
		LDT_SIZE*sizeof(descriptor)-1, DA_LDT);
}


static void init_idt_desc(byte vector, 
					int_handler handler, byte privilege)
{
	gate* p_gate = &idt[vector];
	dword base = (dword) handler;
	p_gate->offset_low	= base & 0xFFFF;
	p_gate->selector 	= Selector_Kernel_CS;
	p_gate->dcount		= 0;
	p_gate->attr			= DA_386IGate | (privilege << 5);
	p_gate->offset_high	= (base>>16) & 0xFFFF;
}

static void init_descriptor(descriptor* p_desc, dword base, dword limit, word attribute)
{
	p_desc->limit_low 	= limit & 0xFFFF;
	p_desc->base_low	= base & 0xFFFF;
	p_desc->base_mid	= (base>>16) & 0xFF;
	p_desc->attr1		= attribute & 0xFF;
	p_desc->limit_high_attr2= ((limit>>16) & 0xF) | ((attribute>>8) & 0xF0);
	p_desc->base_high	= (base>>24) & 0xFF;
}

// 由段名求绝对地址
dword seg2phys(word seg)
{
	descriptor* p_dest = &gdt[seg >> 3];
	return (p_dest->base_high<<24 | p_dest->base_mid<<16 | p_dest->base_low);
}

void exception_handler(int vec_no,int err_code,int eip,int cs,int eflags)
{
	int i, j;
	int text_color = 0x74; /* 灰底红字 */

	char * err_msg[] = {"#DE Divide Error",
			    "#DB RESERVED",
			    "--  NMI Interrupt",
			    "#BP Breakpoint",
			    "#OF Overflow",
			    "#BR BOUND Range Exceeded",
			    "#UD Invalid Opcode (Undefined Opcode)",
			    "#NM Device Not Available (No Math Coprocessor)",
			    "#DF Double Fault",
			    "    Coprocessor Segment Overrun (reserved)",
			    "#TS Invalid TSS",
			    "#NP Segment Not Present",
			    "#SS Stack-Segment Fault",
			    "#GP General Protection",
			    "#PF Page Fault",
			    "--  (Intel reserved. Do not use.)",
			    "#MF x87 FPU Floating-Point Error (Math Fault)",
			    "#AC Alignment Check",
			    "#MC Machine Check",
			    "#XF SIMD Floating-Point Exception"
	};

	/* 通过打印空格的方式清空屏幕的前五行，并把 disp_pos 清零 */
	for (i=0; i<5; i++)
		for (j=0; j<80; j++)
			print(" ", 1, j, i, text_color);
	print("Exception: ", 11, 0, 0, text_color);
	print(err_msg[vec_no], sizeof(err_msg[vec_no]), 12, 0, text_color);
	print("EFLAGS=", 7, 0, 2, text_color);
	printNum(eflags, 9, 7, 2, text_color);
	print("CS=", 3, 0, 3, text_color);
	printNum(cs, 9, 3, 3, text_color);
	print("EIP=", 4, 0, 4, text_color);
	printNum(eip, 9, 4, 4, text_color);

	if(err_code != 0xFFFFFFFF){
		print("Error code: ", 12, 0, 5, text_color);
		printNum(err_code, 9, 12, 5, text_color);
	}
}

