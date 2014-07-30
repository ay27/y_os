//本文件定义了系统在保护模式下需要用到的参数和结构
//特别是中断的各种常量定义


#ifndef		_AY27_PROTECT_H_
#define		_AY27_PROTECT_H_

#include "type.h"

typedef struct s_descriptor
{
	word	limit_low;		//limit
	word	base_low;		//base
	byte	base_mid;		//base
	byte	attr1;			//p(1)  DPL(2)  DT(1)  TYPE(4)
	byte	limit_high_attr2;	//G(1) D(1) 0(1) AVL(1) LimitHigh(4)
	byte	base_high;		//base
}descriptor;

typedef struct s_gate
{
	word	offset_low;
	word	selector;
	byte	dcount;			//只在调用门中生效，当堆栈改变时，用此参数指示需要复制\
							  的参数的个数
	byte	attr;			// P(1) DPL(2) DT(1) TYPE(4)
	word	offset_high;
}gate;

typedef struct s_tss {
	dword	backlink;
	dword	esp0;	/* stack pointer to use during interrupt */
	dword	ss0;	/*   "   segment  "  "    "        "     */
	dword	esp1;
	dword	ss1;
	dword	esp2;
	dword	ss2;
	dword	cr3;
	dword	eip;
	dword	flags;
	dword	eax;
	dword	ecx;
	dword	edx;
	dword	ebx;
	dword	esp;
	dword	ebp;
	dword	esi;
	dword	edi;
	dword	es;
	dword	cs;
	dword	ss;
	dword	ds;
	dword	fs;
	dword	gs;
	dword	ldt;
	word	trap;
	word	iobase;	/* I/O位图基址大于或等于TSS段界限，就表示没有I/O许可位图 */
}TSS;


/* GDT */
/* 描述符索引 */
#define	INDEX_DUMMY		0	/* \                         */
#define	INDEX_FLAT_C		1	/* | LOADER 里面已经确定了的 */
#define	INDEX_FLAT_RW		2	/* |                         */
#define	INDEX_VIDEO		3	/* /                         */
#define	INDEX_TSS		4
#define	INDEX_LDT_FIRST		5
/* 选择子 */
#define	SELECTOR_DUMMY		   0	/* \                         */
#define	SELECTOR_FLAT_C		0x08	/* | LOADER 里面已经确定了的 */
#define	SELECTOR_FLAT_RW	0x10	/* |                         */
#define	SELECTOR_VIDEO		(0x18+3)/* /<-- RPL=3                */
#define	SELECTOR_TSS		0x20	/* TSS                       */
#define SELECTOR_LDT_FIRST	0x28

#define	SELECTOR_KERNEL_CS	SELECTOR_FLAT_C
#define	SELECTOR_KERNEL_DS	SELECTOR_FLAT_RW
#define	SELECTOR_KERNEL_GS	SELECTOR_VIDEO

/* 每个任务有一个单独的 LDT, 每个 LDT 中的描述符个数: */
#define LDT_SIZE		2


/* 宏 */
/* 线性地址 → 物理地址 */
#define vir2phys(seg_base, vir)	(dword)(((dword)seg_base) + (dword)(vir))





// 8259A 中断控制器的端口等数据定义
// Master主片
#define		int_m_ctl		0x20		// io port
#define		int_m_ctlmask	0x21		// value
// Slave从片
#define		int_s_ctl		0xA0		// io port
#define		int_s_ctlmask	0xA1		// value
// 中断向量
#define		int_vector_IRQ0	0x20
#define		int_vector_IRQ8	0x28


/* 中断向量 */
#define		int_vector_divide			0x0
#define		int_vector_debug			0x1
#define		int_vector_nmi				0x2
#define		int_vector_breakpoint		0x3
#define		int_vector_overflow			0x4
#define		int_vector_bounds			0x5
#define		int_vector_inval_op			0x6
#define		int_vector_coproc_not		0x7
#define		int_vector_double_fault		0x8
#define		int_vector_coproc_seg		0x9
#define		int_vector_inval_tss		0xa
#define		int_vector_seg_not			0xb
#define		int_vector_stack_fault		0xc
#define		int_vector_protection		0xd
#define		int_vector_page_fault		0xe
#define		int_vector_coproc_err		0x10


/* 描述符类型值说明 */
// DA_ : Descriptor Attribute
#define	DA_32			0x4000	/* 32 位段				*/
#define	DA_LIMIT_4K		0x8000	/* 段界限粒度为 4K 字节			*/
#define	DA_DPL0			0x00	/* DPL = 0				*/
#define	DA_DPL1			0x20	/* DPL = 1				*/
#define	DA_DPL2			0x40	/* DPL = 2				*/
#define	DA_DPL3			0x60	/* DPL = 3				*/
/* 存储段描述符类型值说明 */
#define	DA_DR			0x90	/* 存在的只读数据段类型值		*/
#define	DA_DRW			0x92	/* 存在的可读写数据段属性值		*/
#define	DA_DRWA			0x93	/* 存在的已访问可读写数据段类型值	*/
#define	DA_C			0x98	/* 存在的只执行代码段属性值		*/
#define	DA_CR			0x9A	/* 存在的可执行可读代码段属性值		*/
#define	DA_CCO			0x9C	/* 存在的只执行一致代码段属性值		*/
#define	DA_CCOR			0x9E	/* 存在的可执行可读一致代码段属性值	*/
/* 系统段描述符类型值说明 */
#define	DA_LDT			0x82	/* 局部描述符表段类型值			*/
#define	DA_TaskGate		0x85	/* 任务门类型值				*/
#define	DA_386TSS		0x89	/* 可用 386 任务状态段类型值		*/
#define	DA_386CGate		0x8C	/* 386 调用门类型值			*/
#define	DA_386IGate		0x8E	/* 386 中断门类型值			*/
#define	DA_386TGate		0x8F	/* 386 陷阱门类型值			*/

/* 选择子类型值说明 */
/* 其中, SA_ : Selector Attribute */
#define	SA_RPL_MASK	0xFFFC
#define	SA_RPL0		0
#define	SA_RPL1		1
#define	SA_RPL2		2
#define	SA_RPL3		3

#define	SA_TI_MASK	0xFFFB
#define	SA_TIG		0
#define	SA_TIL		4


/* 权限 */
#define	PRIVILEGE_KRNL	0
#define	PRIVILEGE_TASK	1
#define	PRIVILEGE_USER	3
/* RPL */
#define	RPL_KRNL	SA_RPL0
#define	RPL_TASK	SA_RPL1
#define	RPL_USER	SA_RPL3


#endif
