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

extern void bcmgpio_directory_output(int gpio, unsigned char val);
extern void bcmgpio_directory_input(int gpio);
extern unsigned int bcmgpio_get_value(int gpio);

/*add by zhangjiajie 2017-2-27*/
static
void reset_by_gpio2()
{
	/*reset fpga by gpio13*/
	bcmgpio_directory_output(13, 0);
	udelay(20000);
	bcmgpio_directory_output(13, 1);

#if 1 /*send bcm5482 reset info to fpga*/
	udelay(2000000);

	unsigned int data[2];

	bcmgpio_directory_output(11, 1);
	bcmgpio_directory_output(12, 1);
	udelay(10000);
	fpga_spi_read(0x40, data, 2);
	data[0] &= 0xfe; 
	fpga_spi_write(0x40,data, 2);
	udelay(10000);
#endif
//	fpga_spi_read();
	/*reset bcm5482 by gpio11, gpio12*/
	bcmgpio_directory_output(11, 0);
	bcmgpio_directory_output(12, 0);
	udelay(20000);
	bcmgpio_directory_output(11, 1);
	bcmgpio_directory_output(12, 1);

	printf("reset bcm5482, fpga done\n");
#if 1
	memset(data, 0, 2);
	fpga_spi_read(0x40, data, 2);
	data[0] |= 0x1; 
	udelay(10000);
	fpga_spi_write(0x40,data, 2);
	memset(data, 0, 2);
#endif
}

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

int wait_fpga_load_done()
{
	bcmgpio_directory_input(6);
	return bcmgpio_get_value(6);
}

struct mspi_hw *priv;

int mspi_init(void)
{
	u32 rval;
	u32 *ptr;
	
	printf("wait fpga load...\n");

	int i = 0;

	do{

		i++;

		if((i>10)||(wait_fpga_load_done()))
			break;

		udelay(1000000);	

	}while(1);	
	printf("%s\n",i>10?"failed":"successfully");

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

	reset_by_gpio2();	

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
		printf("mspi_writeread8 err rv=%d\n",rv);
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
void fpga_spi_read(unsigned short addr, unsigned char data, size_t count)
{
	unsigned short address = 0;
	unsigned char txbuf[16] = {0};
	unsigned char rxbuf[16] = {0};
	int i = 0;

printf("addr=%d,count=%d\n",addr, count);
	mspi_cs_set(0, 0);

	txbuf[0] = 0x3;
	txbuf[1] = (unsigned char)((addr >> 8) & 0xff);
	txbuf[2] = (unsigned char)((addr) & 0xff);

	mspi_writeread8(txbuf,3,rxbuf,10);
	memcpy(data, rxbuf, count);
#if 0
	printf("data\n");
	for(i=0;i<16;i++)
		printf(" 0x%02x",data[i]);
#endif
	printf("\nrxbuf\n");
	for(i=0;i<16;i++)
		printf(" 0x%02x",rxbuf[i]);
	printf("\n");

	mspi_cs_set(0, 1);
	return 0;
}
void fpga_spi_write(unsigned short addr, unsigned char data, size_t count)
{
	unsigned short address = 0;
	unsigned char txbuf[16] = {0};
	unsigned char rxbuf[16] = {0};
	int i = 0;

	mspi_cs_set(0, 0);

	address = addr;
	txbuf[0] = 0x2;
	txbuf[1] = (unsigned char)((address >> 8) & 0xff);
	txbuf[2] = (unsigned char)((address) & 0xff);

	memcpy(&txbuf[3], data, count);
	mspi_write8(txbuf, count+3);

	mspi_cs_set(0, 1);

	return 0;
}
int dpll_spi_read_driver(unsigned short addr, unsigned char *data, size_t count)
{
	unsigned short address = 0;
	unsigned char txbuf[16] = {0};
	unsigned char rxbuf[16] = {0};
	int i = 0;

	mspi_cs_set(1, 0);
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
	printf("rxbuf\n");
	for(i=0;i<16;i++)
		printf(" 0x%02x",rxbuf[i]);
	printf("\n");

	mspi_cs_set(1, 1);
	return 0;
}
int dpll_spi_write_driver(unsigned short addr, unsigned char *data, size_t count)
{
	unsigned short address = 0;
	unsigned char txbuf[16] = {0};
	unsigned char rxbuf[16] = {0};
	int i = 0;

	mspi_cs_set(1, 0);
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
	mspi_cs_set(1, 1);
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
	bcmgpio_directory_output(cs, en);

	return;
	
}
void dpll_init_pre()
{
	printf("dpll init ...");
	mspi_config(1,1);
	udelay(500000);

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

	/*# dpll 2 - force select local clock ic3*/
	dpll_spi_write( 0x0080, 0x01);
	dpll_spi_write( 0x0084, 0x61);
	dpll_spi_write( 0x0085, 0x74);
	dpll_spi_write( 0x0089, 0x27);

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
	dpll_spi_write( 0x00C0, 0x07);
	dpll_spi_write( 0x00C1, 0x30);
	dpll_spi_write( 0x00C3, 0x80);
	dpll_spi_write( 0x00C8, 0x17);
	dpll_spi_write( 0x00c9, 0x00);
	dpll_spi_write( 0x00ca, 0x00);
	dpll_spi_write( 0x00cb, 0x00);
	
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


