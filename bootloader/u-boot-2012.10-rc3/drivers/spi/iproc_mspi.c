/*
 * Copyright (C) 2016, Huahuan Corporation. All Rights Reserved.
 * 
 * Author: Tianzhongyong
 * Date : 2016-12-19
 */

#include <common.h>
#include <malloc.h>
#include "asm/iproc/reg_utils.h"
#include "asm/iproc/iproc_common.h"

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
	priv->mspi_spcr0_lsb = 25;
	
	rval = priv->mspi_spcr1_lsb;
	rval |= 0x01;   /* Delay after transfer- DTL = 0x01 */
	priv->mspi_spcr1_lsb = rval;

	rval = priv->mspi_spcr1_msb;
	rval |= 0x01; /* DSCLK = 0x01 */
	priv->mspi_spcr1_msb = rval;

	dpll_init_pre();	
	atest();
		
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
	
	priv->mspi_spcr0_lsb = 25;
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
//			printf("spi read ok i=%x\n", i);
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
int dpll_spi_read_driver(unsigned short addr, unsigned char *data, size_t count)
{
	unsigned short address = 0;
	unsigned char txbuf[16] = {0};
	unsigned char rxbuf[16] = {0};
	int i = 0;

	mspi_cs_set(1, 1);
//	for(i=0;i<count;i++)
//	{
		address = (addr<<1)&0x7ffe;
		txbuf[0] = (unsigned char)((address >> 8) & 0xff);
		txbuf[1] = (unsigned char)((address) & 0xff);

		/* MSB must be '1' to read */
		txbuf[0] |= 0x80;
		/* LSB must be '1' to burst read */
//		if (count > 1)
			txbuf[1] |= 0x01;

		txbuf[2] = 0;	
		mspi_writeread8(txbuf,2,rxbuf,10);
		memcpy(data, rxbuf, count);
//	}
#if 0
	printf("data\n");
	for(i=0;i<16;i++)
		printf(" 0x%02x",data[i]);
#endif
	printf("\nrxbuf\n");
	for(i=0;i<16;i++)
		printf(" 0x%02x",rxbuf[i]);
	printf("\n");

	mspi_cs_set(1, 0);
	return 0;
}
int dpll_spi_write_driver(unsigned short addr, unsigned char *data, size_t count)
{
	unsigned short address = 0;
	unsigned char txbuf[16] = {0};
	unsigned char rxbuf[16] = {0};
	int i = 0;

	mspi_cs_set(1, 1);
//	for(i=0;i<count;i++)
//	{
		address = (addr<<1)&0x7ffe;
		txbuf[0] = (unsigned char)((address >> 8) & 0xff);
		txbuf[1] = (unsigned char)((address) & 0xff);

		/* MSB must be '0' to write */
		txbuf[0] &= ~0x80;
		/* LSB must be '1' to burst write */
		if (count > 1)
			txbuf[1] |= 0x01;

		memcpy(&txbuf[2], data, count);
//		mspi_writeread8(txbuf, count+2, rxbuf, 2);
		mspi_write8(txbuf, count+2);
//	}
	mspi_cs_set(1, 0);
	return 0;
}
void dpll_spi_write(unsigned short addr, unsigned char data)
{

	dpll_spi_write_driver(addr, &data, 1);
}
#define GPIO_BASE	0x1800a000
#define GPIO_OUTEN	((volatile unsigned int *)0x1800a008)
#define GPIO_VALUE	((volatile unsigned int *)0x1800a004)
void mspi_cs_set(unsigned char cs, unsigned char en)
{
	unsigned int val = 0;
	
	if(cs > 2)
	{
		printf("mspi_cs_set cs =%d err\n",cs);
		return;
	}
	/*set value*/
	val = reg32_read(GPIO_VALUE);	
	val = en? val&( ~(1<<cs)):val|(1<<cs);	
	reg32_write(GPIO_VALUE, val);

	/*set directory*/
	val = reg32_read(GPIO_OUTEN);	
	val &= ~0x7;
	val = en? val|(1<<cs):val;	
	reg32_write(GPIO_OUTEN, val);

	return;
	
}
void dpll_init_pre()
{
	printf("dpll init ...");
	mspi_config(1,1);

	dpll_spi_write( 0x0006, 0x80);
	udelay(1000000);
	dpll_spi_write( 0x0006, 0x00);
	/*#reset MCPLL*/
	dpll_spi_write( 0x03DE, 0xF0);
	dpll_spi_write( 0x03DF, 0x40);
	dpll_spi_write( 0x03E0, 0x3F);
	dpll_spi_write( 0x03E1, 0x8F);
	dpll_spi_write( 0x03E2, 0x80);
	dpll_spi_write( 0x03E3, 0x08);
	dpll_spi_write( 0x03E4, 0x98);
	dpll_spi_write( 0x03E5, 0x04);
	dpll_spi_write( 0x03E5, 0x00);
#if 0
	/*# Output APLL1*/
	dpll_spi_write( 0x00C0, 0x01);
	/*# Enable Noise Shapping*/
	dpll_spi_write( 0x00D3, 0x03);
	dpll_spi_write( 0x00D4, 0x40);
	dpll_spi_write( 0x00D6, 0x3F);
	dpll_spi_write( 0x00D7, 0x0F);
	dpll_spi_write( 0x00D8, 0x87);
	dpll_spi_write( 0x00D9, 0x08);

	/*# Output APLL2*/
	dpll_spi_write( 0x00C0, 0x02);
	/*# Enable Noise Shapping*/
	dpll_spi_write( 0x00D3, 0x03);
	dpll_spi_write( 0x00D4, 0x40);
	dpll_spi_write( 0x00D6, 0x3F);
	dpll_spi_write( 0x00D7, 0x0F);
	dpll_spi_write( 0x00D8, 0x87);
	dpll_spi_write( 0x00D9, 0x08);

	/*# Output APLL3*/
	dpll_spi_write( 0x00C0, 0x03);

	/*# Enable Noise Shapping*/
	dpll_spi_write( 0x00D3, 0x03);
	dpll_spi_write( 0x00D4, 0x40);
	dpll_spi_write( 0x00D6, 0x3F);
	dpll_spi_write( 0x00D7, 0x0F);
	dpll_spi_write( 0x00D8, 0x87);
	dpll_spi_write( 0x00D9, 0x08);

	/*## Configure the DS31400 to use a 19.44MHz MCLKOSC
# Pins OSCFREQ[2:0] must be configured to 100*/
	dpll_spi_write( 0x03DD, 0x83);
	dpll_spi_write( 0x03E1, 0x0F);
	dpll_spi_write( 0x03E4, 0x18);
	dpll_spi_write( 0x03DE, 0x4F);
	dpll_spi_write( 0x03E5, 0x04);
	dpll_spi_write( 0x03E5, 0x00);
#endif
#if 0
	/*# init priority to 0 (disable) ic7 = 1 highest*/
	dpll_spi_write( 0x0080, 0x00);
#endif

	/*# dpll 2 - force select local clock ic3*/
	dpll_spi_write( 0x0080, 0x01);
	dpll_spi_write( 0x0084, 0x61);
	dpll_spi_write( 0x0085, 0x74);
	dpll_spi_write( 0x0089, 0x27);
#if 0
	/*# dpll 1 - force select local clock ic3*/
	dpll_spi_write( 0x0080, 0x00);
	dpll_spi_write( 0x0081, 0x03);
#endif
#if 0
	/*# ic1  64k*/
	dpll_spi_write( 0x0060, 0x01);
	dpll_spi_write( 0x0061, 0x82);
	dpll_spi_write( 0x0062, 0x01);

	/*# ic2  64k*/
	dpll_spi_write( 0x0060, 0x02);
	dpll_spi_write( 0x0061, 0x82);
	dpll_spi_write( 0x0062, 0x01);

	/*##ic3 ic4 5 6 7  156.25M*/
	dpll_spi_write( 0x0060, 0x03);
	dpll_spi_write( 0x0061, 0x9C);
	dpll_spi_write( 0x0062, 0x01);
	dpll_spi_write( 0x0064, 0x00);
	dpll_spi_write( 0x0065, 0x00);
	dpll_spi_write( 0x0066, 0x04);
	dpll_spi_write( 0x0067, 0x00);
	dpll_spi_write( 0x0068, 0x00);
	dpll_spi_write( 0x0069, 0x00);

	dpll_spi_write( 0x0060, 0x04);
	dpll_spi_write( 0x0061, 0x9C);
	dpll_spi_write( 0x0062, 0x01);
	dpll_spi_write( 0x0064, 0x00);
	dpll_spi_write( 0x0065, 0x00);
	dpll_spi_write( 0x0066, 0x04);
	dpll_spi_write( 0x0067, 0x00);
	dpll_spi_write( 0x0068, 0x00);
	dpll_spi_write( 0x0069, 0x00);

	dpll_spi_write( 0x0060, 0x05);
	dpll_spi_write( 0x0061, 0x9C);
	dpll_spi_write( 0x0062, 0x01);
	dpll_spi_write( 0x0064, 0x00);
	dpll_spi_write( 0x0065, 0x00);
	dpll_spi_write( 0x0066, 0x04);
	dpll_spi_write( 0x0067, 0x00);
	dpll_spi_write( 0x0068, 0x00);
	dpll_spi_write( 0x0069, 0x00);

	dpll_spi_write( 0x0060, 0x06);
	dpll_spi_write( 0x0061, 0x9C);
	dpll_spi_write( 0x0062, 0x01);
	dpll_spi_write( 0x0064, 0x00);
	dpll_spi_write( 0x0065, 0x00);
	dpll_spi_write( 0x0066, 0x04);
	dpll_spi_write( 0x0067, 0x00);
	dpll_spi_write( 0x0068, 0x00);
	dpll_spi_write( 0x0069, 0x00);

	/*##ic7 for 2000 is 64K*/
	dpll_spi_write( 0x0060, 0x07);
	dpll_spi_write( 0x0061, 0x82);
	dpll_spi_write( 0x0062, 0x01);

	/*##ic8 64k*/
	dpll_spi_write( 0x0060, 0x08);
	dpll_spi_write( 0x0061, 0x82);
	dpll_spi_write( 0x0062, 0x01);
#endif
#if 1
	/*# oc1 cml 50MHz*/
	dpll_spi_write( 0x00C0, 0x01);
	dpll_spi_write( 0x00C1, 0xC1);
	dpll_spi_write( 0x00C2, 0x10);
	dpll_spi_write( 0x00C4, 0x80);
	dpll_spi_write( 0x00C8, 0x04);
	dpll_spi_write( 0x00C9, 0x00);
	dpll_spi_write( 0x00CA, 0x00);
	dpll_spi_write( 0x00CB, 0x00);
	dpll_spi_write( 0x00D0, 0x92);
	dpll_spi_write( 0x00D1, 0x05);
	dpll_spi_write( 0x00D2, 0x0e);
	dpll_spi_write( 0x00D8, 0x02);
	dpll_spi_write( 0x00E0, 0x00);
	dpll_spi_write( 0x00E1, 0x00);
	dpll_spi_write( 0x00E2, 0x00);
	dpll_spi_write( 0x00E3, 0x00);
	dpll_spi_write( 0x00E4, 0x00);
	dpll_spi_write( 0x00E5, 0x00);
	dpll_spi_write( 0x00E6, 0x00);
	dpll_spi_write( 0x00E7, 0x00);
	dpll_spi_write( 0x00E8, 0x00);
	dpll_spi_write( 0x00E9, 0x4b);
	dpll_spi_write( 0x00F2, 0x2C);

	/*# oc2 cml 50MHz */
	dpll_spi_write( 0x00C0, 0x02);
	dpll_spi_write( 0x00C1, 0xC1);
	dpll_spi_write( 0x00C2, 0x10);
	dpll_spi_write( 0x00C4, 0x80);
	dpll_spi_write( 0x00C8, 0x04);
	dpll_spi_write( 0x00C9, 0x00);
	dpll_spi_write( 0x00CA, 0x00);
	dpll_spi_write( 0x00CB, 0x00);
	dpll_spi_write( 0x00D0, 0x92);
	dpll_spi_write( 0x00D1, 0x25);
	dpll_spi_write( 0x00D2, 0x2e);
	dpll_spi_write( 0x00D8, 0x02);
	dpll_spi_write( 0x00E0, 0x00);
	dpll_spi_write( 0x00E1, 0x00);
	dpll_spi_write( 0x00E2, 0x00);
	dpll_spi_write( 0x00E3, 0x00);
	dpll_spi_write( 0x00E4, 0x00);
	dpll_spi_write( 0x00E5, 0x00);
	dpll_spi_write( 0x00E6, 0x00);
	dpll_spi_write( 0x00E7, 0x00);
	dpll_spi_write( 0x00E8, 0x00);
	dpll_spi_write( 0x00E9, 0x4b);
	dpll_spi_write( 0x00F2, 0x2c);

	/*# oc3 cmos 25MHz, cml 50MHz */
	dpll_spi_write( 0x00C0, 0x03);
	dpll_spi_write( 0x00C1, 0xC0);
	dpll_spi_write( 0x00C2, 0x10);
	dpll_spi_write( 0x00C3, 0x80);
	dpll_spi_write( 0x00C4, 0x80);
	dpll_spi_write( 0x00C8, 0x04);
	dpll_spi_write( 0x00C9, 0x00);
	dpll_spi_write( 0x00CA, 0x00);
	dpll_spi_write( 0x00CB, 0x00);
	dpll_spi_write( 0x00D0, 0x83);
	dpll_spi_write( 0x00D2, 0x0d);
	dpll_spi_write( 0x00D8, 0x02);
	dpll_spi_write( 0x00E0, 0x00);
	dpll_spi_write( 0x00E1, 0x00);
	dpll_spi_write( 0x00E2, 0x00);
	dpll_spi_write( 0x00E3, 0x00);
	dpll_spi_write( 0x00E4, 0x00);
	dpll_spi_write( 0x00E5, 0x00);
	dpll_spi_write( 0x00E6, 0x00);
	dpll_spi_write( 0x00E7, 0x00);
	dpll_spi_write( 0x00E8, 0x00);
	dpll_spi_write( 0x00E9, 0x4d);
	dpll_spi_write( 0x00f2, 0x2c);

	/*# Begin OC4 Register Bank cmos 25MHz cml 125MHz*/
	dpll_spi_write( 0x00C0, 0x04);
	dpll_spi_write( 0x00c1, 0xc0);
	dpll_spi_write( 0x00c2, 0x10);
	dpll_spi_write( 0x00C3, 0x80);
	dpll_spi_write( 0x00C4, 0x80);
	dpll_spi_write( 0x00C8, 0x04);
	dpll_spi_write( 0x00C9, 0x00);
	dpll_spi_write( 0x00CA, 0x00);
	dpll_spi_write( 0x00CB, 0x00);

	/*# oc5 cml 125MHz */
	dpll_spi_write( 0x00C0, 0x05);
	dpll_spi_write( 0x00b0, 0x33);
	dpll_spi_write( 0x00b1, 0x33);
	dpll_spi_write( 0x00b2, 0x33);
	dpll_spi_write( 0x00b3, 0x31);
	dpll_spi_write( 0x00b4, 0x61);
	dpll_spi_write( 0x00b8, 0x02);
	dpll_spi_write( 0x00bc, 0xdf);
	dpll_spi_write( 0x00bd, 0x97);
	dpll_spi_write( 0x00be, 0x69);
	dpll_spi_write( 0x00c1, 0xc1);
	dpll_spi_write( 0x00c2, 0x10);
	dpll_spi_write( 0x00c4, 0x80);
	dpll_spi_write( 0x00d3, 0x58);

	/*# Begin OC6 Register Bank cmos 25MHz*/
	dpll_spi_write( 0x00C0, 0x06);
	dpll_spi_write( 0x00b0, 0x33);
	dpll_spi_write( 0x00b1, 0x33);
	dpll_spi_write( 0x00b2, 0x33);
	dpll_spi_write( 0x00b3, 0x31);
	dpll_spi_write( 0x00b4, 0x61);
	dpll_spi_write( 0x00b8, 0x02);
	dpll_spi_write( 0x00bc, 0xdf);
	dpll_spi_write( 0x00bd, 0x97);
	dpll_spi_write( 0x00be, 0x69);
	dpll_spi_write( 0x00C1, 0xC0);
	dpll_spi_write( 0x00C2, 0x10);
	dpll_spi_write( 0x00C3, 0x80);
	dpll_spi_write( 0x00C8, 0x04);
	dpll_spi_write( 0x00C9, 0x00);
	dpll_spi_write( 0x00CA, 0x00);
	dpll_spi_write( 0x00CB, 0x00);
	dpll_spi_write( 0x00d3, 0x58);
	/*# End OC6 Register Bank

# Begin OC7 Register Bank  cmos 2MHz*/
#if 0
	dpll_spi_write( 0x00C0, 0x07);
	dpll_spi_write( 0x00B0, 0x33);
	dpll_spi_write( 0x00B1, 0x33);
	dpll_spi_write( 0x00B2, 0x33);
	dpll_spi_write( 0x00B3, 0x31);
	dpll_spi_write( 0x00B4, 0x61);
	dpll_spi_write( 0x00B8, 0x02);
	dpll_spi_write( 0x00BC, 0xdf);
	dpll_spi_write( 0x00BD, 0x97);
	dpll_spi_write( 0x00BE, 0x69);
	dpll_spi_write( 0x00C1, 0x40);
	dpll_spi_write( 0x00C3, 0x80);
	dpll_spi_write( 0x00C4, 0x20);
	dpll_spi_write( 0x00C5, 0x01);
	dpll_spi_write( 0x00C8, 0x1f);
	dpll_spi_write( 0x00C9, 0x00);
	dpll_spi_write( 0x00CA, 0x00);
	dpll_spi_write( 0x00CB, 0x00);
	dpll_spi_write( 0x00CC, 0x04);
	dpll_spi_write( 0x00CD, 0x00);
	dpll_spi_write( 0x00CE, 0x00);
	dpll_spi_write( 0x00CF, 0x00);
	dpll_spi_write( 0x00D3, 0x58);
#else
	dpll_spi_write( 0x00C0, 0x07);
	dpll_spi_write( 0x00C1, 0x30);
	dpll_spi_write( 0x00C3, 0x80);
	dpll_spi_write( 0x00C8, 0x17);
	dpll_spi_write( 0x00c9, 0x00);
	dpll_spi_write( 0x00ca, 0x00);
	dpll_spi_write( 0x00cb, 0x00);
	
#endif
	/*# End OC7 Register Bank*/

	dpll_spi_write( 0x03E5, 0x04);
	dpll_spi_write( 0x03E5, 0x00);

	/*# relock apll 1*/
	dpll_spi_write( 0x00C0, 0x01);
	dpll_spi_write( 0x00D0, 0x12);
	dpll_spi_write( 0x00D0, 0x92);
	/*# relock apll 2 */
	dpll_spi_write( 0x00C0, 0x02);
	dpll_spi_write( 0x00D0, 0x12);
	dpll_spi_write( 0x00D0, 0x92);
	/*# relock apll 3*/ 
	dpll_spi_write( 0x00C0, 0x03);
	dpll_spi_write( 0x00D0, 0x03);
	dpll_spi_write( 0x00D0, 0x83);
	/*# lock the dpll register*/
#else
	/*# oc1 cmos 25MHz, cml 25MHz*/
	dpll_spi_write( 0x00C0, 0x01);
	dpll_spi_write( 0x00C1, 0xC1);
	dpll_spi_write( 0x00C2, 0x10);
	dpll_spi_write( 0x00C3, 0x80);
	dpll_spi_write( 0x00C4, 0x80);
	dpll_spi_write( 0x00C8, 0x00);
	dpll_spi_write( 0x00C9, 0x00);
	dpll_spi_write( 0x00CA, 0x00);
	dpll_spi_write( 0x00CB, 0x00);
	dpll_spi_write( 0x00D0, 0x98);
	dpll_spi_write( 0x00D1, 0x13);
	dpll_spi_write( 0x00D2, 0x13);
	dpll_spi_write( 0x00D8, 0x02);
	dpll_spi_write( 0x00E0, 0x00);
	dpll_spi_write( 0x00E1, 0x00);
	dpll_spi_write( 0x00E2, 0x00);
	dpll_spi_write( 0x00E3, 0x00);
	dpll_spi_write( 0x00E4, 0x00);
	dpll_spi_write( 0x00E5, 0x00);
	dpll_spi_write( 0x00E6, 0x00);
	dpll_spi_write( 0x00E7, 0x00);
	dpll_spi_write( 0x00E8, 0x00);
	dpll_spi_write( 0x00E9, 0x50);
	dpll_spi_write( 0x00F2, 0x2C);

	/*# oc2 cmos 25MHz, cml 25MHz */
	dpll_spi_write( 0x00C0, 0x02);
	dpll_spi_write( 0x00C1, 0xC1);
	dpll_spi_write( 0x00C2, 0x10);
	dpll_spi_write( 0x00C3, 0x80);
	dpll_spi_write( 0x00C4, 0x80);
	dpll_spi_write( 0x00C8, 0x00);
	dpll_spi_write( 0x00C9, 0x00);
	dpll_spi_write( 0x00CA, 0x00);
	dpll_spi_write( 0x00CB, 0x00);
	dpll_spi_write( 0x00D0, 0x98);
	dpll_spi_write( 0x00D1, 0x13);
	dpll_spi_write( 0x00D2, 0x13);
	dpll_spi_write( 0x00D8, 0x02);
	dpll_spi_write( 0x00E0, 0x00);
	dpll_spi_write( 0x00E1, 0x00);
	dpll_spi_write( 0x00E2, 0x00);
	dpll_spi_write( 0x00E3, 0x00);
	dpll_spi_write( 0x00E4, 0x00);
	dpll_spi_write( 0x00E5, 0x00);
	dpll_spi_write( 0x00E6, 0x00);
	dpll_spi_write( 0x00E7, 0x00);
	dpll_spi_write( 0x00E8, 0x00);
	dpll_spi_write( 0x00E9, 0x50);

	/*# oc3 cmos 25MHz, cml 25MHz */
	dpll_spi_write( 0x00C0, 0x03);
	dpll_spi_write( 0x00C1, 0xC1);
	dpll_spi_write( 0x00C2, 0x10);
	dpll_spi_write( 0x00C3, 0x80);
	dpll_spi_write( 0x00C4, 0x80);
	dpll_spi_write( 0x00C8, 0x00);
	dpll_spi_write( 0x00C9, 0x00);
	dpll_spi_write( 0x00CA, 0x00);
	dpll_spi_write( 0x00CB, 0x00);
	dpll_spi_write( 0x00D0, 0x98);
	dpll_spi_write( 0x00D1, 0x13);
	dpll_spi_write( 0x00D2, 0x13);
	dpll_spi_write( 0x00D8, 0x02);
	dpll_spi_write( 0x00E0, 0x00);
	dpll_spi_write( 0x00E1, 0x00);
	dpll_spi_write( 0x00E2, 0x00);
	dpll_spi_write( 0x00E3, 0x00);
	dpll_spi_write( 0x00E4, 0x00);
	dpll_spi_write( 0x00E5, 0x00);
	dpll_spi_write( 0x00E6, 0x00);
	dpll_spi_write( 0x00E7, 0x00);
	dpll_spi_write( 0x00E8, 0x00);
	dpll_spi_write( 0x00E9, 0x50);

	/*# Begin OC4 Register Bank  63.536 k*/
	dpll_spi_write( 0x00C0, 0x04);
	dpll_spi_write( 0x00B0, 0x74);
	dpll_spi_write( 0x00B1, 0x9B);
	dpll_spi_write( 0x00B2, 0xD5);
	dpll_spi_write( 0x00B3, 0xE9);
	dpll_spi_write( 0x00B4, 0x51);
	dpll_spi_write( 0x00B5, 0x44);
	dpll_spi_write( 0x00B6, 0x83);
	dpll_spi_write( 0x00B7, 0xAF);
	dpll_spi_write( 0x00B8, 0x01);
	dpll_spi_write( 0x00BC, 0xFF);
	dpll_spi_write( 0x00BD, 0x7F);
	dpll_spi_write( 0x00BE, 0x7D);
	dpll_spi_write( 0x00C1, 0x40);
	dpll_spi_write( 0x00C3, 0x80);
	dpll_spi_write( 0x00C8, 0xE7);
	dpll_spi_write( 0x00C9, 0x03);
	dpll_spi_write( 0x00CA, 0x00);
	dpll_spi_write( 0x00CB, 0x00);
	dpll_spi_write( 0x00D3, 0x5C);

	/*# oc5 cmos and cml 25MHz */
	dpll_spi_write( 0x00C0, 0x05);
	dpll_spi_write( 0x00C1, 0xC0);
	dpll_spi_write( 0x00C3, 0x80);
	dpll_spi_write( 0x00C4, 0x80);
	dpll_spi_write( 0x00C8, 0x01);
	dpll_spi_write( 0x00C9, 0x00);
	dpll_spi_write( 0x00CA, 0x00);
	dpll_spi_write( 0x00CB, 0x00);
	dpll_spi_write( 0x00CC, 0x01);
	dpll_spi_write( 0x00CD, 0x00);
	dpll_spi_write( 0x00CE, 0x00);
	dpll_spi_write( 0x00CF, 0x00);

	/*# Begin OC6 Register Bank 1hz*/
	dpll_spi_write( 0x00C0, 0x06);
	dpll_spi_write( 0x00C1, 0xC0);
	dpll_spi_write( 0x00C3, 0x80);
	dpll_spi_write( 0x00C8, 0x7F);
	dpll_spi_write( 0x00C9, 0xF0);
	dpll_spi_write( 0x00CA, 0xFA);
	dpll_spi_write( 0x00CB, 0x02);
	/*# End OC6 Register Bank

# Begin OC7 Register Bank  63.536 k*/
	dpll_spi_write( 0x00C0, 0x07);
	dpll_spi_write( 0x00B0, 0x74);
	dpll_spi_write( 0x00B1, 0x9B);
	dpll_spi_write( 0x00B2, 0xD5);
	dpll_spi_write( 0x00B3, 0xE9);
	dpll_spi_write( 0x00B4, 0x51);
	dpll_spi_write( 0x00B5, 0x44);
	dpll_spi_write( 0x00B6, 0x83);
	dpll_spi_write( 0x00B7, 0xAF);
	dpll_spi_write( 0x00B8, 0x01);
	dpll_spi_write( 0x00BC, 0xFF);
	dpll_spi_write( 0x00BD, 0x7F);
	dpll_spi_write( 0x00BE, 0x7D);
	dpll_spi_write( 0x00C1, 0x40);
	dpll_spi_write( 0x00C3, 0x80);
	dpll_spi_write( 0x00C8, 0xE7);
	dpll_spi_write( 0x00C9, 0x03);
	dpll_spi_write( 0x00CA, 0x00);
	dpll_spi_write( 0x00CB, 0x00);
	dpll_spi_write( 0x00D3, 0x5C);
	/*# End OC7 Register Bank*/

	dpll_spi_write( 0x03E5, 0x04);
	dpll_spi_write( 0x03E5, 0x00);

	/*# relock apll 1*/
	dpll_spi_write( 0x00C0, 0x01);
	dpll_spi_write( 0x00D0, 0x18);
	dpll_spi_write( 0x00D0, 0x98);
	/*# relock apll 2 */
	dpll_spi_write( 0x00C0, 0x02);
	dpll_spi_write( 0x00D0, 0x18);
	dpll_spi_write( 0x00D0, 0x98);
	/*# relock apll 3*/ 
	dpll_spi_write( 0x00C0, 0x03);
	dpll_spi_write( 0x00D0, 0x18);
	dpll_spi_write( 0x00D0, 0x98);
	/*# lock the dpll register*/
#endif
	printf("done!\n\r");
	return 0;
}

int atest()
{
	int rv;
	int i;
	char tbuf[16]={0};

	char rbuf[16]={0};

	tbuf[0] = 0x80;
	tbuf[1] = 0x01;
	tbuf[2] = 0x00;
	tbuf[3] = 0x00;

	for(i=0; i<16; i++)
		rbuf[i] = 0;
	
	mspi_config(1,1);

#if 0
	rv = mspi_write8(tbuf, 3);
	printf("rv = %x\n", rv);
	mspi_read8(rbuf, 4);
#endif

	memset(rbuf, 0xff, sizeof(rbuf));
	dpll_spi_read_driver(0x00,rbuf,1);
#if 1
	memset(tbuf, 0, sizeof(tbuf));
	memset(rbuf, 0, sizeof(rbuf));
	tbuf[0] = 0xa5;
	tbuf[1] = 0xbb;
//	dpll_spi_write_driver(0x8, tbuf, 1);
	dpll_spi_read_driver(0x0,rbuf,10);
	dpll_spi_read_driver(0xc0,rbuf,10);
	dpll_spi_read_driver(0xd0,rbuf,10);
#endif
}


