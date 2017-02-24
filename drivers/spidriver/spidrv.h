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

#ifndef _SPI_DRV_H_
#define _SPI_DRV_H_
#include "spi.h"

#define CHIPSELECT_FPGA 		0
#define CHIPSELECT_DPLL 		1
#define CHIPSELECT_FPGA_FLASH 	2

#define MULTI_REG_LEN_MAX		64

extern int spidrv_init(void);
extern int spidrv_exit(void);
extern int spidrv_write(unsigned char *data, int len, int rxlen, unsigned char cs);
extern int spidrv_read(unsigned char *txbuf, int txlen,unsigned char *rxbuf, int rxlen, unsigned char cs);

#endif /* _SPI_DRV_H_ */

