/*
*  COPYRIGHT NOTICE
*  Copyright (C) 2016 HuaHuan Electronics Corporation, Inc. All rights reserved
*
*  Author       		:yuhongyue
*  File Name        	:rtcdrv.h
*  Create Date        	:2016/12/5 
*  Last Modified      	:2016/12/5 
*  Description    	:
*/

#ifndef _EEPROM_H
#define _EEPROM_H

#include "i2c.h"
#include "i2cdrv.h"

#define I2C_RXTX_LEN	128	/* maximum tx/rx buffer length */

int eeprom_read (unsigned dev_addr, unsigned offset, unsigned char *buffer, unsigned cnt);
int eeprom_write (unsigned dev_addr, unsigned offset, unsigned char *buffer, unsigned cnt);

#endif

