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
#include <malloc.h>
#include <sdhci.h>

#include <asm/arch/iproc_regs.h>

struct iproc_sdhci_host {
    struct sdhci_host host;
    u32 shadow_cmd;
    u32 shadow_blk;
};

static inline void iproc_sdhci_raw_writel(struct sdhci_host *host, u32 val, int reg)
{
    writel(val, host->ioaddr + reg);
}

static inline u32 iproc_sdhci_raw_readl(struct sdhci_host *host, int reg)
{
    return readl(host->ioaddr + reg);
}

static void iproc_sdhci_writel(struct sdhci_host *host, u32 val, int reg)
{
    iproc_sdhci_raw_writel(host, val, reg);
}

static void iproc_sdhci_writew(struct sdhci_host *host, u16 val, int reg)
{
    struct iproc_sdhci_host *iproc_host = (struct iproc_sdhci_host *)host;
    u32 oldval, newval;
    u32 word_num = (reg >> 1) & 1;
    u32 word_shift = word_num * 16;
    u32 mask = 0xffff << word_shift;

    if (reg == SDHCI_COMMAND) {
        if (iproc_host->shadow_blk != 0) {
            iproc_sdhci_raw_writel(host, iproc_host->shadow_blk, SDHCI_BLOCK_SIZE);
            iproc_host->shadow_blk = 0;
        }
        oldval = iproc_host->shadow_cmd;
    } else if (reg == SDHCI_BLOCK_SIZE || reg == SDHCI_BLOCK_COUNT) {
        oldval = iproc_host->shadow_blk;
    } else {
        oldval = iproc_sdhci_raw_readl(host, reg & ~3);
    }
    newval = (oldval & ~mask) | (val << word_shift);

    if (reg == SDHCI_TRANSFER_MODE) {
        iproc_host->shadow_cmd = newval;
    } else if (reg == SDHCI_BLOCK_SIZE || reg == SDHCI_BLOCK_COUNT) {
        iproc_host->shadow_blk = newval;
    } else {
        iproc_sdhci_raw_writel(host, newval, reg & ~3);
    }
}

static void iproc_sdhci_writeb(struct sdhci_host *host, u8 val, int reg)
{
    u32 oldval, newval;
    u32 byte_num = reg & 3;
    u32 byte_shift = byte_num * 8;
    u32 mask = 0xff << byte_shift;

    oldval = iproc_sdhci_raw_readl(host, reg & ~3);
    newval = (oldval & ~mask) | (val << byte_shift);
    
    iproc_sdhci_raw_writel(host, newval, reg & ~3);
}

static u32 iproc_sdhci_readl(struct sdhci_host *host, int reg)
{
    return iproc_sdhci_raw_readl(host, reg);
}

static u16 iproc_sdhci_readw(struct sdhci_host *host, int reg)
{
    u32 val;
    u32 word_num = (reg >> 1) & 1;
    u32 word_shift = word_num * 16;

    val = iproc_sdhci_raw_readl(host, (reg & ~3));
    return (val >> word_shift) & 0xffff;
}

static u8 iproc_sdhci_readb(struct sdhci_host *host, int reg)
{
    u32 val;
    u32 byte_num = reg & 3;
    u32 byte_shift = byte_num * 8;

    val = iproc_sdhci_raw_readl(host, (reg & ~3));
    return (val >> byte_shift) & 0xff;
}

static const struct sdhci_ops iproc_sdhci_ops = {
    .write_l = iproc_sdhci_writel,
    .write_w = iproc_sdhci_writew,
    .write_b = iproc_sdhci_writeb,
    .read_l = iproc_sdhci_readl,
    .read_w = iproc_sdhci_readw,
    .read_b = iproc_sdhci_readb,
};

int board_mmc_init(bd_t *bis)
{
    struct iproc_sdhci_host *iproc_host;
    struct sdhci_host *host;
    u32 val;
    
    iproc_host = malloc(sizeof(*iproc_host));
    if (!iproc_host) {
        printf("sdhci_host malloc fail!\n");
        return 1;
    }

    /* Enable the SDIO clock */
    val = readl((void *)SDIO_IDM0_IO_CONTROL_DIRECT);
    val |= (1 << SDIO_IDM0_IO_CONTROL_DIRECT__clk_enable);
    writel(val, (void *)SDIO_IDM0_IO_CONTROL_DIRECT);
    
    /* Release reset */
    writel(0x0, (void *)SDIO_IDM0_IDM_RESET_CONTROL);

    host = &iproc_host->host;
    host->name = "iproc_sdhci";
    host->ioaddr = (void *)SDIO0_eMMCSDXC_SYSADDR;
    host->quirks = SDHCI_QUIRK_WAIT_SEND_CMD;
    host->ops = &iproc_sdhci_ops;
    host->version = sdhci_readw(host, SDHCI_HOST_VERSION);

    debug("SDIO controller capabilities, cap1: %.8x and cap2: %.8x\n", 
                                sdhci_readl(host, SDHCI_CAPABILITIES),
                                sdhci_readl(host, SDHCI_CAPABILITIES_1));
    add_sdhci(host, 0, 0);
    return 0;
}
