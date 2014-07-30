
// 本文件定义了一些普通常量

#ifndef		_AY27_CONST_H_
#define		_AY27_CONST_H_

#define 	GDT_SIZE		128
#define		IDT_SIZE		256

// 在loader中已定义好
#define		Selector_Kernel_CS	0x08

#define		video_addr		0xB8000
#define		video_length	0xffff
#define		video_max_Height	30
#define		video_max_Width		80

// color
#define		color_blue		9
#define		color_green 	10
#define		color_red		12




#endif
