/*
*  COPYRIGHT NOTICE
*  Copyright (C) 2016 HuaHuan Electronics Corporation, Inc. All rights reserved
*
*  Author       	:fzs
*  File Name        	:/home/kevin/works/projects/H20RN-2000/drivers/outband/phy_api.h
*  Create Date        	:2016/12/15 11:09
*  Last Modified      	:2016/12/15 11:09
*  Description    	:
*/
#ifndef __PHY_API_H__
#define __PHY_API_H__
/* Generic MII registers. */

#define MII_BMCR            0x00        /* Basic mode control register */
//#define MII_BMSR            0x01        /* Basic mode status register  */
#define MII_BMSR            0x02        /* Basic mode status register  */

#define BCM53101_A 0
#define BCM53101_B 1
#define BCM53101_C 2

long switch_access_set(unsigned char cmd);
int bcm53101_write(unsigned char which_sw, unsigned char page, unsigned char addr, unsigned short *value);
extern int bcm53101_read(unsigned char which_sw, unsigned char page, unsigned char addr, unsigned short *value);

int outband_init(void);
void outband_exit(void);

#endif

