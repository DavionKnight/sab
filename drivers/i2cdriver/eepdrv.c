/*
*  COPYRIGHT NOTICE
*  Copyright (C) 2016 HuaHuan Electronics Corporation, Inc. All rights reserved
*
*  Author               :yuhongyue
*  File Name            :rtcdrv.c
*  Create Date          :2016/12/5
*  Last Modified        :2016/12/5
*  Description      :
*/
#include "i2c.h"
#include "eepdrv.h"


#include <stdio.h>

int eeprom_read ( unsigned dev_addr, unsigned offset, unsigned char *buffer, unsigned cnt )
{
	unsigned end = offset + cnt;
	unsigned blk_off;
	int rcode = 0;

	while ( offset < end )
	{
		unsigned alen, len;
		unsigned maxlen;

		unsigned char addr[2];

		blk_off = offset & 0xFF;	/* block offset */

		addr[0] = offset >> 8;		/* block number */
		addr[1] = blk_off;		/* block offset */
		alen	= 2;

		addr[0] |= dev_addr;		/* insert device address */

		len = end - offset;

		maxlen = 0x100 - blk_off;
		if ( maxlen > I2C_RXTX_LEN )
		{
			maxlen = I2C_RXTX_LEN;
		}
		if ( len > maxlen )
		{
			len = maxlen;
		}
printf("dev_addr = %d, offset = %d, alen = %d, len = %d\n ", addr[0], offset, alen - 1, len);
		if ( i2c_read ( addr[0], offset, alen - 1, buffer, len ) != 0 )
		{
			rcode = 1;
		}
		buffer += len;
		offset += len;
	}

	return rcode;
}


int eeprom_write ( unsigned dev_addr, unsigned offset, unsigned char *buffer, unsigned cnt )
{
    unsigned end = offset + cnt;
    unsigned blk_off;
    int rcode = 0;

    while ( offset < end )
    {
        unsigned alen, len;
        unsigned maxlen;

        unsigned char addr[2];

        blk_off = offset & 0xFF;    /* block offset */

        addr[0] = offset >> 8;      /* block number */
        addr[1] = blk_off;      /* block offset */
        alen    = 2;

        addr[0] |= dev_addr;        /* insert device address */

        len = end - offset;


#define EEPROM_PAGE_SIZE    (1 << 3)//(1 << CONFIG_SYS_EEPROM_PAGE_WRITE_BITS)
#define EEPROM_PAGE_OFFSET(x)   ((x) & (EEPROM_PAGE_SIZE - 1))

        maxlen = EEPROM_PAGE_SIZE - EEPROM_PAGE_OFFSET ( blk_off );

        if ( maxlen > I2C_RXTX_LEN )
        {
            maxlen = I2C_RXTX_LEN;
        }

        if ( len > maxlen )
        {
            len = maxlen;
        }

        if ( i2c_write ( addr[0], offset, alen - 1, buffer, len ) != 0 )
        {
            rcode = 1;
        }

        buffer += len;
        offset += len;

        usleep (5 * 1000);//( CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS * 1000 );
    }

    return rcode;
}



