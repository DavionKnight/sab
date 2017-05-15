/*
 * Copyright (C) 2013, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

//#include <common.h>
//#include <config.h>
#include "i2c.h"
//#include <asm/io.h>

#include "iproc_i2c_regs.h"
#include "iproc_i2c_defs.h"
//#include "iproc_i2c.h"
//#include "errno.h"

#include <stdio.h>
#include <string.h>

#include "i2cdrv.h"


/* I2C */
#define CONFIG_HARD_I2C
#define CONFIG_DRIVER_DAVINCI_I2C
#define CONFIG_SYS_I2C_SPEED        400000
#define CONFIG_SYS_I2C_SLAVE        0x10    /* SMBus host address */

extern unsigned int *mapbase;


static unsigned int iproc_i2c_reg_read ( unsigned int reg_addr )
{
    unsigned int val;
    //printf("mapbase[0] + reg_addr = %x\n", mapbase[0] + reg_addr);

    return ( * ( ( volatile unsigned int * ) ( mapbase + reg_addr ) ) );
}

static int iproc_i2c_reg_write ( unsigned int reg_addr, unsigned int val )
{
    ( * ( ( volatile unsigned int * ) ( mapbase + reg_addr ) ) = ( val ) );
    return ( 0 );
}


static int iproc_i2c_startbusy_wait ( void )
{
    unsigned int regval;

    regval = iproc_i2c_reg_read ( CCB_SMB_MSTRCMD_REG );

    if ( regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK )
    {
        unsigned int i = 0;

        do
        {
            usleep ( 10000 ); /* Wait for 1 msec */
            i++;
            regval = iproc_i2c_reg_read ( CCB_SMB_MSTRCMD_REG );

            /* If start-busy bit cleared, exit the loop */
        }
        while ( ( regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK ) && ( i < IPROC_SMB_MAX_RETRIES ) );

        if ( i >= IPROC_SMB_MAX_RETRIES )
        {
            printf ( "%s: START_BUSY bit didn't clear, exiting\n", __func__ );;
            return -1;
        }
    }
    return 0;
}


static void iproc_i2c_write_trans_data ( unsigned short dev_addr, struct iproc_xact_info *info )
{
    unsigned int regval;
    unsigned int i;
    unsigned int num_data_bytes = 0;

    iproc_i2c_reg_write ( CCB_SMB_MSTRDATAWR_REG, dev_addr );

    if ( info->cmd_valid )
    {
        iproc_i2c_reg_write ( CCB_SMB_MSTRDATAWR_REG, info->command );
    }

    switch ( info->smb_proto )
    {

    case SMBUS_PROT_RECV_BYTE:
        /* No additional data to be written */
        num_data_bytes = 0;
        break;

    case SMBUS_PROT_SEND_BYTE:
        num_data_bytes = info->size;
        break;

    case SMBUS_PROT_RD_BYTE:
    case SMBUS_PROT_RD_WORD:
    case SMBUS_PROT_BLK_RD:
        /* Write slave address with R/W~ set (bit #0) */
        iproc_i2c_reg_write ( CCB_SMB_MSTRDATAWR_REG, dev_addr | 0x1 );
        num_data_bytes = 0;
        break;
    case SMBUS_PROT_WR_BYTE:
    case SMBUS_PROT_WR_WORD:
        /* No additional bytes to be written. Data portion is written in the
         * 'for' loop below
         */
        num_data_bytes = info->size;
        break;

    case SMBUS_PROT_BLK_WR:
        /* 3rd byte is byte count */
        iproc_i2c_reg_write ( CCB_SMB_MSTRDATAWR_REG, info->size );
        num_data_bytes = info->size;
        break;

    default:
        break;

    }

    /* Copy actual data from caller, next. In general, for reads, no data is
     * copied
     */
    for ( i = 0; num_data_bytes; --num_data_bytes, i++ )
    {
        /* For the last byte, set MASTER_WR_STATUS bit */
        regval = ( num_data_bytes == 1 ) ?
                 info->data[i] | CCB_SMB_MSTRWRSTS_MASK : info->data[i];

        iproc_i2c_reg_write ( CCB_SMB_MSTRDATAWR_REG, regval );
    }

    return;
}


static int iproc_i2c_data_send ( unsigned short addr, struct iproc_xact_info *info )
{
    int rc, retry = 3;
    unsigned int regval;

    rc = iproc_i2c_startbusy_wait();

    if ( rc < 0 )
    {
        printf ( "%s: Send: bus is busy, exiting\n", __func__ );
        return rc;
    }

    iproc_i2c_write_trans_data ( addr, info );

    regval = ( info->smb_proto << CCB_SMB_MSTRSMBUSPROTO_SHIFT ) |
             CCB_SMB_MSTRSTARTBUSYCMD_MASK;

    iproc_i2c_reg_write ( CCB_SMB_MSTRCMD_REG, regval );

    regval = iproc_i2c_reg_read ( CCB_SMB_MSTRCMD_REG );
    while ( regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK )
    {
        usleep ( 10000 );
        if ( retry-- <= 0 )
        {
            break;
        }
        regval = iproc_i2c_reg_read ( CCB_SMB_MSTRCMD_REG );
    }

    if ( ! ( regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK ) )
    {
        regval &= CCB_SMB_MSTRSTS_MASK;
        regval >>= CCB_SMB_MSTRSTS_SHIFT;

        if ( regval != MSTR_STS_XACT_SUCCESS )
        {
            printf ( "%s: ERROR: Send: Error in transaction %u, exiting",
                     __func__, regval );
            return -1;//EREMOTEIO;
        }
    }

    return ( 0 );
}


static int iproc_i2c_data_recv ( unsigned short addr,
                                 struct iproc_xact_info *info,
                                 unsigned int *num_bytes_read )
{
    int rc, retry = 3;
    unsigned int regval;

    rc = iproc_i2c_startbusy_wait();

    if ( rc < 0 )
    {
        printf ( "%s: Receive: Bus is busy, exiting\n", __func__ );
        return rc;
    }

    iproc_i2c_write_trans_data ( addr, info );

    regval = ( info->smb_proto << CCB_SMB_MSTRSMBUSPROTO_SHIFT ) |
             CCB_SMB_MSTRSTARTBUSYCMD_MASK | info->size;


    iproc_i2c_reg_write ( CCB_SMB_MSTRCMD_REG, regval );

    regval = iproc_i2c_reg_read ( CCB_SMB_MSTRCMD_REG );

    while ( regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK )
    {
        usleep ( 1000 );
        if ( retry-- <= 0 )
        {
            break;
        }
        regval = iproc_i2c_reg_read ( CCB_SMB_MSTRCMD_REG );
    }

    if ( ! ( regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK ) )
    {
        regval &= CCB_SMB_MSTRSTS_MASK;
        regval >>= CCB_SMB_MSTRSTS_SHIFT;

        if ( regval != MSTR_STS_XACT_SUCCESS )
        {
            printf ( "%s: Error in transaction %d, exiting\n",
                     __func__, regval );
            return -1;//EREMOTEIO;
        }
    }

    regval = iproc_i2c_reg_read ( CCB_SMB_MSTRDATARD_REG );

    if ( info->smb_proto == SMBUS_PROT_BLK_RD )
    {

        int i;

        *num_bytes_read = regval & CCB_SMB_MSTRRDDATA_MASK;

        for ( i = 0; ( i < *num_bytes_read ) && ( i < I2C_SMBUS_BLOCK_MAX ); i++ )
        {

            regval = iproc_i2c_reg_read ( CCB_SMB_MSTRDATARD_REG );
            info->data[i] = regval & CCB_SMB_MSTRRDDATA_MASK;
        }
    }
    else if ( info->size == 2 )
    {
        /* 2 Bytes data */
        info->data[0] = regval & CCB_SMB_MSTRRDDATA_MASK;
        regval = iproc_i2c_reg_read ( CCB_SMB_MSTRDATARD_REG );
        info->data[1] = regval & CCB_SMB_MSTRRDDATA_MASK;
        *num_bytes_read = 2;
    }
    else
    {
        /* 1 Byte data */
        * ( info->data ) = regval & CCB_SMB_MSTRRDDATA_MASK;
        *num_bytes_read = 1;
    }

    return ( 0 );
}


static int iproc_i2c_set_clk_freq ( smb_clk_freq_t freq )
{
    unsigned int regval;
    unsigned int val;

    switch ( freq )
    {

    case I2C_SPEED_100KHz:
        val = 0;
        break;

    case I2C_SPEED_400KHz:
        val = 1;
        break;

    default:
        printf ( "iproc_i2c_set_clk_freq default\n" ); //addby lihz
        return -1;//EINVAL;
        break;
    }

    regval = iproc_i2c_reg_read ( CCB_SMB_TIMGCFG_REG );

    SETREGFLDVAL ( regval, val, CCB_SMB_TIMGCFG_MODE400_MASK, CCB_SMB_TIMGCFG_MODE400_SHIFT );

    iproc_i2c_reg_write ( CCB_SMB_TIMGCFG_REG, regval );
    return ( 0 );
}

static void iproc_i2c_init ( int speed, int slaveadd )
{
    unsigned int regval;

    regval = CCB_SMB_MSTRRXFIFOFLSH_MASK | CCB_SMB_MSTRTXFIFOFLSH_MASK;
    iproc_i2c_reg_write ( CCB_SMB_MSTRFIFOCTL_REG, regval );

    regval = iproc_i2c_reg_read ( CCB_SMB_CFG_REG );

    regval |= CCB_SMB_CFG_SMBEN_MASK;
    iproc_i2c_reg_write ( CCB_SMB_CFG_REG, regval );

    usleep ( 10000 ); /* ÷¡…Ÿ—” ±50us */

    iproc_i2c_set_clk_freq ( I2C_SPEED_100KHz );

    regval = 0x0;
    iproc_i2c_reg_write ( CCB_SMB_EVTEN_REG, regval );

    regval = iproc_i2c_reg_read ( CCB_SMB_EVTSTS_REG );
    iproc_i2c_reg_write ( CCB_SMB_EVTSTS_REG, regval );

}


void i2c_init ( int speed, int slaveadd )
{
    int aaa = 0;

    iproc_i2c_init ( speed, slaveadd );
#if 0
    aaa = iproc_i2c_reg_read ( 0x02 );
    printf ( "aaa = %d, %p\n", aaa, mapbase + 0x02 );
    iproc_i2c_reg_write ( 0x02, 0xa5 );

    iproc_i2c_reg_read ( 0x02 );
    printf ( "aaa = %d\n", aaa );
#endif

}

static int i2c_read_byte ( unsigned char devaddr, unsigned char regoffset, unsigned char *value )
{
    int rc;
    struct iproc_xact_info info;
    unsigned int num_bytes_read = 0;

    devaddr <<= 1;

    info.cmd_valid = 1;
    info.command = ( unsigned char ) regoffset;
    info.data = value;
    info.size = 1;
    info.flags = 0;
    info.smb_proto = SMBUS_PROT_RD_BYTE;
    /* Refer to i2c_smbus_read_byte for params passed. */
    rc = iproc_i2c_data_recv ( devaddr, &info, &num_bytes_read );

    if ( rc < 0 )
    {
        printf ( "%s: %s error accessing device 0x%X\n",
                 __func__, "Read", devaddr );
        return -1;
    }

    return ( 0 );
}

static int i2c_write_byte ( unsigned char devaddr, unsigned char regoffset, unsigned char value )
{
    int rc;
    struct iproc_xact_info info;


    devaddr <<= 1;

    info.cmd_valid = 1;
    info.command = ( unsigned char ) regoffset;
    info.data = &value;
    info.size = 1;
    info.flags = 0;
    info.smb_proto = SMBUS_PROT_WR_BYTE;

    rc = iproc_i2c_data_send ( devaddr, &info );

    if ( rc < 0 )
    {
        printf ( "%s: %s error accessing device 0x%X\n",
                 __func__, "Write", devaddr );
        return -1;//EREMOTEIO;
    }

    return ( 0 );
}


int i2c_read (unsigned char chip, unsigned int addr, int alen, unsigned char * buffer, int len)
{
	int i;

	if (alen > 1) 
	{
		printf ("I2C read: addr len %d not supported\n", alen);
		return 1;
	}

	if (addr + len > 256) 
	{
		printf ("I2C read: address out of range\n");
		return 1;
	}

	for (i = 0; i < len; i++)
	{
		if (i2c_read_byte (chip, addr + i, &buffer[i])) 
		{
			printf ("I2C read: I/O error\n");
			i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
			return 1;
		}
	}

	return 0;
}

int i2c_write (unsigned char chip, unsigned int addr, int alen, unsigned char * buffer, int len)
{
	int i;

	if (alen > 1) 
	{
		printf ("I2C read: addr len %d not supported\n", alen);
		return 1;
	}

	if (addr + len > 256) 
	{
		printf ("I2C read: address out of range\n");
		return 1;
	}

	for (i = 0; i < len; i++) 
	{
		if (i2c_write_byte (chip, addr + i, buffer[i])) 
		{
			printf ("I2C read: I/O error\n");
			i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
			return 1;
		}
	}

	return 0;
}
int i2c_write_word (unsigned char devaddr, unsigned int addr, unsigned short value)
{
    int rc;
    struct iproc_xact_info info;
    unsigned char data[2];

    devaddr <<= 1;
    data[0] = value & 0xff;
    data[1] = (value & 0xff00) >> 8;

    info.cmd_valid = 1;
    info.command = (unsigned char)addr;
    info.data = data; 
    info.size = 2;
    info.flags = 0;
    info.smb_proto = SMBUS_PROT_WR_WORD;

    /* Refer to i2c_smbus_write_byte params passed. */
    rc = iproc_i2c_data_send(devaddr, &info);

    if (rc < 0) 
	{
        printf("%s: %s error accessing device 0x%X\n", 
                    __func__, "Write", devaddr);
        return -1;//EREMOTEIO;
    }

    return (0);
}

int i2c_read_word (unsigned char devaddr, unsigned int addr , unsigned short* value)
{
    int rc;
    struct iproc_xact_info info;
    unsigned char data[2];
    unsigned int num_bytes_read;

    devaddr <<= 1;

    info.cmd_valid = 1;
    info.command = (unsigned char)addr;
    info.data = data; 
    info.size = 2;
    info.flags = 0;
    info.smb_proto = SMBUS_PROT_RD_WORD;

    rc = iproc_i2c_data_recv(devaddr, &info, &num_bytes_read);

    if (rc < 0) {
        printf("%s: %s error accessing device 0x%X\n", 
                    __func__, "Read", devaddr);
        return -1;//EREMOTEIO;
    }

    *value = (unsigned short) (info.data[0] & 0xff);
    *value |= (unsigned short)(((unsigned short)info.data[1]) << 8);	    

    return (0);
}
int i2c_block_read(unsigned char chip, unsigned int addr, unsigned char* buffer, int* len)
{
    int i, rc;
    struct iproc_xact_info info;
    unsigned int num_bytes_read;

    chip <<= 1;

    info.cmd_valid = 1;
    info.command = (unsigned char)addr;
    info.data = buffer; 
    info.size = 0;
    info.flags = 0;
    info.smb_proto = SMBUS_PROT_BLK_RD;

    rc = iproc_i2c_data_recv(chip, &info, &num_bytes_read);

    if (rc < 0) 
	{
        printf("%s: %s error accessing device 0x%X\n", 
                    __func__, "Read", chip);
        return -1;//EREMOTEIO;
    }

    for( i = 0; i < num_bytes_read; i++) {
        buffer[i] = info.data[i];
    }
    *len = num_bytes_read;

    return (0);
}
int i2c_send_byte (unsigned char chip, unsigned char addr)
{
    int rc;
    struct iproc_xact_info info;
    unsigned char data[2];

    chip <<= 1;
    data[0] = addr;

    info.cmd_valid = 0;
    info.data = data; 
    info.size = 1;
    info.flags = 0;
    info.smb_proto = SMBUS_PROT_SEND_BYTE;

    /* Refer to i2c_smbus_write_byte params passed. */
    rc = iproc_i2c_data_send(chip, &info);

    if (rc < 0) 
	{
        printf("%s: %s error accessing device 0x%X\n", 
                    __func__, "Write", chip);
        return -1;//EREMOTEIO;
    }

    return (0);
}

