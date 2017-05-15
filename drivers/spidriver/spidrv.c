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
#include "spidrv.h"

struct spi_device spidev;
int spidrv_semid;
struct sembuf spidrv_sembufLock, spidrv_sembufUnlock;
static int spidrv_initialized = 0;


#define MAP_SIZE 			4096UL
#define MAP_MASK 			(MAP_SIZE - 1)  

#ifdef H20RN181

union semun arg;
extern void *mapbase;
int fd_mmap;


/* Function : spidrv_write 
 *  - Write data to Keystone's SPI device via FIFO.
 * Return :
 * Note :
 */

int
spidrv_write(unsigned char *data, int len, int rxlen, unsigned char chip_select) 
{
    int idx, m;
    unsigned int value,ret;
	int i;
	void *fifo_mapbase,*virt_addr;
	struct spi_device *spi = &spidev;

	spi->chip_select = chip_select;
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

	ret = mips53003_spi_control(spi->chip_select, rxlen, len, 0);
	if(0 != ret)
	{
		printf("mips53003_spi_control error!!!\n");
		return SPI_ERR_PARAM;
	}

	ret = mips53003_spi_disable(spi->chip_select);
	if(0 != ret)
		printf("mips53003_spi_disable error!!!\n");

    return SPI_ERR_NONE;
}

/* Function : spidrv_read
 *  - Read operation through Keystone's SPI.
 * Return :
 * Note : 
 *     For SPI read operation, we start to do really read transaction on FIFO 
 *     when we call function spidrv_read. 
 */
int
spidrv_read(unsigned char *txbuf, int txlen,unsigned char *rxbuf, int rxlen, unsigned char chip_select)
{
    chipc_spi_softc_t *s = &spi_softc;
    int idx, m,i;
    unsigned int value,ret;
	void *fifo_mapbase,*virt_addr;
	struct spi_device *spi = &spidev;

	spi->chip_select = chip_select;
	ret = mips53003_spi_enable(spi->chip_select);
	if(0 != ret)
		printf("mips53003_spi_enable error!!!\n");
	if(mips53003_spi_buf_write(txbuf,txlen))
		printf("%s: Failed to do write buf operation!\n",__func__);
	
    //SPI_INIT_CHK;
    /* 
     * (Reading data length) <= 32 bytes
     * - The Maximum size of SPI's FIFO is 32 bytes(once time).
     */
    if (rxlen > SPI_FIFO_MAX_SIZE) {
        SPI_ERROR(("%s: Read failed : can not exceed 32 bytes!\n",__func__));
        return SPI_ERR_PARAM;
    }

    //SPI_INIT_CHK;
    /* 
     *  if s->states == SPI_STATES_WRITE, start to do Read transaction on FIFO
     *  ReadDataCnt = len, WriteDataCnt = s->buf_index, WriteCmdCnt = 0
     */
 
    if (s->states == SPI_STATES_WRITE || s->states == SPI_STATES_ENABLE) {
        if (s->states == SPI_STATES_WRITE) {
			if (mips53003_spi_write_for_read(s->buf, s->buf_index,rxlen)) {
                SPI_ERROR(("%s: Failed to write data to SPI FIFO at SPI_%d!\n", __func__, s->id));
                return SPI_ERR_PARAM;
            }
        }

        s->states = SPI_STATES_READ;
    } else {
        SPI_ERROR(("%s: Failed to do SPI read operation!\n",__func__));
        return SPI_ERR_PARAM;
    }
	for(i=0;i<10000;i++);	/*yu20160711*/
    /* Get spi status */
    value = mips53003_spi_status_get();
    /* Check FIFO is not empty for reading */
    if (value & CC_SPISTS_FIFOE) {
        SPI_ERROR(("%s: Read failed at SPI: FIFO is empty for reading!\n", __func__));
        return SPI_ERR_INTERNAL;
    }

    /* Read data from FIFO IO register until FIFO is empty(read 4 bytes each time) */

    value = 0;
    for (idx = 0 ; idx < rxlen ; idx++) {
        m = idx % 4;
        if (m == 0) {
			virt_addr = mapbase + (SPI_FIFO_IO & MAP_MASK);
			for(i=0;i<800;i++);
			value = *((unsigned int *) virt_addr);	
//		value = spi_fifo_read(0, 0);
        }
        *rxbuf++ = (unsigned char)(value >> ((3-m)*8));
    }
#if 1
	ret = mips53003_spi_disable(spi->chip_select);
	if(0 != ret)
		printf("mips53003_spi_disable error!!!\n");
#endif
    return SPI_ERR_NONE;
}

int spidrv_setup_init(void)
{
		struct spi_device *spi = &spidev;
		unsigned int flags = 0;
	    unsigned long bus_hz;
		int rv = SPI_ERR_NONE;
		chipc_spi_softc_t *s = &spi_softc;
		unsigned int value = 0;
		unsigned int  sys_freq, spi_freq;
	    int  n,ret; 

		spi->mode = SPI_MODE_3; 
		spi->bits_per_word = 8;
		spi->max_speed_hz = 8000000;
		spi->chip_select = 0;				//new add
	
		memset(s, 0, sizeof(s));
		s->id = spi->chip_select;
		s->buf_index = 0;
		s->states = SPI_STATES_DISABLE;
	/***************************************************************/
		/* Initialize the hardware */
		/* Select the SPI interface for SPI device id */
		if (mips53003_spi_interface_select(s->id, 1)) {
			SPI_ERROR(("%s: Failed on enabling SPI_%d!\n", __func__, s->id));
			return SPI_ERR_INTERNAL;
		}
		/* Set the SPI mode */
		value = CC_SPI_MODE_CPOL_1_CPHA_1;
		if (mips53003_spi_mode_set(SPI_MODE_CTRL_MODE, value)) {
			SPI_ERROR(("%s: Failed to set device mode at SPI_%d!\n", __func__, s->id));
			return SPI_ERR_PARAM;
		}
		value = CC_SPIMCTRL_BE;
	
		/* Set the SPI endian */
		if (mips53003_spi_mode_set(SPI_MODE_CTRL_ENDIAN, value)) {
			SPI_ERROR(("%s: Failed to set device endian at SPI_%d!\n", __func__, s->id));
			return SPI_ERR_PARAM;
		}
		if (spi->mode & SPI_LSB_FIRST) {
	//		flags |= CC_SPIMCTRL_LSB_MASK;
			flags |= 0x100;
		}
		/* Set the SPI RACK enable */
		value = (flags & CC_SPIMCTRL_ACKEN_MASK) ? CC_SPIMCTRL_ACKEN : 0;
		if (mips53003_spi_mode_set(SPI_MODE_CTRL_ACKEN, value)) {
			SPI_ERROR(("%s: Failed to set device RACK at SPI_%d!\n", __func__, s->id));
			return SPI_ERR_PARAM;
		}
		/* Set the SPI LSB first enable */
		value = (flags & CC_SPIMCTRL_LSB_MASK) ? CC_SPIMCTRL_LSB_FIRST: 0;
		if (mips53003_spi_mode_set(SPI_MODE_CTRL_LSBEN, value)) {
			SPI_ERROR(("%s: Failed to set device LSB first at SPI_%d!\n", __func__, s->id));
			return SPI_ERR_PARAM;
		}
		/* Set the SPI clodk */
	//	  value = spi->max_speed_hz;	 
		/* -------- set spi clock --------- */
		/* get Fsys */
		sys_freq = 150000000;
		SPI_MSG(("%s: sys_clock = %d, speed_hz = %d\n", __func__, sys_freq, spi->max_speed_hz));
	
		/* retrive the N value */
		/* spi clock counting formula  : spi_freq = sys_freq / (2^(N+1)) */
		for (n = 0; n < SPI_CCD_MAX; n++){
			spi_freq = sys_freq / (2 << n);
			if (spi_freq <= spi->max_speed_hz) {
				break;
			}
		}
	
		SPI_MSG(("%s: spi_freq = %d, clock divider parameters N = 0x%x\n", 
			__func__, spi_freq, n));
	
		/* Set the SPI clock (clock divider parameters n : [7:4] in SPI Moder Control register) */
		value = n << 4;
		if (mips53003_spi_mode_set(SPI_MODE_CTRL_CLOCK, value)) {
			SPI_ERROR(("%s: Failed to set device clock frequence at SPI_%d!\n", __func__, s->id));
			return SPI_ERR_PARAM;
		}
	
		ret = mips53003_spi_enable(spi->chip_select);
		if (ret < 0) {
			ret = -1;
			goto out_unmap_regs;
		}
		
		return 0;
	
	out_unmap_regs:

	return 0;
}
int spidrv_mmap_init(void)
{
    int fd;
	void *int_mapbase,*virt_addr;
	unsigned int  reg_val = 0, temp;
    /* mmap SERIAL_IO_SEL register */
	if((fd_mmap = open("/dev/mem", O_RDWR | O_SYNC)) == -1) 
		return ERROR;
	mapbase = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_mmap, SPI_REGISTER_BASE);

	return 0;
}
void spidrv_mmap_exit(void)
{
	munmap(mapbase,MAP_SIZE);
	close(fd_mmap);
}

#endif

#if defined(H20RN1000)||defined(H20RN2000) 
union semun
{
 int val; //信号量初始值                   
 struct semid_ds *buf;        
 unsigned short int *array;  
 struct seminfo *__buf;      
}; 
union semun arg;

int spidrv_read(unsigned char *txbuf, int txlen,unsigned char *rxbuf, int rxlen, unsigned char chip_select)
{
	unsigned int value;
	int ret, len;
	struct spi_device *spi = &spidev;
	unsigned char buf[MULTI_REG_LEN_MAX + 2] = {0};

	if(chip_select > 2)
	{
		printf("chip select %d error\n",chip_select);
		return -1;
	}
	if(rxlen > MULTI_REG_LEN_MAX)
	{
		printf("spidrv_read rxlen = %d, error\n", rxlen);
		return -1;
	}
	spi->chip_select = chip_select;

	if(CHIPSELECT_FPGA == spi->chip_select)
	{
		txlen = 4; /*181和2000时序不同，txlen统一用的3，这里需要4*/
	}

	len = rxlen + txlen;

	spi_setup(spi);

	if(ret = spi_transfer(spi, txbuf, buf, len) < 0)
	{       
		printf("spi transfer error\n");
		return -1; 
	}
	memcpy(rxbuf, buf+txlen, rxlen);

	return 0;
}

int spidrv_write(unsigned char *txbuf, int txlen, int rxlen, unsigned char chip_select)
{
	unsigned int value;
	int ret, len;
	struct spi_device *spi = &spidev;
	unsigned char buf[MULTI_REG_LEN_MAX + 2] = {0};

	if(chip_select > 2)
	{
		printf("chip select %d error\n",chip_select);
		return -1;
	}
	if(txlen > MULTI_REG_LEN_MAX)
	{
		printf("spidrv_write txlen = %d, error\n", txlen);
		return -1;
	}

	spi->chip_select = chip_select;
	if(CHIPSELECT_FPGA == spi->chip_select)
	{
		txlen = txlen + 1; /*181和2000时序不同，txlen统一用的3，这里需要4*/
	}

	len = txlen;

	spi_setup(spi);

	if(ret = spi_transfer(spi, txbuf, buf, len) < 0)
	{       
		printf("spi transfer error\n");
		return -1; 
	}

	return 0;
}

static int spidrv_setup_init()
{
        int i = 0;

        semop(spidrv_semid, &spidrv_sembufLock, 1);

        spi_dev_init(&spidev);

        for(i = 0; i <4; i++)
        {
                spidev.max_speed_hz = 6500000;//the real rate 6.25M
                spidev.chip_select = i;
                spidev.mode = SPI_MODE_3;
                spidev.bits_per_word = 8;

                spi_setup(&spidev);
        }

        semop(spidrv_semid, &spidrv_sembufUnlock, 1);

        return 0;
}

#define SPI_REGISTER_BASE 0xffe07000
int fd_mmap;
static int spidrv_mmap_init()
{

        if((fd_mmap = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
        {
                return -1;
        }
        spidev.spi_reg = (struct spi_reg_t *)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_mmap, SPI_REGISTER_BASE);
        if(NULL == spidev.spi_reg)
                return -1;
        return 0;
}
static void spidrv_mmap_exit()
{
        munmap((void *)spidev.spi_reg,MAP_SIZE);
        close(fd_mmap);
}

#endif
#if H20RN181_ARM

int spidrv_read(unsigned char *txbuf, int txlen,unsigned char *rxbuf, int rxlen, unsigned char chip_select)
{
//printf("spidrv_read txlen=%d\n",txlen);
	unsigned int value;
	int ret, len;
	struct spi_device *spi = &spidev;
	unsigned char buf[MULTI_REG_LEN_MAX + 2] = {0};

	if(chip_select > 2)
	{
		printf("chip select %d error\n",chip_select);
		return -1;
	}
	if(rxlen > MULTI_REG_LEN_MAX)
	{
		printf("spidrv_read rxlen = %d, error\n", rxlen);
		return -1;
	}
	spi->chip_select = chip_select;
#if 0
	if(CHIPSELECT_FPGA == spi->chip_select)
	{
		txlen = 4; /*181和2000时序不同，txlen统一用的3，这里需要4*/
	}
#endif
	//printf("txbuf[0]=0x%02x,txbuf[1]=0x%02x,txbuf[2]=0x%02x,txlen=0x%02x,rxlen=0x%02x\n",txbuf[0],txbuf[1],txbuf[2],txlen,rxlen);
	gpio_direction_output(spi->chip_select, 0);
	arm5626x_mspi_writeread8(txbuf,txlen,buf,rxlen); 
	gpio_direction_input(spi->chip_select);

	memcpy(rxbuf, buf, rxlen);

	return 0;
}

int spidrv_write(unsigned char *txbuf, int txlen, int rxlen, unsigned char chip_select)
{
	unsigned int value;
	int ret, len;
	struct spi_device *spi = &spidev;
	unsigned char buf[MULTI_REG_LEN_MAX + 2] = {0};

	if(chip_select > 2)
	{
		printf("chip select %d error\n",chip_select);
		return -1;
	}
	if(txlen > MULTI_REG_LEN_MAX)
	{
		printf("spidrv_write txlen = %d, error\n", txlen);
		return -1;
	}

	spi->chip_select = chip_select;
#if 0
	if(CHIPSELECT_FPGA == spi->chip_select)
	{
		txlen = txlen + 1; /*181和2000时序不同，txlen统一用的3，这里需要4*/
	}
#endif

	gpio_direction_output(spi->chip_select, 0);
	arm5626x_mspi_write8(txbuf, txlen);
	gpio_direction_input(spi->chip_select);

	return 0;
}

static int spidrv_setup_init()
{
        int ret  = 0;

        semop(spidrv_semid, &spidrv_sembufLock, 1);
	ret = arm5626x_mspi_init(&spidev);

        semop(spidrv_semid, &spidrv_sembufUnlock, 1);

        return ret;
}

#define SPI_REGISTER_BASE 0x03201000
int fd_mmap;
static int spidrv_mmap_init()
{

        if((fd_mmap = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
        {
                return -1;
        }
        spidev.spi_reg = (struct spi_reg_t *)(0x500+mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_mmap, SPI_REGISTER_BASE));
        if(NULL == spidev.spi_reg)
                return -1;
        return 0;
}
static void spidrv_mmap_exit()
{
        munmap(((void *)spidev.spi_reg-0x500),MAP_SIZE);
        close(fd_mmap);
}


#endif
union semun
{
    int val; //淇″彿閲忓垵濮嬪€?
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfo *__buf;
};
union semun arg;
int spidrv_semlock_init(void)  
{  
	int ret = 0;

	spidrv_semid = semget(0x0812, 1, IPC_CREAT|IPC_EXCL|0666);		
	if (spidrv_semid == -1){ 
		spidrv_initialized = 1;
		spidrv_semid = semget(0x0812, 1, IPC_CREAT|0666);
		if (spidrv_semid == -1){
				printf("spidrv_semid=%d\n",spidrv_semid);
				return -1;
		}
	} 
	else
	{
	
		spidrv_initialized = 0;
		arg.val = 1;
		ret = semctl(spidrv_semid, 0, SETVAL, arg); 
	}

	spidrv_sembufLock.sem_num = 0;  
	spidrv_sembufLock.sem_op = -1;  
	spidrv_sembufLock.sem_flg = SEM_UNDO;  

	spidrv_sembufUnlock.sem_num = 0;  
	spidrv_sembufUnlock.sem_op = 1;  
	spidrv_sembufUnlock.sem_flg = SEM_UNDO; 
	return ret;
}


int spidrv_init(void)
{
	int ret;

	memset(&spidev, 0, sizeof(spidev));

	if(spidrv_semlock_init())
	{
		printf("spidrv semlock init err\n");
		return -1;
	}
printf("semlock init ok\n");
	if(spidrv_mmap_init())
	{
		printf("spidrv mmap init err\n");
		return -1;
	}
printf("mmap init ok\n");
//	if(0 == spidrv_initialized)
//	{
		if(spidrv_setup_init())
		{
			printf("spidrv spi setup init error\n");
			return -1;
		}
		printf("Initialize spidrv successfully\n");
//	}

	return 0;
}
int spidrv_exit(void)
{
	spidrv_mmap_exit();
	return 0;	
}




