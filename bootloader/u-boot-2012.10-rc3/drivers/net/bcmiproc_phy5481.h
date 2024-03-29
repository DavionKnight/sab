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
 *
 * These routines provide access to the external phy
 *
 */

#ifndef _bcm_iproc_phy5481_h_
#define _bcm_iproc_phy5481_h_


/* ---- Include Files ---------------------------------------------------- */
#include <config.h>
#include <common.h>
#include <net.h>
#include <asm/arch/bcmenetphy.h>
#include <asm/arch/iproc_regs.h>
#include <asm/arch/iproc_gmac_regs.h>
#include <asm/iproc/reg_utils.h>
#include <asm/arch/ethHw.h>
#include <asm/arch/bcmutils.h>
#include "ethHw_data.h"

/* Indirect PHY register address flags */
#define SOC_PHY_REG_RESERVE_ACCESS    0x20000000
#define SOC_PHY_REG_1000X             0x40000000
#define SOC_PHY_REG_INDIRECT          0x80000000
#define _SOC_PHY_REG_DIRECT ((SOC_PHY_REG_1000X << 1) | (SOC_PHY_REG_1000X >> 1))

/* ---- External Function Prototypes ------------------------------------- */

extern int phy5481_wr_reg(uint phyaddr, uint32 flags, uint16 reg_bank, uint8 reg_addr, uint16 *data);
extern int phy5481_rd_reg(uint phyaddr, uint32 flags, uint16 reg_bank, uint8 reg_addr, uint16 *data);
extern int phy5481_mod_reg(uint phyaddr, uint32 flags, uint16 reg_bank, uint8 reg_addr, uint16 data, uint16 mask);
extern int phy5481_init(uint phyaddr);
extern int phy5481_link_get(uint phyaddr, int *link);
extern int phy5481_speed_get(uint phyaddr, int *speed, int *duplex);

#endif	/* _bcm_iproc_phy5481_h_ */
