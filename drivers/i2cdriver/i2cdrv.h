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


#ifdef H20RN2000
/*I2C register definition*/
#define I2C_REGISTER_BASE	0xFFE03000
#define I2C1_ADR			0xFFE03000
#define I2C1_FDR 			0xFFE03004
#define I2C1_CR 			0xFFE03008
#define I2C1_SR				0xFFE0300C
#define I2C1_DR				0xFFE03010
#define I2C1_DFSRRDR 		0xFFE03014
#define I2C2_ADR	 		0xFFE03100
#define I2C2_FDR	 		0xFFE03104
#define I2C2_CR 			0xFFE03108
#define I2C2_SR				0xFFE0310C
#define I2C2_DR				0xFFE03110
#define I2C2_DFSRRDR 		0xFFE03114
#endif

#ifdef HT201
#define I2C_REGISTER_BASE	0x18008000



#define MAP_SIZE            4096UL
#define MAP_MASK            (MAP_SIZE - 1)

#endif







extern int i2cdrv_init(void* ptr);
extern int i2cdrv_exit(void);
//extern int i2cdrv_write(unsigned char *data, int len, int rxlen, unsigned char cs);
//extern int i2cdrv_read(unsigned char *txbuf, int txlen,unsigned char *rxbuf, int rxlen, unsigned char cs);

#endif /* _SPI_DRV_H_ */

