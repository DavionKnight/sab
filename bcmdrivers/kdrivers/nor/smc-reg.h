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


#define BCM539xx_SMC_REG_START  0x18021000
#define BCM539xx_SMC_REG_SIZE   0x1000

#define BCM539xx_SMC_NOR_MEM_START 0x20000000
/* size window up to 96 MB */
#define BCM539xx_SMC_NOR_MEM_MAX 0x6000000 
#define BCM539XX_S29GL128_SIZE  0x1000000

#define HR2_SMC_REG_BASE_ADDR    BCM539xx_SMC_REG_START

/* SMC register definition */
#define SMC_MEMC_STATUS    0x0
#define SMC_MEMIF_CFG      0x4
#define SMC_CFG_SET        0x8
#define SMC_CFG_CLR        0xC
#define SMC_DIRECT_CMD     0x010
#define SMC_SET_CYCLES     0x014
#define SMC_SETOPMODE      0x018
#define SMC_REFRESH_0      0x020
#define SMC_REFRESH_1      0x24
/* 0x028 ~ 0x0FC reerved */
#define SMC_SRAM_CYCLES0_0   0x100
#define SMC_SRAM_CYCLES0_1   0x120
#define SMC_SRAM_CYCLES0_2   0x140
#define SMC_SRAM_CYCLES0_3   0x160
#define SMC_SRAM_CYCLES1_0   0x180
#define SMC_SRAM_CYCLES1_1   0x1A0
#define SMC_SRAM_CYCLES1_2   0x1C0
#define SMC_SRAM_CYCLES1_3   0x1E0

#define SMC_OPMODE0_0        0x104
#define SMC_OPMODE0_1        0x124
#define SMC_OPMODE0_2        0x144
#define SMC_OPMODE0_3        0x164
#define SMC_OPMODE1_0        0x184
#define SMC_OPMODE1_1        0x1A4
#define SMC_OPMODE1_2        0x1C4
#define SMC_OPMODE1_3        0x1E4

#define SMC_USER_STATUS      0x200
#define SMC_user_CONFIG      0x204

/* 0x208 ~ 0x2FC reserved */

#define SMC_ECC0_STATUS      0x300
#define SMC_ECC1_STATUS      0x400

#define SMC_ECC0_CFG         0x304
#define SMC_ECC1_CFG         0x404

#define SMC_ECC0_MEMCMD0     0x308
#define SMC_ECC1_MEMCMD0     0x408

#define SMC_ECC0_MEMCMD1     0x30C
#define SMC_ECC1_MEMCMD1     0x40C

#define SMC_ECC0_ADDR0       0x310
#define SMC_ECC1_ADDR0       0x410

#define SMC_ECC0_ADDR1       0x314
#define SMC_ECC1_ADDR1       0x414

#define SMC_ECC0_BLOCK0      0x318
#define SMC_ECC0_BLOCK1      0x31C
#define SMC_ECC0_BLOCK2      0x320
#define SMC_ECC0_BLOCK3      0x324

#define SMC_ECC1_BLOCK0      0x418
#define SMC_ECC1_BLOCK1      0x41C
#define SMC_ECC1_BLOCK2      0x420
#define SMC_ECC1_BLOCK3      0x424

#define SMC_ECC0_EXTRA_BLOCK 0x328
#define SMC_ECC1_EXTRA_BLOCK 0x428

/* 0x32C ~ 0x3FC reserved */
/* 0x42C ~ 0x4FC reserved */

#define SMC_INT_CFG          0xE00
#define SMC_INT_INPUTS       0xE04
#define SMC_INT_OUTPUTS      0xE08

/* 0xE0C ~ 0xFDC reserved */

#define SMC_PERIPH_ID_0      0xFE0
#define SMC_PERIPH_ID_1      0xFE4
#define SMC_PERIPH_ID_2      0xFE8
#define SMC_PERIPH_ID_3      0xFEC

#define SMC_PCELL_ID_0       0xFF0
#define SMC_PCELL_ID_1       0xFF4
#define SMC_PCELL_ID_2       0xFF8
#define SMC_PCELL_ID_3       0xFFC
