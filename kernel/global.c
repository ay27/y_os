// 本文件定义了全局变量，然后需要在global.h中extern


#include "const.h"
#include "fun_define.h"
#include "protect.h"
#include "type.h"
#include "process.h"

byte gdt_ptr[6];
byte idt_ptr[6];
descriptor gdt[GDT_SIZE];
gate	idt[IDT_SIZE];
PROCESS proc_table[NR_TASKS];
byte task_stack[STACK_SIZE_TOTAL];
TSS tss;
PROCESS* p_proc_ready;

