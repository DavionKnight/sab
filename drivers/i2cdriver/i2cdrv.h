/*
*  COPYRIGHT NOTICE
*  Copyright (C) 2016 HuaHuan Electronics Corporation, Inc. All rights reserved
*
*  Author       		:yuhongyue
*  File Name        	:i2cdrv.h
*  Create Date          :2016/12/5
*  Last Modified        :2016/12/5
*  Description    	:
*/

#ifndef _I2C_DRV_H_
#define _I2C_DRV_H_

#include "i2c.h"

#define CHIPSELECT_FPGA 		0
#define CHIPSELECT_DPLL 		1
#define CHIPSELECT_FPGA_FLASH 	2

#define MULTI_REG_LEN_MAX		64

#define HT201


#define BCMDBG_ERR 1

/* debug/trace */
#ifdef BCMDBG_ERR
#define	SI_ERROR(args)	printf args
#else
#define	SI_ERROR(args)
#endif	/* BCMDBG_ERR */


#ifdef HT201
#define I2C_REGISTER_BASE	0x18008000


#define MAP_SIZE            4096UL
#define MAP_MASK            (MAP_SIZE - 1)

#endif







extern int i2cdrv_init(void* ptr);
extern int i2cdrv_exit(void);

#endif /* _SPI_DRV_H_ */

