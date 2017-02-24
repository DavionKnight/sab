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

extern int spidrv_semid;
extern struct sembuf spidrv_sembufLock, spidrv_sembufUnlock;

#ifdef H20RN181
int spi_flag=0xff;

int dpll_write(unsigned short addr, unsigned char *data, size_t count,unsigned char flag)
{
	unsigned short address = 0;
	unsigned char buf[MULTI_REG_LEN_MAX + 2] = {0};
	unsigned char chip_select[2]={0};
	int ret=0;

	if (!data || count > MULTI_REG_LEN_MAX)
		return -1;

	chip_select[1] = flag;
	spi_flag = flag;
	ret = fpga_write(0x12, chip_select,2,0);  
	ret = fpga_read(0x12, chip_select,2,0);

	semop(spidrv_semid, &spidrv_sembufLock, 1);

	if((chip_select[1] == DS31400_CHIP) && (spi_flag == 0))
	{
		address = (addr << 1) & 0x7ffe;

		buf[0] = (unsigned char)((address >> 8) & 0xff);
		buf[1] = (unsigned char)((address) & 0xff);

		/* MSB must be '0' to write */
		buf[0] &= ~0x80;
		/* LSB must be '1' to burst write */
		if (count > 1)
			buf[1] |= 0x01;

		memcpy(&buf[2], data, count);
	
		ret = spidrv_write(buf,count+2,0, CHIPSELECT_DPLL);
		if(0 != ret)
			printf("spidrv_write error!!!\n");	
	
		spi_flag = 0xff;
		semop(spidrv_semid, &spidrv_sembufUnlock, 1);

		return ret;
	}
	else if((chip_select[1] == E1_CHIP) && (spi_flag == 1))
	{
	        if(1 == count)
	        {
	            buf[0] = SPI_FPGA_WR_SINGLE;
	        }
	        else
	        {
	            buf[0] = SPI_FPGA_WR_BURST;
	        }	
		address = addr;
		buf[1] = (unsigned char)((address >> 8) & 0xff);
		buf[2] = (unsigned char)((address) & 0xff);


		memcpy(&buf[3], data, count);
	
		ret = spidrv_write(buf,count+3,0,CHIPSELECT_DPLL);
		if(0 != ret)
			printf("spidrv_write error!!!\n");	
	
		spi_flag = 0xff;
		semop(spidrv_semid, &spidrv_sembufUnlock, 1);

		return ret;
	}
	else
	{
		printf("......write dpll or E1 error......select=%d  spi_flag: %d add:%x  data=%2d%2d\n",chip_select,spi_flag,addr,data[0],data[1]);
		spi_flag = 0xff;
		semop(spidrv_semid, &spidrv_sembufUnlock, 1);
		return -1;
	}
}

int dpll_read(unsigned short addr, unsigned char *data, size_t count,unsigned char flag)
{
	int ret;
	struct spi_transfer xfer;

	unsigned short address = 0;
	unsigned char buf[MULTI_REG_LEN_MAX + 2] = {0};
	unsigned char rx_buf[MULTI_REG_LEN_MAX + 2] = {0};
	struct spi_device *spi = &spidev;
	
	unsigned char chip_select[2]={0};
	
	if (!data || count > MULTI_REG_LEN_MAX)
		return -EINVAL;

	spi->chip_select = 0;
	chip_select[1] = flag;
	spi_flag = flag;
	ret = fpga_write(0x12, chip_select,2,0);
	ret = fpga_read(0x12, chip_select,2,0);

	semop(spidrv_semid, &spidrv_sembufLock, 1);

	spi->chip_select = 1;
	if((chip_select[1] == DS31400_CHIP) && (spi_flag == 0))
	{
	
		address = (addr << 1) & 0x7ffe;

		buf[0] = (unsigned char)((address >> 8) & 0xff);
		buf[1] = (unsigned char)((address) & 0xff);

		/* MSB must be '1' to read */ 
		buf[0] |= 0x80;
		/* LSB must be '1' to burst read */
		if (count > 1)
			buf[1] |= 0x01;

		memcpy(&buf[2], data, count);

		ret = spidrv_read(buf,2,rx_buf,count, CHIPSELECT_DPLL);
		if(0 != ret)
			printf("spidrv_read error!!!\n");
		
	}
	else if((chip_select[1] == E1_CHIP) && (spi_flag == 1))
	{
		address = addr;
	        if(1 == count)
	        {
	            buf[0] = SPI_FPGA_RD_SINGLE;        
	        }
	        else 
	        {
	            buf[0] = SPI_FPGA_RD_BURST;
	        }
		buf[1] = (unsigned char)((address >> 8) & 0xff);
		buf[2] = (unsigned char)((address) & 0xff);


		memcpy(&buf[3], data, count);
		
		ret = spidrv_read(buf,3,rx_buf,count, CHIPSELECT_DPLL);
		if(0 != ret)
			printf("spidrv_read error!!!\n");
		
	}
	else
	{
		printf("......read dpll or E1 error......chip=%d spi_flag: %d  addr =%x \n", chip_select,spi_flag,addr);
		spi_flag = 0xff;
		semop(spidrv_semid, &spidrv_sembufUnlock, 1);

		return -1;
	}

	memcpy(data, &rx_buf[0], count);
	spi_flag = 0xff;
	semop(spidrv_semid, &spidrv_sembufUnlock, 1);

	return ret;
}
#endif

#if defined (H20RN2000)||defined(H20RN1000)

int dpll_read(unsigned short addr, unsigned char *data, size_t count)
{
	int ret = 0;
	unsigned int len = 0;
	unsigned char rxbuf[MULTI_REG_LEN_MAX + 2] = {0};
	unsigned char txbuf[MULTI_REG_LEN_MAX + 2] = {0};
	unsigned short address = 0;

	if (!data || count > MULTI_REG_LEN_MAX)
		return -1;

	semop(spidrv_semid, &spidrv_sembufLock, 1);

	address = (addr << 1) & 0x7ffe;

	txbuf[0] = (unsigned char)((address >> 8) & 0xff);
	txbuf[1] = (unsigned char)((address) & 0xff);

	/* MSB must be '1' to read */
	txbuf[0] |= 0x80;
	/* LSB must be '1' to burst read */
	if (count > 1)
		txbuf[1] |= 0x01;

	ret = spidrv_read(txbuf, 2, data, count, CHIPSELECT_DPLL);
	if(0 != ret)
		printf("dpll read error!!!\n");

	semop(spidrv_semid, &spidrv_sembufUnlock, 1);

	return ret;
}
int dpll_write(unsigned short addr, unsigned char *data, size_t count)
{
	int ret = 0;
	unsigned int len = 0;
	unsigned short address = 0;
	unsigned char rxbuf[MULTI_REG_LEN_MAX + 2] = {0};
	unsigned char txbuf[MULTI_REG_LEN_MAX + 2] = {0};

	semop(spidrv_semid, &spidrv_sembufLock, 1);

	address = (addr << 1) & 0x7ffe;

	txbuf[0] = (unsigned char)((address >> 8) & 0xff);
	txbuf[1] = (unsigned char)((address) & 0xff);

	/* MSB must be '1' to read */
	txbuf[0] &= ~0x80;
	/* LSB must be '1' to burst read */
	if (count > 1)
		txbuf[1] |= 0x01;

	/*fill txbuf len equal to rx count*/
	memcpy(&txbuf[2], data, count);

	if(spidrv_write(txbuf, count+2, 0, CHIPSELECT_DPLL) < 0)
	{
		printf("dpll spi write failed.\n");
		ret = -1;
	}

	semop(spidrv_semid, &spidrv_sembufUnlock, 1);

	return ret;
}

#endif
#if defined H20RN181_ARM

int dpll_read(unsigned short addr, unsigned char *data, size_t count)
{
	int ret = 0;
	unsigned int len = 0;
	unsigned char rxbuf[MULTI_REG_LEN_MAX + 2] = {0};
	unsigned char txbuf[MULTI_REG_LEN_MAX + 2] = {0};
	unsigned short address = 0;

	if (!data || count > MULTI_REG_LEN_MAX)
		return -1;

	semop(spidrv_semid, &spidrv_sembufLock, 1);

	address = (addr << 1) & 0x7ffe;

	txbuf[0] = (unsigned char)((address >> 8) & 0xff);
	txbuf[1] = (unsigned char)((address) & 0xff);

	/* MSB must be '1' to read */
	txbuf[0] |= 0x80;
	/* LSB must be '1' to burst read */
	if (count > 1)
		txbuf[1] |= 0x01;

	ret = spidrv_read(txbuf, 2, data, count, CHIPSELECT_DPLL);
	if(0 != ret)
		printf("dpll read error!!!\n");

	semop(spidrv_semid, &spidrv_sembufUnlock, 1);

	return ret;
}
int dpll_write(unsigned short addr, unsigned char *data, size_t count)
{
	int ret = 0;
	unsigned int len = 0;
	unsigned short address = 0;
	unsigned char rxbuf[MULTI_REG_LEN_MAX + 2] = {0};
	unsigned char txbuf[MULTI_REG_LEN_MAX + 2] = {0};

	semop(spidrv_semid, &spidrv_sembufLock, 1);

	address = (addr << 1) & 0x7ffe;

	txbuf[0] = (unsigned char)((address >> 8) & 0xff);
	txbuf[1] = (unsigned char)((address) & 0xff);

	/* MSB must be '1' to read */
	txbuf[0] &= ~0x80;
	/* LSB must be '1' to burst read */
	if (count > 1)
		txbuf[1] |= 0x01;

	/*fill txbuf len equal to rx count*/
	memcpy(&txbuf[2], data, count);

	if(spidrv_write(txbuf, count+2, 0, CHIPSELECT_DPLL) < 0)
	{
		printf("dpll spi write failed.\n");
		ret = -1;
	}

	semop(spidrv_semid, &spidrv_sembufUnlock, 1);

	return ret;
}

#endif


