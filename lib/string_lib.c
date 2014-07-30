// 本文件实现了对字符串操作的函数


#include "global.h"


void memcpy(byte* dest, byte* src, dword size)
{
	dword i;
	for (i=0; i<size; i++)
		dest[i] = src[i];
}

void memset(byte* dest, byte data, dword size)
{
	dword i;
	for (i=0; i<size; i++)
		dest[i] = data;
}
