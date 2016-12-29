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

#include <common.h>
#include <post.h>

#if CONFIG_POST & CONFIG_SYS_POST_DS1WM

#include "asm/arch/socregs.h"
#include "../../drivers/d1w/ds1wm.h"

#define DS2438_SENSOR       25
#define DS2438_PAGE_SIZE	8
#define DS2438_PAGE_NUM		8

#define DS2438_CONVERT_TEMP	0x44
#define DS2438_CONVERT_VOLT	0xB4
#define DS2438_WRITE_SCRATCHPAD	0x4E
#define DS2438_COPY_SCRATCHPAD	0x48
#define DS2438_RECALL_MEMORY	0xB8

enum DS2438_PAGE {
	PAGE0_CONTROL = 0,
	PAGE1_ETM,
	PAGE2_STAMP,
	PAGE3_RESV3,
	PAGE4_RESV4,
	PAGE5_RESV5,
	PAGE6_RESV6,
	PAGE7_CCA,
};

enum DS2438_REG {
	/* PAGE 0 */
	PAGE0_STAT_CTRL = 0,
	PAGE0_TEMP_LSB = 1,
	PAGE0_TEMP_MSB = 2,
	PAGE0_VOLTAGE_LSB = 3,
	PAGE0_VOLTAGE_MSB = 4,
	PAGE0_CURRENT_LSB = 5,
	PAGE0_CURRENT_MSB = 6,
	PAGE0_THRESHOLD = 7,

	/* PAGE 1 */
	PAGE1_ETM_BYTE0 = 0,
	PAGE1_ETM_BYTE1 = 1,
	PAGE1_ETM_BYTE2 = 2,
	PAGE1_ETM_BYTE3 = 3,
	PAGE1_ICA = 4,
	PAGE1_OFFSET_LSB = 5,
	PAGE1_OFFSET_MSB = 6,

	/* PAGE 2 */
	PAGE2_DISCONNECT_BYTE0 = 0,
	PAGE2_DISCONNECT_BYTE1 = 1,
	PAGE2_DISCONNECT_BYTE2 = 2,
	PAGE2_DISCONNECT_BYTE3 = 3,
	PAGE2_END_CHARGE_BYTE0 = 4,
	PAGE2_END_CHARGE_BYTE1 = 5,
	PAGE2_END_CHARGE_BYTE2 = 6,
	PAGE2_END_CHARGE_BYTE3 = 7,

	/* PAGE 3 */
	/* PAGE 4 */
	/* PAGE 5 */
	/* PAGE 6 */
	/* PAGE 7 */
	PAGE7_CCA_LSB = 4,
	PAGE7_CCA_MSB = 5,
	PAGE7_DCA_LSB = 6,
	PAGE7_DCA_MSB = 7,
};

#define DS2438_CTRL_IAD		(1 << 0)
#define DS2438_CTRL_CA		(1 << 1)
#define DS2438_CTRL_EE		(1 << 2)
#define DS2438_CTRL_AD		(1 << 3)
#define DS2438_STAT_TB		(1 << 4)
#define DS2438_STAT_NVB		(1 << 5)
#define DS2438_STAT_ADB		(1 << 6)

#define W1_SEARCH		0xF0
#define W1_ALARM_SEARCH		0xEC
#define W1_CONVERT_TEMP		0x44
#define W1_SKIP_ROM		0xCC
#define W1_READ_SCRATCHPAD	0xBE
#define W1_READ_ROM		0x33
#define W1_READ_PSUPPLY		0xB4
#define W1_MATCH_ROM		0x55
#define W1_RESUME_CMD		0xA5


static inline unsigned short ds2438_readw(unsigned char *raw)
{
	return ((*(raw + 1)) << 8) | (*raw);
}

static inline void ds2438_writew(unsigned char *raw, unsigned short data)
{
	*raw++ = data & 0xFF;
	*raw = (data >> 8) & 0xFF;
}

static int ds2438_read_page(unsigned char page, unsigned char *buf)
{
    unsigned char buffer[8] = {0};
	if (!ds1wm_reset(d1w_master)) {
        buffer[0] = W1_READ_SCRATCHPAD;
		ds1wm_write_bytes(d1w_master, buffer, 1);
        buffer[0] = page;
		ds1wm_write_bytes(d1w_master, buffer, 1);
        ds1wm_read_bytes(d1w_master,buffer,DS2438_PAGE_SIZE);
	}
	return 0;
}

static int ds2438_write_page(unsigned char page, unsigned char *buf)
{
    unsigned char buffer[8] = {0};
	if (!ds1wm_reset(d1w_master)) {
        buffer[0] = W1_READ_SCRATCHPAD;
		ds1wm_write_bytes(d1w_master, buffer, 1);
        buffer[0] = page;
		ds1wm_write_bytes(d1w_master, buffer, 1);
        ds1wm_read_bytes(d1w_master,buf,DS2438_PAGE_SIZE);
	}
	return 0;
}


static int ds2438_command(unsigned char command, unsigned char data)
{
    unsigned char buffer[8] = {0};
	if (!ds1wm_reset(d1w_master)) {
        buffer[0] = command;
		ds1wm_write_bytes(d1w_master, buffer, 1);
        if(command == DS2438_COPY_SCRATCHPAD || command == DS2438_RECALL_MEMORY)
        {
            buffer[0] = data;
    		ds1wm_write_bytes(d1w_master, buffer, 1);
        }
	}
	return 0;
}

static int ds2438_drain_sram(unsigned char page)
{
	return ds2438_command(DS2438_COPY_SCRATCHPAD, page);
}

static int ds2438_load_sram(unsigned char page)
{
	return ds2438_command(DS2438_RECALL_MEMORY, page);
}

static void ds2438_init(void)
{
    unsigned char buffer[DS2438_PAGE_SIZE] = {0};
    ds2438_load_sram(PAGE0_CONTROL);
    ds2438_read_page(PAGE0_CONTROL, buffer);
    buffer[PAGE0_STAT_CTRL] |= DS2438_CTRL_EE;
    buffer[PAGE0_STAT_CTRL] |= DS2438_CTRL_AD;
    buffer[PAGE0_THRESHOLD] = 0x40;
    ds2438_write_page(PAGE0_CONTROL, buffer);
    ds2438_drain_sram(PAGE0_CONTROL);
}

static void ds2438_calibrate_init(void)
{
	int current_raw;
    unsigned char buffer[DS2438_PAGE_SIZE] = {0};
	/* disable ICA */
	ds2438_load_sram( PAGE0_CONTROL);
	ds2438_read_page( PAGE0_CONTROL, buffer);
	buffer[PAGE0_STAT_CTRL] &= ~DS2438_CTRL_IAD;
	ds2438_write_page( PAGE0_CONTROL, buffer);
	ds2438_drain_sram( PAGE0_CONTROL);

	/* Zero offset */
	ds2438_load_sram( PAGE1_ETM);
	ds2438_read_page( PAGE1_ETM, buffer);
	ds2438_writew(buffer + PAGE1_OFFSET_LSB, 0);
	ds2438_drain_sram( PAGE1_ETM_BYTE0);

	/* enable ICA & read current */
	ds2438_load_sram( PAGE0_CONTROL);
	ds2438_read_page( PAGE0_CONTROL, buffer);
	buffer[PAGE0_STAT_CTRL] |= DS2438_CTRL_IAD;
	ds2438_write_page( PAGE0_CONTROL, buffer);
	ds2438_drain_sram( PAGE0_CONTROL);
	/*wait current convert about 36HZ */
	mdelay(30);
	/* disable ICA */
	ds2438_load_sram( PAGE0_CONTROL);
	ds2438_read_page( PAGE0_CONTROL, buffer);
	buffer[PAGE0_STAT_CTRL] &= ~DS2438_CTRL_IAD;
	ds2438_write_page( PAGE0_CONTROL, buffer);
	ds2438_drain_sram( PAGE0_CONTROL);
	/* read current value */
	current_raw = ds2438_readw(buffer + PAGE0_CURRENT_LSB);
	/* write offset by current value */
	ds2438_load_sram( PAGE1_ETM);
	ds2438_read_page( PAGE1_ETM, buffer);
	ds2438_writew(buffer + PAGE1_OFFSET_LSB, current_raw << 8);
	ds2438_write_page( PAGE1_ETM, buffer);
	ds2438_drain_sram( PAGE1_ETM);

	/*enable ICA again */
	ds2438_load_sram( PAGE0_CONTROL);
	ds2438_read_page( PAGE0_CONTROL, buffer);
	buffer[PAGE0_STAT_CTRL] |= DS2438_CTRL_IAD;
	ds2438_write_page( PAGE0_CONTROL, buffer);
	ds2438_drain_sram( PAGE0_CONTROL);
}

static int ds2438_get_ica(u32 senser, u8 raw)
{
	int charge_uAh;
	/* (x * 1000 * 1000)uA / (2048 * (Rsens / 1000)) */
	charge_uAh = (raw * 125 * 125 * 125) >> 4;
	charge_uAh /= (senser << 4);
	return charge_uAh;
}

static int ds2438_battery_update_page1(void)
{
	int ica_raw;
    unsigned char buffer[DS2438_PAGE_SIZE] = {0};
	ds2438_load_sram(PAGE1_ETM);
	ds2438_read_page(PAGE1_ETM, buffer);
	ica_raw = buffer[PAGE1_ICA];

	return ds2438_get_ica(DS2438_SENSOR, ica_raw);;
}

static inline int ds2438_get_temp(u16 raw)
{
	int degree, s;

	s = !!(raw & 0x8000);

	if (s)
		raw = ((~raw & 0x7FFF) + 1);
	degree = ((raw >> 8) * 10) + (((raw & 0xFF) * 5) + 63) / 128;

	return s ? -degree : degree;
}

static inline int ds2438_get_current(u32 senser, u16 raw)
{
	int s, current_uA;
	s = !!(raw & 0xFC00);
	/* (x * 1000 * 1000)uA / (4096 * (Rsens / 1000)) */
	raw &= 0x3FF;
	current_uA = raw * 125 * 125 * 125;
	current_uA /= (senser << 3);
	return s ? -current_uA : current_uA;
}

static int ds2438_battery_read_status(void)
{
	u8 status;
	int temp_raw, voltage_raw, current_raw;
    unsigned char buffer[DS2438_PAGE_SIZE] = {0};
    int temp_C = 0,voltage_uV = 0,current_uA = 0,accum_current_uAh = 0;


    ds2438_load_sram( PAGE0_CONTROL);
    ds2438_read_page( PAGE0_CONTROL, buffer);
	status = buffer[PAGE0_STAT_CTRL];
	temp_raw = ds2438_readw(buffer + PAGE0_TEMP_LSB);
	voltage_raw = ds2438_readw(buffer + PAGE0_VOLTAGE_LSB);
	current_raw = ds2438_readw(buffer + PAGE0_CURRENT_LSB);
	temp_C = ds2438_get_temp(temp_raw);
	voltage_uV = voltage_raw * 10000;
	current_uA = ds2438_get_current(DS2438_SENSOR, current_raw);

	accum_current_uAh = ds2438_battery_update_page1();

	if (!(status & DS2438_STAT_TB))
    {
        ds2438_command( DS2438_CONVERT_TEMP, 0);
    }
	if (!(status & DS2438_STAT_ADB))
    {
        ds2438_command( DS2438_CONVERT_VOLT, 0);
    }

    printf("temp_c : %d\nvoltage_uV : %d\ncurrent_uA : %d\naccum_current_uAh : %d\n",
        temp_C, voltage_uV, current_uA, accum_current_uAh);
	return 0;
}

int DS1WM_post_test (int flags)
{
    printf("DS1WM testing ...\n");
    ds1wm_init();
    printf("Initializing DS2438 ...\n");
    ds2438_init();
    printf("DS2438 calibrate inittializing ...\n");
    ds2438_calibrate_init();
    printf("Reading battery status ...\n");
    ds2438_battery_read_status();

    return 0;
}
#endif

