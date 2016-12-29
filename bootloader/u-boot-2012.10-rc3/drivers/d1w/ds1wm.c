/*
 *  Copyright (C) 1999-2013, Broadcom Corporation
 *  
 *       Unless you and Broadcom execute a separate written software license
 *  agreement governing use of this software, this software is licensed to you
 *  under the terms of the GNU General Public License version 2 (the "GPL"),
 *  available at http://www.broadcom.com/licenses/GPLv2.php, with the
 *  following added to such license:
 *  
 *       As a special exception, the copyright holders of this software give you
 *  permission to link this software with independent modules, and to copy and
 *  distribute the resulting executable under terms of your choice, provided that
 *  you also meet, for each linked independent module, the terms and conditions of
 *  the license of that module.  An independent module is a module which is not
 *  derived from this software.  The special exception does not apply to any
 *  modifications of the software.
 *  
 *       Notwithstanding the above, under no circumstances may you combine this
 *  software in any way with any other Broadcom software provided under a license
 *  other than the GPL, without Broadcom's express prior written consent.
 */
#include <common.h>
#include <config.h>
#include <asm/io.h>

#include "ds1wm.h"

static struct {
	unsigned long freq;
	unsigned long divisor;
} freq[] = {
	{   1000000, 0x80 },
	{   2000000, 0x84 },
	{   3000000, 0x81 },
	{   4000000, 0x88 },
	{   5000000, 0x82 },
	{   6000000, 0x85 },
	{   7000000, 0x83 },
	{   8000000, 0x8c },
	{  10000000, 0x86 },
	{  12000000, 0x89 },
	{  14000000, 0x87 },
	{  16000000, 0x90 },
	{  20000000, 0x8a },
	{  24000000, 0x8d },
	{  28000000, 0x8b },
	{  32000000, 0x94 },
	{  40000000, 0x8e },
	{  48000000, 0x91 },
	{  56000000, 0x8f },
	{  64000000, 0x98 },
	{  80000000, 0x92 },
	{  96000000, 0x95 },
	{ 112000000, 0x93 },
	{ 128000000, 0x9c },
};

ds1wm_info_t *d1w_master = NULL;

static int ds1wm_find_divisor(int gclk)
{
	int i;
    int array_size = sizeof(freq)/sizeof(freq[0]);

	for (i = array_size-1; i >= 0; --i)
		if (gclk >= freq[i].freq)
			return freq[i].divisor;

	return 0;
}

static inline void ds1wm_write_register
    (
    ds1wm_info_t *ds1wm, 
    unsigned int reg_addr,
	unsigned char val
	)
{
    writel(val, ds1wm->base_addr + D1W_DIN_OFFSET);
    writel(reg_addr, ds1wm->base_addr + D1W_ADDR_OFFSET);
    writel(0x3, ds1wm->base_addr + D1W_CTRL_OFFSET);
}

static inline unsigned char ds1wm_read_register
    (
    ds1wm_info_t *ds1wm, 
    unsigned int reg_addr
    )
{
    writel(reg_addr, ds1wm->base_addr+ D1W_ADDR_OFFSET);
    writel(0x2, ds1wm->base_addr + D1W_CTRL_OFFSET);
    return readb(ds1wm->base_addr + D1W_DOUT_OFFSET);
}

int ds1wm_reset(ds1wm_info_t *ds1wm)
{
	unsigned char int_status = 0;
    unsigned int check_times = 10000;

	ds1wm_write_register(ds1wm, DS1WM_CMD, DS1WM_CMD_1W_RESET);
    udelay(1250); /*Delay 1250 us*/
    while(check_times--)
    {
        int_status = ds1wm_read_register(ds1wm, DS1WM_INT);
        ds1wm->slave_present = (int_status & DS1WM_INT_PDR) ? 0 : 1;
        if(int_status&DS1WM_INT_PD)
            break;
    }

    if(!check_times)    /*Reset failed*/
        return -1;
    
	return 0;
}

void ds1wm_enable(ds1wm_info_t *ds1wm)
{
    int divisor = 0;
    unsigned int reg;

    reg = readl(D1W_CLK_GATE_CTRL);
    reg &= ~D1W_CLK_GATE_EN;
    writel(D1W_CLK_GATE_CTRL, reg);

    reg = readl(CRMU_IOMUX_CTRL4);
    reg &= ~(7<<16);
    writel(CRMU_IOMUX_CTRL4, reg);
    
    divisor = ds1wm_find_divisor(D1W_CLOCK_RATE);
    ds1wm_write_register(ds1wm, DS1WM_CLKDIV, divisor);
	udelay(1000);
	ds1wm_reset(ds1wm);
}

void ds1wm_disable(ds1wm_info_t *ds1wm)
{
    unsigned int reg;

    reg = readl(D1W_CLK_GATE_CTRL);
    reg |= D1W_CLK_GATE_EN;
    writel(D1W_CLK_GATE_CTRL, reg);

    reg = readl(CRMU_IOMUX_CTRL4);
    reg |= 1<<16;
    writel(CRMU_IOMUX_CTRL4, reg);
}


static int ds1wm_write(ds1wm_info_t *ds1wm, unsigned char data)
{
    unsigned char intr_status;
	ds1wm_write_register(ds1wm, DS1WM_DATA, data);
    while(1)
    {
        intr_status = ds1wm_read_register(ds1wm, DS1WM_INT);
        if(intr_status & DS1WM_INT_TSRE)
            break;
    }

	return 0;
}

static unsigned char ds1wm_read(ds1wm_info_t *ds1wm, unsigned char write_data)
{
    unsigned char intr_status;
    unsigned char val = 0xFF;
    
	ds1wm_read_register(ds1wm, DS1WM_DATA);
	ds1wm_write_register(ds1wm, DS1WM_DATA, write_data);
    while(1)
    {
        intr_status = ds1wm_read_register(ds1wm, DS1WM_INT);
        if(intr_status & DS1WM_INT_RBF)
        {
            val = ds1wm_read_register(ds1wm, DS1WM_DATA);
        }
    }
	return val;
}

int ds1wm_write_bytes(ds1wm_info_t *ds1wm, unsigned char *data, int data_len)
{
    int i;

    for(i = 0; i < data_len; i++)
    {
        ds1wm_write(ds1wm, data[i]);
    }
    
    return 0;
}

int ds1wm_read_bytes(ds1wm_info_t *ds1wm, unsigned char *data, int data_len)
{
    int i;

    for(i = 0; i < data_len; i++)
    {
        data[i] = ds1wm_read(ds1wm, 0xFF);
    }

    return 0;
}
extern void *malloc(unsigned int num_bytes);
int ds1wm_init(void)
{
	ds1wm_info_t *ds1wm = NULL;
    unsigned char buffer[8] = {0};

	d1w_master = (ds1wm_info_t *)malloc(sizeof(ds1wm_info_t));
    ds1wm = d1w_master;
	if (ds1wm == NULL)
		return -1;

    memset(ds1wm, 0, sizeof(ds1wm_info_t));
    /*Set register base address*/
    ds1wm->base_addr    = D1W_BASE_ADDRESS;

    /*Disable all interrupt*/
    ds1wm_write_register(ds1wm, DS1WM_INT_EN, 0);

    /*Set clock and reset d1w bus*/
	ds1wm_enable(ds1wm);

    ds1wm_reset(ds1wm);
    /*Assume that only one slave device is present, so we don't need to process ROM searching*/
    buffer[0] = SLAVE_CMD_SKIPROM;
    ds1wm_write_bytes(ds1wm, buffer, 1);

    ds1wm_reset(ds1wm);
    /*Read ROM code*/
    buffer[0] = SLAVE_CMD_READROM;
    ds1wm_write_bytes(ds1wm, buffer, 1);
    ds1wm_read_bytes(ds1wm, buffer, 8);
    printf("ROM code: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n", 
                            buffer[0],buffer[1],buffer[2],buffer[3],
                            buffer[4],buffer[5],buffer[6],buffer[7]);
    
	return 0;
}


