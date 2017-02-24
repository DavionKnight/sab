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

#ifndef _SPI_H_
#define _SPI_H_

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>		
#include <sys/types.h>	
#include <sys/stat.h>	
#include <sys/time.h>	
#include <time.h>
#include <unistd.h>		
#include <errno.h>
#include <signal.h>
#include <termios.h>
#include <pthread.h>
 
#include <sys/ipc.h>  
#include <sys/sem.h>  
#include <semaphore.h>
#include <sys/shm.h>  

#include <linux/types.h>	/* sys/types.h and linux/types.h are oil and water */


#define	SPI_CPHA		0x01			/* clock phase */
#define	SPI_CPOL		0x02			/* clock polarity */
#define	SPI_MODE_0		(0|0)			/* (original MicroWire) */
#define	SPI_MODE_1		(0|SPI_CPHA)
#define	SPI_MODE_2		(SPI_CPOL|0)
#define	SPI_MODE_3		(SPI_CPOL|SPI_CPHA)
#define	SPI_CS_HIGH		0x04			/* chipselect active high? */
#define	SPI_LSB_FIRST	0x08			/* per-word bits-on-wire */
#define	SPI_3WIRE		0x10			/* SI/SO signals shared */
#define	SPI_LOOP		0x20			/* loopback mode */

#define SPI_FPGA_WR_SINGLE 0x01
#define SPI_FPGA_WR_BURST  0x02
#define SPI_FPGA_RD_BURST  0x03
#define SPI_FPGA_RD_SINGLE 0x05

#define FPGA_REG_ADDR_MAX	0xffff

struct spi_reg_t {
        unsigned int mode;
        unsigned int event;
        unsigned int mask;
        unsigned int command;
        unsigned int transmit;
        unsigned int receive;
        unsigned int res[2];
        unsigned int csmode[4];
};

struct spi_device {
	volatile struct spi_reg_t		*spi_reg;
	unsigned int		max_speed_hz;
	unsigned char		chip_select;
	unsigned char		mode;
	unsigned char 		bits_per_word;
	int					irq;
};


#ifdef H20RN181

typedef unsigned int	int32;

union semun {
   short val;          		/*SETVAL用的值*/
   struct semid_ds* buf; 	/*IPC_STAT、IPC_SET用的semid_ds结构*/
   unsigned short* array; 	/*SETALL、GETALL用的数组值*/
//   struct seminfo *buf;   /*为控制IPC_INFO提供的缓存*/
};

typedef struct chipc_spi_softc_s {
    unsigned int id;        /* SPI device ID */
    unsigned char buf[32];        /* Software control info */
    unsigned int buf_index;        /* the buffer length */
    unsigned int states;        /* start ready to do read/write transaction */
} chipc_spi_softc_t;

/*macro definition*/
#define MODEBITS (SPI_CPOL | SPI_CPHA | SPI_LSB_FIRST)



/* SPI device ID  :
 *  - Keystone has three SPI device selected.
 */
typedef enum cc_spi_id_e {
    CC_SPI_SS0 = 0,
    CC_SPI_SS1,
    CC_SPI_SS2,
    CC_SPI_NUM_DEV
} cc_spi_id_t;

/* SPI Mode :
 *  - Keystone has four SPI mode control.
 */
typedef enum cc_spi_mode_e {
    CC_SPI_MODE_CPOL_0_CPHA_0 = 0,
    CC_SPI_MODE_CPOL_0_CPHA_1,
    CC_SPI_MODE_CPOL_1_CPHA_0,
    CC_SPI_MODE_CPOL_1_CPHA_1,
    CC_SPI_NUM_MODE
} cc_spi_mode_t;

#define CC_SPI_ID_IS_VALID(_id)    ((_id) < CC_SPI_NUM_DEV)

#define CC_SPI_MODE_IS_VALID(_id)    ((_id) < CC_SPI_NUM_MODE)

/* SPI reg-spi_mode_ctrl :field defintiion */
#define CC_SPIMCTRL_MODE_MASK     0x3        /* SPI mode valid bits */
#define CC_SPIMCTRL_MODE_0     0x0        /* SPI mode : CPOL = 0, CPHA = 0 */
#define CC_SPIMCTRL_MODE_1     0x1        /* SPI mode : CPOL = 0, CPHA = 1 */
#define CC_SPIMCTRL_MODE_2     0x2        /* SPI mode : CPOL = 1, CPHA = 0 */
#define CC_SPIMCTRL_MODE_3     0x3        /* SPI mode : CPOL = 1, CPHA = 1 */
#define CC_SPIMCTRL_ACKEN_MASK     0x4        /* SPI RACK enable valid bits */
#define CC_SPIMCTRL_ACKEN     0x4        /* SPI RACK */
#define CC_SPIMCTRL_ENDIAN_MASK     0x8        /* SPI endiag valid bits */
#define CC_SPIMCTRL_BE     0x8        /* SPI endiag : big endian mode */
#define CC_SPIMCTRL_LE     0x0         /* SPI endiag : little endian mode */
#define CC_SPIMCTRL_LSB_MASK     0x100        /* SPI LSB first valid bits */
#define CC_SPIMCTRL_LSB_FIRST     0x100        /* SPI LSB : big endian mode */
#define CC_SPIMCTRL_MSB_FIRST    0x0         /* SPI LSB : little endian mode */
#define CC_SPIMCTRL_CLK_MASK     0xf0        /* SPI clock divider valid bits */

/* SPI reg-spi_config :field defintiion */
#define CC_SPICFG_MASK     0x9f87e1fb        /* SPI config valid bits */   

/* SPI reg-spi_fifo_io :field defintiion */
#define CC_SPIFIFOIO_MASK     0xffffffff        /* SPI FIFO IO valid bits */   

/* SPI reg-spi_status :field defintiion */
#define CC_SPISTS_MASK     0x37        /* SPI status valid bits */
#define CC_SPISTS_READY    0x1        /* SPI ready */
#define CC_SPISTS_RDONE    0x2       /* SPI transaction done and FIFO is empty when reading data */
#define CC_SPISTS_INTFLAG   0x4    /* SPI Interrupt Flag */
#define CC_SPISTS_FIFOE    0x10      /* SPI FIFO empty */
#define CC_SPISTS_FIFOF  0x20       /* SPI FIFO full */

/* Serial IO eanble MASK and bit value definition */
#define CC_SERIAL_IO_SPI_INTR_MASK       0x4
#define CC_SERIAL_IO_SPI0_MASK       0x4
#define CC_SERIAL_IO_SPI1_MASK       0xc
#define CC_SERIAL_IO_SPI2_MASK       0x1c

extern struct spi_device spidev;
extern chipc_spi_softc_t spi_softc;
//void * libm_handle = NULL;
extern int spi_flag;

/******************************dplle1.c******************************/
#define fpga_chip_select_addr 	9 
#define DS31400_CHIP			0
#define E1_CHIP					1

#define DS31400_REG_ADDR_MAX	0xffff

/*****************************fpga.c&dplle1.c********************************/

/***********************************epcs.c***********************************/

/*********************************starting spidrv.c***********************************/
#define ERROR 				-1

/*SPI register definition*/
#define SPI_REGISTER_BASE	0x18000000
#define SPI_MODE_CTRL 		0x18000280
#define SPI_CONFIG 			0x18000284
#define SPI_FIFO_IO			0x18000288
#define SPI_STATUS 			0x1800028C
#define SERIAL_IO_INTMASK 	0x180002F0
#define SERIAL_IO_SEL 		0x180002F4

#define BCMDBG_ERR
#ifdef BCMDBG_ERR
#define	SPI_ERROR(args)	printf args
#else
#define	SPI_ERROR(args)
#endif	/* BCMDBG_ERR */

#ifdef BCMDBG
#define	SPI_MSG(args)	printf args
#else
#define	SPI_MSG(args)
#endif	/* BCMDBG */



#define SPI_FIFO_MAX_SIZE 32
#define SPI_INTFLAG_TIMEOUT 5000

#define SPI_STATES_DISABLE 0x0
#define SPI_STATES_ENABLE 0x1
#define SPI_STATES_WRITE 0x2
#define SPI_STATES_READ 0x4

/* SPI mode control definition */
#define SPI_MODE_CTRL_MODE     	0x0        /* SPI Mode (CPOL, CPHA) */
#define SPI_MODE_CTRL_ACKEN     0x1        /* SPI RACK enable */
#define SPI_MODE_CTRL_ENDIAN    0x2        /* SPI Big Endian enable */
#define SPI_MODE_CTRL_CLOCK     0x4        /* SPI Clock divider parameter */
#define SPI_MODE_CTRL_LSBEN     0x10        /* SPI LSB first enable */

#define SPI_CCD_MAX      0xf     /* max N value for spi clock divider parameter(CCD) */

/* reutrn value for SPI driver */
#define SPI_ERR_NONE          0
#define SPI_ERR_TIMEOUT       -1
#define SPI_ERR_INTERNAL      -2
#define SPI_ERR_PARAM         -3
#define SPI_ERR_UNAVAIL       -4
#define SPI_ERR_UNKNOW        -5

#define _DD_MAKEMASK1(n) (1 << (n))
#define _DD_MAKEMASK(v,n) ((((1)<<(v))-1) << (n))
#define _DD_MAKEVALUE(v,n) ((v) << (n))
#define _DD_GETVALUE(v,n,m) (((v) & (m)) >> (n))

/* SPICONFIG: SPI Configuration Register (0x284, R/W) */
#define S_SPICFG_SS             0                       /* SPI SS (device(n)) enable */
#define V_SPICFG_SS(x)         _DD_MAKEVALUE(x,S_SPICFG_SS)

#define S_SPICFG_RDC           3                       /* SPI Read byte count */
#define V_SPICFG_RDC(x)        _DD_MAKEVALUE(x,S_SPICFG_RDC)

#define S_SPICFG_WDC           13                       /* SPI Write data byte count */
#define V_SPICFG_WDC(x)        _DD_MAKEVALUE(x,S_SPICFG_WDC)

#define S_SPICFG_WCC           23                       /* SPI Write command byte count */
#define V_SPICFG_WCC(x)        _DD_MAKEVALUE(x,S_SPICFG_WCC)

#define V_SPICFG_START         _DD_MAKEVALUE(1,31) /* Start SPI transfer */


/* meaning of other SR_* bits may differ between vendors */
#define	SR_BP0			4	/* Block protect 0 */
#define	SR_BP1			8	/* Block protect 1 */
#define	SR_BP2			0x10	/* Block protect 2 */
#define	SR_SRWD			0x80	/* SR write protect */


#define FPGA_READ_ENABLE	0x0132
#define FPGA_DEV_SLAVE_ADD	0x0134
#define FPGA_READ_DATA		0x0138
#define FPGA_DATA_VALID		0x013a
/*spidrv define*/
int mips53003_spi_mode_set(unsigned int flags, unsigned int value);
int mips53003_spi_interface_select(cc_spi_id_t id, int en);
unsigned int mips53003_spi_status_get(void);
int mips53003_spi_control(cc_spi_id_t id, unsigned int rdc, unsigned int wdc, unsigned int wcc);
int mips53003_spi_enable(cc_spi_id_t id);
int mips53003_spi_disable(cc_spi_id_t id);
int mips53003_spi_buf_write(unsigned char *buf, int len);
int mips53003_spi_write_for_read(unsigned char *data, int len, int rxlen);
#endif

#endif
