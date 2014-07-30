
//本文件包含了几个用于数据输出的函数

#include "global.h"

// X对应width（列数）， Y对应height（行数）
void print(byte* src, dword size, byte x, byte y, byte color)
{
	byte* temp = (byte*) (video_addr + (80*y+x)*2);
	dword i,j=0;
	for (i=0; i<size; i++)
	{
		if (src[i] == '\n') 
		{
			j += 160;
			continue;
		}
		temp[j++] = src[i];
		temp[j++] = color;
	}
}

void printNum(dword data, byte size, byte x, byte y, byte color)
{
	byte temp[10];
	num2str(data, temp);
	print(temp+(9-size), size, x, y, color);
}

void clean_screen()
{
	int i,j;
	for (i=0; i<0x18; i++)
		for (j=0; j<0x4F; j++)
			print(" ", 1, j, i, 0);
}
