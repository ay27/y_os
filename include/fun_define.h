
// 本文件对所有已实现且全局的函数进行声明

#ifndef		_AY27_FUN_DEFINE_H_
#define		_AY27_FUN_DEFINE_H_

#include "type.h"

// kernel/i8259
void init_8259A();
void spurious_irq(dword irq);

// kernel/main
void kernel_main();
void TestA();

//kernel/kernel
void restart();

// kernel/protect
void init_prot();
void exception_handler(int vec_no,int err_code,int eip,int cs,int eflags);
dword seg2phys(word seg);

// lib/io_lib
void print(byte* src, dword size, byte x, byte y, byte color);
void printNum(dword data, byte size, byte x, byte y, byte color);
void clean_screen();

// lib/num_lib
void num2str(dword data, byte temp[10]);


// lib/process_lib
void delay(dword time);


// lib/string_lib
void memcpy(byte* dest, byte* src, dword size);
void memset(byte* dest, byte data, dword size);


// lib/fun_asm
void debug();
void out_port(word port, byte value);
byte in_port(word port);


#endif
