// 本文件实现了一些用于进程处理的子函数

#include "global.h"

// 毫秒级
void delay(dword time)
{
	int i,j;
	for (i=0; i<time; i++)
		for (j=0; j<5000; j++)
		{}
}
