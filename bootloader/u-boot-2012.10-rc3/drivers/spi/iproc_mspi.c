/*
 * Copyright (C) 2016, Huahuan Corporation. All Rights Reserved.
 * 
 * Author: Tianzhongyong
 * Date : 2016-12-19
 */

#include <common.h>
#include <malloc.h>

/* Chip attributes */
#define MSPI_REG_BASE                       0x03201500
#define CMIC_OVERRIDE_STRAP		    0x3210234

#define NUM_TXRAM                           32
#define NUM_RXRAM                           32
#define NUM_CDRAM                           16

/* register def */
#define ENABLE_OVERRIDE_SPI_MASTER_SLAVE_MODE 	(1<<5)
#define SPI_MASTER_SLAVE_MODE			(1<<2)

/* MSPI registers */
struct mspi_hw {
    u32             mspi_spcr0_lsb;               /* 0x000 */
    u32             mspi_spcr0_msb;               /* 0x004 */
    u32             mspi_spcr1_lsb;               /* 0x008 */
    u32             mspi_spcr1_msb;               /* 0x00c */
    u32             mspi_newqp;                   /* 0x010 */
    u32             mspi_endqp;                   /* 0x014 */
    u32             mspi_spcr2;                   /* 0x018 */
    u32             reserved0;                    /* 0x01c */
    u32             mspi_status;             	  /* 0x020 */
    u32             mspi_cptqp;                   /* 0x024 */
    u32             reserved1[6];            	  /* 0x028 */
    u32             mspi_txram[NUM_TXRAM];        /* 0x040 */
    u32             mspi_rxram[NUM_RXRAM];        /* 0x0c0 */
    u32             mspi_cdram[NUM_CDRAM];        /* 0x140 */
    u32             cmicm_bspi_mast_n_boot;       /* 0x180 */
    u32             cmim_bspi_busy_status;        /* 0x184 */
};

struct mspi_hw *priv;

int mspi_init(void)
{
	u32 rval;
	u32 *ptr;
	
	ptr = (u32*)CMIC_OVERRIDE_STRAP;
	priv =	 (volatile struct mspi_hw *)(MSPI_REG_BASE + 0x000);

	/* Claim the SPI bus */
	rval = *ptr;
	rval |= ( ENABLE_OVERRIDE_SPI_MASTER_SLAVE_MODE | SPI_MASTER_SLAVE_MODE);
	*ptr = rval;

	priv->cmicm_bspi_mast_n_boot = 1;


	/* 
     	* Set speed and transfer size
     	*/
	priv->mspi_spcr0_lsb = 0x8;
	
	rval = priv->mspi_spcr1_lsb;
	rval |= 0x01;   /* Delay after transfer- DTL = 0x01 */
	priv->mspi_spcr1_lsb = rval;

	rval = priv->mspi_spcr1_msb;
	rval |= 0x01; /* DSCLK = 0x01 */
	priv->mspi_spcr1_msb = rval;
	
	//atest();
		
	return 0;

}

int mspi_config(int cpol, int cpha) 
{
	u32 rval;

	rval = priv->mspi_spcr0_msb;	
	if( cpol) 
	{
		rval |= (cpol << 1);
		priv->mspi_spcr0_msb = rval;
	}
	else
	{
		rval &= ~(cpol << 1);
		priv->mspi_spcr0_msb = rval;
	}

	if( cpha) 
	{
		rval |= (cpha << 0);
		priv->mspi_spcr0_msb = rval;
	}
	else
	{
		rval &= ~(cpha << 0);
		priv->mspi_spcr0_msb = rval;
	}

	return 0;

}


int mspi_writeread8( u8 *wbuf, int wlen, u8 *rbuf, int rlen)
{

    	int i, tlen, rv = 0;
    	u8 *datptr;
    	u32 rval=0;
	
	priv->mspi_spcr0_lsb = 0x8;
	priv->mspi_status = 0;


    	tlen = wlen + rlen;
    	if (tlen > 16) {
        	return -1;
    	}

    	if ((wbuf != NULL) && (wlen > 0)) {
        	datptr = wbuf;
        	for (i=0; i<wlen; i++) {
            	/* Use only Even index of TXRAM for 8 bit xmit */
		priv->mspi_txram[2*i] = (u32) *datptr;

            	datptr++;
        	}
    	}

    	for (i=0; i<tlen; i++) {
        	/* Release CS ony on last byute */
		priv->mspi_cdram[i] = (i == (tlen-1) ? 0 : 0x80);
    	}

    	/* Set queue pointers */
	priv->mspi_newqp = 0;
	priv->mspi_endqp = tlen-1;

    	/* Start SPI transfer */
    	rval = 0x40; /* SPE=1, SPIFIE=WREN=WRT0=LOOPQ=HIE=HALT=0 */
	priv->mspi_spcr2 = rval;

	rv = -1;
	for(i=0; i<0xffff; i++)
	{
		rval = priv->mspi_status;
		if(i== 0xfff) printf("mspi_status = %x\n", rval);
		if(rval & 0x01) 
		{
			rv = 0;
			//printf("spi read ok i=%x\n", i);
			break;
		}
	}
		

    	if (rv != 0) {
        	return -1;
    	}

    if ((rbuf != NULL) && (rlen > 0)) {
        datptr = rbuf;
        for (i=wlen; i<tlen; i++) {
            /* Use only Odd index of TXRAM for 8 bit Recv */
            *datptr = ((u8) priv->mspi_rxram[2*i+1] & 0xff);
            datptr++;
        }
    }
    return 0;
}

int mspi_read8( u8 *buf, int len)
{
    return  mspi_writeread8( NULL, 0, buf, len);
}

int mspi_write8( u8 *buf, int len)
{
    return  mspi_writeread8(buf, len, NULL, 0);
}

int atest()
{
	int rv;
	int i;
	char tbuf[10];

	char rbuf[16];

	tbuf[0] = 0x90;

	for(i=0; i<16; i++)
		rbuf[i] = 0;
	
	mspi_config(1,1);
/*
	rv = mspi_write8(tbuf, 1);
	printf("rv = %x\n", rv);

	mspi_read8(rbuf, 5);
*/
	mspi_writeread8(tbuf,1,rbuf,5);
	for(i=0; i<5; i++)
		printf("rxbuf = %x ", rbuf[i]);

}


