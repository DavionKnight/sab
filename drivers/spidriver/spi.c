/*
*  COPYRIGHT NOTICE
*  Copyright (C) 2016 HuaHuan Electronics Corporation, Inc. All rights reserved
*
*  Author       		:yuhongyue
*  File Name        	:spidrv.c
*  Create Date        	:2016/11/5 
*  Last Modified      	:2016/11/5 
*  Description    	:
*/
#include "spi.h"

#ifdef H20RN181

void *mapbase = NULL;

chipc_spi_softc_t spi_softc;

/* --------- SPI Bus level(Low Level) driver --------- 
 *  The drivers in this level will combine the Keystone's register 
 *      configuration to proceed the requesting process.
 *  - interface between SPI Core driver and High level driver.
 */

/* Function : spi_mode_set
 *  - Allow user to set SPI device mode (CPOL, CPHA).
 * Return : 
 * Note :
 *     flags = SPI_MODE_CTRL_MODE
 *                SPI_MODE_CTRL_ACKEN
 *                SPI_MODE_CTRL_ENDIAN
 *                SPI_MODE_CTRL_CLOCK
 *                SPI_MODE_CTRL_LSBEN
 */
int mips53003_spi_mode_set(unsigned int flags, unsigned int value)
{    
    unsigned int mask = 0, val = 0;
    int fd;
	unsigned int reg_val;
	void *mode_mapbase,*virt_addr;

    switch(flags) {
        case SPI_MODE_CTRL_MODE :
            if (!CC_SPI_MODE_IS_VALID(value)){
                printf("%s: Invalid SPI device MODE!\n");
                return SPI_ERR_PARAM;
            }
            mask = CC_SPIMCTRL_MODE_MASK;

            switch(value) {
                case CC_SPI_MODE_CPOL_0_CPHA_0 :
                    val = CC_SPIMCTRL_MODE_0;
                    break;
                case CC_SPI_MODE_CPOL_0_CPHA_1 :
                    val = CC_SPIMCTRL_MODE_1;
                    break;
                case CC_SPI_MODE_CPOL_1_CPHA_0 :
                    val = CC_SPIMCTRL_MODE_2;
                    break;
                case CC_SPI_MODE_CPOL_1_CPHA_1 :
                    val = CC_SPIMCTRL_MODE_3;
                    break;
                default :
                    return SPI_ERR_PARAM;
            }
            
            break;
        case SPI_MODE_CTRL_ACKEN:
            mask = CC_SPIMCTRL_ACKEN_MASK;
            if (value) {
                val = CC_SPIMCTRL_ACKEN;
            } else {
                val = 0;
            }
            break;
        case SPI_MODE_CTRL_ENDIAN:
            mask = CC_SPIMCTRL_ENDIAN_MASK;
            if (value) {
                val = CC_SPIMCTRL_BE;
            } else {
                val = CC_SPIMCTRL_LE;
            }
            break;
        case SPI_MODE_CTRL_CLOCK:
            mask = CC_SPIMCTRL_CLK_MASK;
            val = value;
            break;
        case SPI_MODE_CTRL_LSBEN:
            mask = CC_SPIMCTRL_LSB_MASK;
            if (value) {
                val = CC_SPIMCTRL_LSB_FIRST;
            } else {
                val = CC_SPIMCTRL_MSB_FIRST;
            }
            break;
        default :
            return SPI_ERR_PARAM;
    }

    SPI_MSG(("%s: flags = 0x%x, value = 0x%x\n", __func__, flags, val));

    /* select the spi interface */
	virt_addr = mapbase + (SPI_MODE_CTRL & MAP_MASK);
	if(mask || val)
		*((unsigned int *) virt_addr) = (*((unsigned int *) virt_addr) & ~mask) | val;
	reg_val = *((unsigned int *) virt_addr);
//    si_spi_modectrl(mask, val);

    return SPI_ERR_NONE;
}

/* Function : spi_interface_select 
 *  - Enable Keystone's SPI port.
 * Return :
 * Note :
 */
int mips53003_spi_interface_select(cc_spi_id_t id, int en) 
{
	void *sel_mapbase,*virt_addr;
	unsigned int  reg_val = 0, temp;
    if (!CC_SPI_ID_IS_VALID(id)){
        SPI_ERROR(("%s: Invalid SPI device ID!\n", __func__));
	return SPI_ERR_PARAM;
    }
	
	/* config spi port && select the spi interface*/
	virt_addr = mapbase + (SERIAL_IO_SEL & MAP_MASK); 
	reg_val = *((unsigned int *) virt_addr);
    temp = (id == CC_SPI_SS0) ? CC_SERIAL_IO_SPI0_MASK : 
            (id == CC_SPI_SS1) ? CC_SERIAL_IO_SPI1_MASK : 
            (id == CC_SPI_SS2) ? CC_SERIAL_IO_SPI2_MASK : 0;
    if (en){
        reg_val |= temp;
    } else {
        reg_val &= ~temp;
    }
	*((unsigned int *) virt_addr) = reg_val;	
    SPI_MSG(("%s: %s SPI_%d is DONE!\n", 
            __func__, (en) ? "Enable" : "Disable", id));
    return SPI_ERR_NONE;
}

 /* Function : spi_status_get 
 *     - Get spi device status (offset 0x28c in chipcommon).
 * Return :
 *     - Return the current SPI status.
 * Note :
 */
unsigned int mips53003_spi_status_get(void) 
{
	unsigned int spi_status;
	int i;
	unsigned int    reg_val = 0;
	void *int_clr_mapbase,*virt_addr;

	virt_addr = mapbase + (SPI_STATUS & MAP_MASK);

	for(i=0;i<800;i++);

	reg_val = *((unsigned int *) virt_addr);

	spi_status = (reg_val & CC_SPISTS_MASK);

	return spi_status;

}

/* Function : mips53003_spi_control 
 *  - Start to do Read/write transaction on FIFO to Keystone's SPI device.
 * Return :
 * Note :
 */
int mips53003_spi_control(cc_spi_id_t id, uint rdc, uint wdc, uint wcc)
{
    int rv = SPI_ERR_NONE;
    unsigned int reg_val = 0;
	void *config_mapbase,*virt_addr;
	virt_addr = mapbase + (SPI_CONFIG & MAP_MASK);
	
    if (!CC_SPI_ID_IS_VALID(id)){
        SPI_ERROR(("%s: Invalid SPI device ID!\n", __func__));
        return SPI_ERR_PARAM;
    }

    /* (RdDatCnt/WrDatCnt/WrCmdCnt) <= 32 bytes */
    if ((rdc > SPI_FIFO_MAX_SIZE) || 
        (wdc > SPI_FIFO_MAX_SIZE) || 
        (wcc > SPI_FIFO_MAX_SIZE)) {
        SPI_ERROR(("%s: Config failed : can not exceed 32 bytes!\n", __func__));
        return SPI_ERR_PARAM;
    }

    /* (WrCmdCnt + WrDatCnt) <= 32 bytes */
    if ((wcc + wdc) > SPI_FIFO_MAX_SIZE) {
        SPI_ERROR(("%s: Config failed : (WrCmdCnt+WrDatCnt) can not exceed 32 bytes!\n",
            __func__));
        return SPI_ERR_PARAM;
    }
    
    reg_val = (V_SPICFG_SS(id) |
                 V_SPICFG_RDC(rdc) |
                 V_SPICFG_WDC(wdc) |
                 V_SPICFG_WCC(wcc) |
                 V_SPICFG_START);

	if(CC_SPICFG_MASK || reg_val)
		*((unsigned int *) virt_addr) = (*((unsigned int *) virt_addr) & ~CC_SPICFG_MASK) | reg_val;

    return rv;
}

int
mips53003_spi_write(unsigned char *data, int len, int rxlen) 
{
    int idx, m;
    unsigned int value,ret;
	int i;
	void *fifo_mapbase,*virt_addr;
	struct spi_device *spi = &spidev;
	ret = mips53003_spi_enable(spi->chip_select);
	if(0 != ret)
		printf("mips53003_spi_enable error!!!\n");
    /* len < 32 bytes */
    if (len > SPI_FIFO_MAX_SIZE) {
        SPI_ERROR(("%s: Write failed : can not exceed 32 bytes!\n", __func__));
        return SPI_ERR_PARAM;
    }

    /* Write command/data to Keystone's SPI FIFO */

    value = 0;
    for (idx = 0 ; idx < len ; idx++) {
        m = idx % 4;
        value |= (*data << ((3-m)*8));
        data++;
        if ((m == 3) || (idx == (len - 1))) {
			virt_addr = mapbase + (SPI_FIFO_IO & MAP_MASK);
			for(i=0;i<800;i++);
			if(CC_SPIFIFOIO_MASK || value)
			{
				*((unsigned int *) virt_addr) = value;
			}
//            spi_fifo_write(CC_SPIFIFOIO_MASK, value);
            value = 0;
        }
    }

#if 1
	ret = mips53003_spi_control(spi->chip_select, rxlen, len, 0);
	if(0 != ret)
	{
		printf("mips53003_spi_control error!!!\n");
		return SPI_ERR_PARAM;
	}
#endif
#if 1
	ret = mips53003_spi_disable(spi->chip_select);
	if(0 != ret)
		printf("mips53003_spi_disable error!!!\n");
#endif
    return SPI_ERR_NONE;
}

/* --------- SPI High level driver --------- 
 * The drivers in this level also know as general SPI driver. 
 *  - Request the SPI read/write on SPI device through SPI driver.
 * Function : spi_enable 
 * Function : spi_disable 
 * Function : spi_read
 * Function : spi_buf_write

 */


/* Function : spi_enable
 *  - Enable the SPI device :
 *       initialize the sw_info : get ready to do read/write transaction on FIFO.
 * Return :
 * Note : 
 */
int mips53003_spi_enable(cc_spi_id_t id)
{
    chipc_spi_softc_t *s = &spi_softc;

    //SPI_INIT_CHK;

    if (!CC_SPI_ID_IS_VALID(id)){
        SPI_ERROR(("%s: Invalid SPI device ID!\n", __func__));
        return SPI_ERR_PARAM;
    }

    memset(s->buf, 0, sizeof(s->buf));
    s->id = id;
    s->buf_index = 0;
    s->states = SPI_STATES_ENABLE;

    return SPI_ERR_NONE;
}

/* Function : spi_disable
 *  - Disable the SPI device : 
 *       finished read/write transaction on FIFO (set states = SPI_STATES_DISABLE).
 * Return :
 * Note : 
 *     For SPI write operation (while s->states == SPI_STATES_WRITE), 
 *     we start to do really write transaction on FIFO 
 *     when we call function spi_disable. 
 *
 *     For SPI read operation, we do really read transaction on FIFO 
 *     when we call function spi_read.
 */
int mips53003_spi_disable(cc_spi_id_t id)
{
    chipc_spi_softc_t *s = &spi_softc;

    //SPI_INIT_CHK;

    if (!CC_SPI_ID_IS_VALID(id)){
        SPI_ERROR(("%s: Invalid SPI device ID!\n", __func__));
        return SPI_ERR_PARAM;
    }

    /* 
     *  if s->states == SPI_STATES_WRITE, start to do write transaction on FIFO
     *  ReadDataCnt = 0, WriteDataCnt = s->buf_index, WriteCmdCnt = 0
     */
    if (s->states == SPI_STATES_WRITE) {
        if (mips53003_spi_write(s->buf, s->buf_index,0)) {
            SPI_ERROR(("%s: Failed to write data to SPI FIFO at SPI_%d!\n", __func__, s->id));
            return SPI_ERR_PARAM;
        }
/*        if (mips53003_spi_control(s->id, 0, s->buf_index, 0)) {
            SPI_ERROR(("%s: Failed to start write transaction at SPI_%d!\n", __func__, s->id));
            return SPI_ERR_PARAM;
        }*/
    }

    s->states = SPI_STATES_DISABLE;
    
    return SPI_ERR_NONE;
}

/* Function : spi_buf_write 
 *  - Write operation through Keystone's SPI.
 * Return :
 * Note : 
 *     Write the SPI device command(or data) and keep in software buffer first.
 *     For SPI write operation (while s->states == SPI_STATES_WRITE), 
 *     we start to do really write transaction on FIFO 
 *     when we call function spi_disable. 
 */
int mips53003_spi_buf_write(unsigned char *buf, int len)
{
    chipc_spi_softc_t *s = &spi_softc;
    int i = 0;

    //SPI_INIT_CHK;

    if (s->states != SPI_STATES_ENABLE && s->states != SPI_STATES_WRITE) {
        SPI_ERROR(("%s: Failed to do SPI read/write operation!\n", __func__));
        return SPI_ERR_PARAM;
    }

    /* The Maximum size of SPI's FIFO is 32 bytes */
    if ((s->buf_index + len) > SPI_FIFO_MAX_SIZE) {
        SPI_ERROR(("%s: Failed to write data to FIFO : the size of FIFO > 32 bytes!\n", 
            __func__));
        return SPI_ERR_PARAM;
    }

    /* Write SPI command or data and keep in software first */
    while (len) {
        s->buf[s->buf_index++] = buf[i++];
        len--;
    }
    s->states = SPI_STATES_WRITE;
	
    return SPI_ERR_NONE;
}

/* Function : mips53003_spi_write 
 *  - Write data to Keystone's SPI device via FIFO.
 * Return :
 * Note :
 */
int mips53003_spi_write_for_read(unsigned char *data, int len, int rxlen) 
{
    int idx, m;
    unsigned int value,ret;
	int i;
	void *fifo_mapbase,*virt_addr;
	struct spi_device *spi = &spidev;
	
    /* len < 32 bytes */
    if (len > SPI_FIFO_MAX_SIZE) {
        SPI_ERROR(("%s: Write failed : can not exceed 32 bytes!\n", __func__));
        return SPI_ERR_PARAM;
    }

    /* Write command/data to Keystone's SPI FIFO */

    value = 0;
    for (idx = 0 ; idx < len ; idx++) {
        m = idx % 4;
        value |= (*data << ((3-m)*8));
        data++;
        if ((m == 3) || (idx == (len - 1))) {
			virt_addr = mapbase + (SPI_FIFO_IO & MAP_MASK);
			for(i=0;i<800;i++);
			if(CC_SPIFIFOIO_MASK || value)
			{
				*((unsigned int *) virt_addr) = value;
			}
//            spi_fifo_write(CC_SPIFIFOIO_MASK, value);
            value = 0;
        }
    }

#if 1
	ret = mips53003_spi_control(spi->chip_select, rxlen, len, 0);
	if(0 != ret)
	{
		printf("mips53003_spi_control error!!!\n");
		return SPI_ERR_PARAM;
	}
#endif
    return SPI_ERR_NONE;
}

#endif

#if defined(H20RN1000)||defined(H20RN2000) 
/* SPI Controller mode register definitions */
#define CSMODE_CI_INACTIVEHIGH  (1 << 31)
#define CSMODE_CP_BEGIN_EDGECLK (1 << 30)
#define CSMODE_REV              (1 << 29)
#define CSMODE_DIV16            (1 << 28)
#define CSMODE_PM(x)            ((x) << 24)
#define CSMODE_POL_1            (1 << 20)
#define CSMODE_LEN(x)           ((x) << 16)
#define SPMODE_ENABLE           (1 << 31)

/* SPIM register values */
#define SPIM_NE         0x00000200      /* Not empty */
#define SPIM_NF         0x00000100      /* Not full */

#define SPI_EV_NE       (0x80000000 >> 22)      /* Receiver Not Empty */
#define SPI_EV_NF       (0x80000000 >> 23)      /* Transmitter Not Full */

#define SPI_MODE_LOOP   (0x80000000 >> 1)       /* Loopback mode */
#define SPI_MODE_REV    (0x80000000 >> 5)       /* Reverse mode - MSB first */
#define SPI_MODE_MS     (0x80000000 >> 6)       /* Always master */
#define SPI_MODE_EN     (0x80000000 >> 7)       /* Enable interface */

#define SPI_TIMEOUT             10000
                
#define CSMODE_PM_MAX           (0xF)
#define CSMODE_PM_MIN           (0x2)
        
static void spi_reg_read(const volatile unsigned *addr, unsigned int *val)
{
        __asm__ __volatile__("lwz%U1%X1 %0,%1; twi 0,%0,0; isync"
                             : "=r" (*val) : "m" (*addr));
}
static void spi_reg_write(volatile unsigned *addr, unsigned int val)
{
        __asm__ __volatile__("stw%U0%X0 %1,%0; sync"
                             : "=m" (*addr) : "r" (val));
}

int spi_transfer(struct spi_device *spidev, unsigned char *txbuf, unsigned char *rxbuf, int len)
{
	unsigned int txword = 0, rxword  = 0;
	unsigned int event = 0, command = 0;
	int isRead = 0, tm = 0, i = 0, ret = 0, nfcount = 0;

	command = (spidev->chip_select<<30)|(len - 1);
	/*enable chipselect and tell spi the length of data*/
	spi_reg_write(&spidev->spi_reg->command, command);

	/*enable rx ints*/
	spi_reg_write(&spidev->spi_reg->mask, SPIM_NE);

	do{
		spi_reg_read(&spidev->spi_reg->event, &event);
		if(event & SPI_EV_NF)
		{
			nfcount = 0;
			txword = 0;
			txword = *(unsigned int *)txbuf;
			txbuf += 4;

			spi_reg_write(&spidev->spi_reg->transmit, txword);

		}
		else
		{
			if(nfcount ++>1000)
				break;
			continue;
		}
		/*here wait tx data over*/
		//				usleep(5);
//printf("txword=0x%08x, len=%d\n",txword, len);
		for(tm = 0, isRead = 0; tm < SPI_TIMEOUT; tm++)
		{
			spi_reg_read(&spidev->spi_reg->event, &event);
			if (event & SPI_EV_NE) {
				/*here wait rx over*/
				for(i = 0; i < SPI_TIMEOUT; i++)
				{
					spi_reg_read(&spidev->spi_reg->event, &event);
					if(((event>>24)&0x3f) >= 4)
						break;
				}
				spi_reg_read(&spidev->spi_reg->receive, &rxword);
//				printf("in transfer rxword=0x%08x event=0x%08x\n",rxword,event);
				isRead = 1;

				*(unsigned int *) rxbuf = rxword;
				rxbuf += 4;
			}
			/*
			 * Only bail when we've had both NE and NF events.
			 * This will cause timeouts on RO devices, so maybe
			 * in the future put an arbitrary delay after writing
			 * the device.	Arbitrary delays suck, though...
			 */
			if (isRead)
			{
				len -= 4;
				break;
			}
		}
		if (tm >= SPI_TIMEOUT)
		{
			printf("spi error:Time out when spi transfer\n");
			ret = -1;
			break;
		}
	}while(len>0);

	/*disable rx ints*/
	spi_reg_write(&spidev->spi_reg->mask, 0);

	return ret;

}
#define SPIMODE_TXTHR(x)        ((x) << 8)
#define SPIMODE_RXTHR(x)        ((x) << 0)
#define SPMODE_INIT_VAL (SPIMODE_TXTHR(4) | SPIMODE_RXTHR(3))
#define SPMODE_ENABLE           (1 << 31)

#define CSMODE_POL_1            (1 << 20)
#define CS_BEF(x)               ((x) << 12)
#define CS_AFT(x)               ((x) << 8)
#define CS_CG(x)                ((x) << 3)
#define CSMODE_CI_INACTIVEHIGH  (1 << 31)
/*
 * Default for SPI Mode:
 *      SPI MODE 0 (inactive low, phase middle, MSB, 8-bit length, slow clk
 */
#define CSMODE_INIT_VAL (CSMODE_POL_1 | CS_BEF(0) | CS_AFT(0) | CS_CG(1))  | CSMODE_CI_INACTIVEHIGH

int spi_dev_init(struct spi_device *spidev)
{
        unsigned int i = 0;

        if(NULL == spidev->spi_reg)
                return -1;

        /* SPI controller initializations */
        spi_reg_write(&spidev->spi_reg->mode, 0);
        spi_reg_write(&spidev->spi_reg->mask, 0);
        spi_reg_write(&spidev->spi_reg->command, 0);
        spi_reg_write(&spidev->spi_reg->event, 0xffffffff);

        spi_reg_write(&spidev->spi_reg->mode, SPMODE_INIT_VAL | SPMODE_ENABLE);
        /* init CS mode interface */
        for (i = 0; i < 4; i++)
                spi_reg_write(&spidev->spi_reg->csmode[i],CSMODE_INIT_VAL);

        return 0;
}


int spi_setup(struct spi_device *spidev)
{
        unsigned int hw_mode;
        unsigned long flags;
        unsigned int regval;
        unsigned char bits_per_word, pm, cs_sel = spidev->chip_select;
        unsigned int hz = 0;
        unsigned int spibrg = 198000000;

        if(!spidev->max_speed_hz)
                return -1;

        if(!spidev->bits_per_word)
                spidev->bits_per_word = 8;

        spi_reg_read(&spidev->spi_reg->csmode[spidev->chip_select], &hw_mode);
        hw_mode &= ~(CSMODE_CP_BEGIN_EDGECLK | CSMODE_CI_INACTIVEHIGH | CSMODE_REV);

        if (spidev->mode & SPI_CPHA)
                hw_mode |= CSMODE_CP_BEGIN_EDGECLK;
        if (spidev->mode & SPI_CPOL)
                hw_mode |= CSMODE_CI_INACTIVEHIGH;
        if (!(spidev->mode & SPI_LSB_FIRST))
                hw_mode |= CSMODE_REV;

        if (!hz)
                hz = spidev->max_speed_hz;

        /* mask out bits we are going to set */
        hw_mode &= ~(CSMODE_LEN(0xF) | CSMODE_DIV16 | CSMODE_PM(0xF));

        //hw_mode |= CSMODE_LEN(bits_per_word) | CSMODE_INIT_VAL;
        hw_mode |= CSMODE_LEN(spidev->bits_per_word-1);
        if ((spibrg / hz) > 32) {
                hw_mode |= CSMODE_DIV16;
                pm = spibrg / (hz * 32);
                if (pm > CSMODE_PM_MAX) {
                        pm = CSMODE_PM_MAX;
                        printf("Requested speed is too low: %d Hz. Will use %d Hz instead.\n",
                                hz, spibrg / 32 * 16);
                }
        } else {
                pm = spibrg / (hz * 2);
/*delete by zhangjj 2015-11-12 change spi max HZ*/
#if 0
                if (pm < CSMODE_PM_MIN)
                        pm = CSMODE_PM_MIN;
#endif
/*delete end*/
        }

        hw_mode |= CSMODE_PM(pm);
        /* Reset the hw mode */
        spi_reg_read(&spidev->spi_reg->mode, &regval);

        /* Turn off SPI unit prior changing mode */
        spi_reg_write(&spidev->spi_reg->mode, regval & ~SPMODE_ENABLE);
        spi_reg_write(&spidev->spi_reg->csmode[cs_sel], hw_mode);
        spi_reg_write(&spidev->spi_reg->mode, regval);
        return 0;
}

#endif
#ifdef H20RN181_ARM
#define NUM_TXRAM                           32
#define NUM_RXRAM                           32
#define NUM_CDRAM                           16

/* register def */
#define ENABLE_OVERRIDE_SPI_MASTER_SLAVE_MODE 	(1<<5)
#define SPI_MASTER_SLAVE_MODE			(1<<2)

/* MSPI registers */
struct mspi_hw {
    unsigned int             mspi_spcr0_lsb;               /* 0x000 */
    unsigned int             mspi_spcr0_msb;               /* 0x004 */
    unsigned int             mspi_spcr1_lsb;               /* 0x008 */
    unsigned int             mspi_spcr1_msb;               /* 0x00c */
    unsigned int             mspi_newqp;                   /* 0x010 */
    unsigned int             mspi_endqp;                   /* 0x014 */
    unsigned int             mspi_spcr2;                   /* 0x018 */
    unsigned int             reserved0;                    /* 0x01c */
    unsigned int             mspi_status;             	  /* 0x020 */
    unsigned int             mspi_cptqp;                   /* 0x024 */
    unsigned int             reserved1[6];            	  /* 0x028 */
    unsigned int             mspi_txram[NUM_TXRAM];        /* 0x040 */
    unsigned int             mspi_rxram[NUM_RXRAM];        /* 0x0c0 */
    unsigned int             mspi_cdram[NUM_CDRAM];        /* 0x140 */
    unsigned int             cmicm_bspi_mast_n_boot;       /* 0x180 */
    unsigned int             cmim_bspi_busy_status;        /* 0x184 */
};

struct mspi_hw *priv;

#define MAP_SIZE            4096UL
#define MAP_MASK            (MAP_SIZE - 1)

int fd_mmap;
//#define CMIC_OVERRIDE_STRAP		    0x3210234
#define CMIC_OVERRIDE_STRAP		    0x3210000
unsigned int *cmic_override_strap;
static int arm5626x_mspi_mmap()
{
        if((fd_mmap = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
        {
                return -1;
        }
        cmic_override_strap = (unsigned int *)(0x234+mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_mmap, CMIC_OVERRIDE_STRAP));
        if(NULL == cmic_override_strap)
                return -1;
        return 0;
}

static void arm5626x_mspi_mmap_exit()
{
        munmap((void *)(cmic_override_strap-0x234),MAP_SIZE);
        close(fd_mmap);
}
int arm5626x_mspi_init(struct spi_device *spidev)
{
	unsigned int rval;
	unsigned int *ptr;

	if(-1 == arm5626x_mspi_mmap())
	{
		printf("arm5626x_mspi_mmap error\n");
		return -1;
	}
printf("arm5626x_mspi_init after mmap\n");
	ptr = (unsigned int*)cmic_override_strap;
	priv =	 (volatile struct mspi_hw *)spidev->spi_reg;

	/* Claim the SPI bus */
	rval = *ptr;
printf("arm5626x_mspi_init after mmap\n");
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
	
	arm5626x_mspi_mmap_exit();
	//atest();
		
	return 0;
}
int arm5626x_mspi_config(int cpol, int cpha) 
{
	unsigned int rval;

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
int arm5626x_mspi_writeread8( unsigned char *wbuf, int wlen, unsigned char *rbuf, int rlen)
{

    	int i, tlen, rv = 0;
    	unsigned char *datptr;
    	unsigned int rval=0;
	
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
		priv->mspi_txram[2*i] = (unsigned int) *datptr;

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
            *datptr = ((unsigned char) priv->mspi_rxram[2*i+1] & 0xff);
            datptr++;
        }
    }
    return 0;
}
int arm5626x_mspi_read8( unsigned char *buf, int len)
{
    return  arm5626x_mspi_writeread8( NULL, 0, buf, len);
}

int arm5626x_mspi_write8( unsigned char *buf, int len)
{
    return  arm5626x_mspi_writeread8(buf, len, NULL, 0);
}

#endif






