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
#ifndef __DS1WM_H__
#define __DS1WM_H__

#define DS1WM_CMD	    0x00	/* R/W 4 bits command */
#define DS1WM_DATA	    0x01	/* R/W 8 bits, transmit/receive buffer */
#define DS1WM_INT	    0x02	/* R/W interrupt status */
#define DS1WM_INT_EN	0x03	/* R/W interrupt enable */
#define DS1WM_CLKDIV	0x04	/* R/W 5 bits of divisor and pre-scale */
#define DS1WM_CNTRL	    0x05	/* R/W master control register (not used yet) */

#define DS1WM_CMD_1W_RESET  (1 << 0)	/* force reset on 1-wire bus */
#define DS1WM_CMD_SRA	    (1 << 1)	/* enable Search ROM accelerator mode */
#define DS1WM_CMD_DQ_OUTPUT (1 << 2)	/* write only - forces bus low */
#define DS1WM_CMD_DQ_INPUT  (1 << 3)	/* read only - reflects state of bus */
#define DS1WM_CMD_RST	    (1 << 5)	/* software reset */
#define DS1WM_CMD_OD	    (1 << 7)	/* overdrive */

#define DS1WM_INT_PD	    (1 << 0)	/* presence detect */
#define DS1WM_INT_PDR	    (1 << 1)	/* presence detect result */
#define DS1WM_INT_TBE	    (1 << 2)	/* tx buffer empty */
#define DS1WM_INT_TSRE	    (1 << 3)	/* tx shift register empty */
#define DS1WM_INT_RBF	    (1 << 4)	/* rx buffer full */
#define DS1WM_INT_RSRF	    (1 << 5)	/* rx shift register full */
#define DS1WM_INT_OW_SHORT  (1 << 6)
#define DS1WM_INT_OW_LOW    (1 << 7)

#define DS1WM_INTEN_EPD	    (1 << 0)	/* enable presence detect int */
#define DS1WM_INTEN_IAS	    (1 << 1)	/* INTR active state */
#define DS1WM_INTEN_ETBE    (1 << 2)	/* enable tx buffer empty int */
#define DS1WM_INTEN_ETMT    (1 << 3)	/* enable tx shift register empty int */
#define DS1WM_INTEN_ERBF    (1 << 4)	/* enable rx buffer full int */
#define DS1WM_INTEN_ERSRF   (1 << 5)	/* enable rx shift register full int */
#define DS1WM_INTEN_DQO	    (1 << 6)	/* enable direct bus driving ops */
#define DS1WM_INTEN_EOWL    (1 << 7)
#define DS1WM_INTEN_NOT_IAS (~DS1WM_INTEN_IAS)	/* all but INTR active state */

#define DS1WM_TIMEOUT (HZ * 5)


#define D1W_BASE_ADDRESS    0x180ab000
#define D1W_CLK_GATE_CTRL   0x180aa024
#define D1W_CLK_GATE_EN     (1<<5)
#define D1W_CLOCK_RATE      80000000
#define CRMU_IOMUX_CTRL4    0x0301d0d8

#define D1W_DIN_OFFSET  0x00
#define D1W_DOUT_OFFSET 0x04
#define D1W_ADDR_OFFSET 0x08
#define D1W_CTRL_OFFSET 0x0C

#define SLAVE_CMD_SKIPROM 0xCC
#define SLAVE_CMD_READROM 0x33

typedef struct _ds1wm_info
{
    unsigned int base_addr;
    unsigned char slave_present;
} ds1wm_info_t;


extern ds1wm_info_t *d1w_master;
extern int ds1wm_reset(ds1wm_info_t *ds1wm);
extern void ds1wm_enable(ds1wm_info_t *ds1wm);
extern void ds1wm_disable(ds1wm_info_t *ds1wm);
extern int ds1wm_write_bytes(ds1wm_info_t *ds1wm, unsigned char *data, int data_len);
extern int ds1wm_read_bytes(ds1wm_info_t *ds1wm, unsigned char *data, int data_len);
extern int ds1wm_init(void);

#endif
