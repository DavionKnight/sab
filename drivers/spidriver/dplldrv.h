/*
*  COPYRIGHT NOTICE
*  Copyright (C) 2016 HuaHuan Electronics Corporation, Inc. All rights reserved
*
*  Author       		:yuhongyue
*  File Name        	:spidrv.h
*  Create Date        	:2016/11/5 
*  Last Modified      	:2016/11/5 
*  Description    	:
*/

#ifndef _DPLL_DRV_H_
#define _DPLL_DRV_H_
#include "spi.h"
/*dpll外部接口函数*/
extern int dpll_write(unsigned short addr, unsigned char *data, size_t count,unsigned char flag);
extern int dpll_read(unsigned short addr, unsigned char *data, size_t count,unsigned char flag);

#endif

