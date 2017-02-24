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

#ifndef _FPGA_DRV_H_
#define _FPGA_DRV_H_
#include "spi.h"
#include "spidrv.h"

/*fpga读写接口函数*/
extern int fpga_write(unsigned int addr, unsigned char *data, size_t count,unsigned char slot);
extern int fpga_read(unsigned int addr, unsigned char *data, size_t count,unsigned char slot);
/*read from eeprom*/
extern int get_bdtype_from_eeprom(void);
/*epcs芯片接口函数*/
extern void fpga_flash_gpio_set(unsigned char en);
extern void fpga_flash_reconfig(void);
extern int fpga_flash_erase(void);
extern int fpga_flash_erase_sector(unsigned int offset);
extern int fpga_flash_write(long to, const unsigned char *buf, unsigned int len);
extern int fpga_flash_read(long from,unsigned char *buf, unsigned int len);

#endif
