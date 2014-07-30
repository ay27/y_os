// 本文件定义了一些用于进程处理的结构体和常量

#ifndef	_AY27_PROCESS_H_
#define	_AY27_PROCESS_H_

#include "const.h"
#include "protect.h"
#include "type.h"

// 堆栈片
typedef struct s_stackframe
{
	dword	gs;		/* \                                    */
	dword	fs;		/* |                                    */
	dword	es;		/* |                                    */
	dword	ds;		/* |                                    */
	dword	edi;		/* |                                    */
	dword	esi;		/* | pushed by save()                   */
	dword	ebp;		/* |                                    */
	dword	kernel_esp;	/* <- 'popad' will ignore it            */
	dword	ebx;		/* |                                    */
	dword	edx;		/* |                                    */
	dword	ecx;		/* |                                    */
	dword	eax;		/* /                                    */
	dword	retaddr;	/* return addr for kernel.asm::save()   */
	dword	eip;		/* \                                    */
	dword	cs;		/* |                                    */
	dword	eflags;		/* | pushed by CPU during interrupt     */
	dword	esp;		/* |                                    */
	dword	ss;		/* /                                    */
}STACK_FRAME;

// 进程表
typedef struct s_proc {
	STACK_FRAME regs;          /* process registers saved in stack frame */

	word ldt_sel;               /* gdt selector giving ldt base and limit */
	descriptor ldts[LDT_SIZE]; /* local descriptors for code and data */
	dword pid;                   /* process id passed in from MM */
	byte p_name[16];           /* name of the process */
}PROCESS;


/* Number of tasks */
#define NR_TASKS	1

/* stacks of tasks */
#define STACK_SIZE_TESTA	0x8000

#define STACK_SIZE_TOTAL	STACK_SIZE_TESTA

#endif
