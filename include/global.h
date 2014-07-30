// 本文件extern了全局变量，也include了其他所有的头文件，方便进行include


#ifndef _AY27_GLOBAL_H_
#define	_AY27_GLOBAL_H_

#include "const.h"
#include "fun_define.h"
#include "protect.h"
#include "type.h"
#include "process.h"

extern byte gdt_ptr[6];
extern byte idt_ptr[6];
extern descriptor gdt[GDT_SIZE];
extern gate	idt[IDT_SIZE];
extern PROCESS proc_table[NR_TASKS];
extern byte task_stack[STACK_SIZE_TOTAL];
extern TSS tss;
extern PROCESS* p_proc_ready;


#endif
