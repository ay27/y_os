// 本文件实现了对数据的一些操作


#include "global.h"


void num2str(dword data, byte temp[10])
{
	dword x = 0x10000000;
	byte k;
	dword i;
	for (i=0; i<8; i++)
	{
		k = data / x;
		if (k>9) temp[i] = (k-10) + 'A';
			else temp[i] = k + '0';
		data %= x;
		x /= 0x10;
	}
	temp[8] = 'H';
	
}
