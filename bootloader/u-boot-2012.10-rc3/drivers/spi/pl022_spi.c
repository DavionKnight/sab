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
#include <spi.h>
#include <asm/io.h>
#include <asm/arch/socregs.h>

/* base address must be address of SPI0, spi: 0-5 */
#define SPI_ADDR(base_addr,spi) \
    (base_addr + (spi) * (ChipcommonG_SPI1_SSPCR0 - ChipcommonG_SPI0_SSPCR0))

/* setup spi io multiplex */
extern void iproc_spi_iomux(int spi, int op);

#define SCR_MAX		0xFF
#define CPSR_MAX	0xFE

/* pending rx bytes after last cmd */
static int pending;

void pl022_spi_init(void) {
}

struct spi_slave *pl022_spi_setup_slave(unsigned int bus, unsigned int cs,
        unsigned int max_hz, unsigned int mode) {
    struct spi_slave *slave;
    u16 scr = 1, prescaler, cr0 = 0, cpsr = 0;
    uint32_t data;

    if (bus < CONFIG_PL022_SPI_BUS ) {
        return NULL;
    }

    slave = malloc(sizeof(*slave));
    if (!slave) {
        return NULL;
    }

    slave->bus = bus;
    slave->cs = cs;

    /* Setup io multiplex */
    iproc_spi_iomux(cs, 1);

    if (bus != CONFIG_PL022_SPI_BUS) {
        data = 0;
        data |= (1 << ChipcommonG_SPI0_SSPCR0__SCR_R);
        if (mode & SPI_CPHA)
            data |= (1 << ChipcommonG_SPI0_SSPCR0__SPH);
        if (mode & SPI_CPOL)
            data |= (1 << ChipcommonG_SPI0_SSPCR0__SPO);
        data |= (0 << ChipcommonG_SPI0_SSPCR0__FRF_R); //moto
        data |= (7 << ChipcommonG_SPI0_SSPCR0__DSS_R);
        writel(data, SPI_ADDR(ChipcommonG_SPI0_SSPCR0, cs));
        return slave;
    }


    /* PID should be 0x00041022 */
    if ((readl(SPI_ADDR(ChipcommonG_SPI0_SSPPeriphID0, slave->cs)) == 0x22)
            && (readl(SPI_ADDR(ChipcommonG_SPI0_SSPPeriphID1, slave->cs))
                    == 0x10)
            && ((readl(SPI_ADDR(ChipcommonG_SPI0_SSPPeriphID2, slave->cs)) & 0xf)
                    == 0x04)
            && (readl(SPI_ADDR(ChipcommonG_SPI0_SSPPeriphID3, slave->cs))
                    == 0x00)) {
    } else {
        free(slave);
        return NULL;
    }

    /* Disable SSP */
    writel(0x0, SPI_ADDR(ChipcommonG_SPI0_SSPCR1, slave->cs));

    /* Set requested polarity and 8bit mode */
    cr0 = 7; /* 8 bits; */
    cr0 |= (mode & SPI_CPHA) ? (1 << ChipcommonG_SPI0_SSPCR0__SPH) : 0;
    cr0 |= (mode & SPI_CPOL) ? (1 << ChipcommonG_SPI0_SSPCR0__SPO) : 0;

    writel(cr0, SPI_ADDR(ChipcommonG_SPI0_SSPCR0, slave->cs));

    /* Program the SSPClk frequency */
    prescaler = CONFIG_SYS_SPI_CLK / max_hz;

    if (prescaler <= 0xFF) {
        cpsr = prescaler;
    } else {
        for (scr = 1; scr <= SCR_MAX; scr++) {
            if (!(prescaler % scr)) {
                cpsr = prescaler / scr;
                if (cpsr <= CPSR_MAX)
                    break;
            }
        }

        if (scr > SCR_MAX) {
            scr = SCR_MAX;
            cpsr = prescaler / scr;
            cpsr &= CPSR_MAX;
        }
    }

    if (cpsr & 0x1)
        cpsr++;

    writel(cpsr, SPI_ADDR(ChipcommonG_SPI0_SSPCPSR, slave->cs));

    cr0 = readl(SPI_ADDR(ChipcommonG_SPI0_SSPCR0, slave->cs));
    writel(cr0 | (scr - 1) << ChipcommonG_SPI0_SSPCR0__SCR_R,
            SPI_ADDR(ChipcommonG_SPI0_SSPCR0, slave->cs));

    return slave;
}

void pl022_spi_free_slave(struct spi_slave *slave) {
    free(slave);
}

int pl022_spi_claim_bus(struct spi_slave *slave) {

    pending = 0;

    /* Enable the SPI hardware */
    if (slave->bus == CONFIG_PL022_SPI_BUS) {
        writel(1 << ChipcommonG_SPI0_SSPCR1__SSE,
            SPI_ADDR(ChipcommonG_SPI0_SSPCR1, slave->cs));
    }

    return 0;
}

void pl022_spi_release_bus(struct spi_slave *slave) {
    /* Disable SPI */
    if (slave->bus == CONFIG_PL022_SPI_BUS){
        writel(0x0, SPI_ADDR(ChipcommonG_SPI0_SSPCR1, slave->cs));
    }
}

int ccg_spi_transfer(uint8_t cs, uint8_t len, const uint8_t *wr_ptr, uint8_t *rd_ptr) {
    uint32_t i;
    uint32_t data;

    if (wr_ptr == NULL || rd_ptr == NULL)
        return -1;

    /* enable ssp */
    writel((1 << ChipcommonG_SPI0_SSPCR1__SSE), SPI_ADDR(ChipcommonG_SPI0_SSPCR1, cs));

    /* wait if busy */
    do {
        data = readl(SPI_ADDR(ChipcommonG_SPI0_SSPSR, cs));
        data = (data >> ChipcommonG_SPI0_SSPSR__BSY) & 1;
    } while (data);


    /* write all data */
    for(i=0; i<len; i++) {
        writel( *(wr_ptr+i), SPI_ADDR(ChipcommonG_SPI0_SSPDR, cs));
    }

    /* wait if busy */
    do {
        data = readl(SPI_ADDR(ChipcommonG_SPI0_SSPSR, cs));
        data = (data >> ChipcommonG_SPI0_SSPSR__BSY) & 1;
    } while (data);

    /* read until receive fifo not empty */
    i=0;
    while (readl(SPI_ADDR(ChipcommonG_SPI0_SSPSR, cs)) & (1 << ChipcommonG_SPI0_SSPSR__RNE)) {
        data = readl(SPI_ADDR(ChipcommonG_SPI0_SSPDR, cs));
        //print_log("READ_DATA[%d] = 0x%08X\n", i, data);
        *(rd_ptr+i) = data;
        i++;
    }

    /* disable ssp */
    writel(0, SPI_ADDR(ChipcommonG_SPI0_SSPCR1, cs));

    return 0;
}

/* one invocation one frame */
int spi_long_transfer(uint8_t cs, uint8_t len, const uint8_t *wr_ptr, uint8_t *rd_ptr) {
    uint32_t i, j=0, stat;
    uint32_t data;

    if (wr_ptr == NULL || rd_ptr == NULL)
        return -1;

    /* enable ssp */
    writel((1 << ChipcommonG_SPI0_SSPCR1__SSE), SPI_ADDR(ChipcommonG_SPI0_SSPCR1, cs));

    /* wait if busy */
    do {
        data = readl(SPI_ADDR(ChipcommonG_SPI0_SSPSR, cs));
        data = (data >> ChipcommonG_SPI0_SSPSR__BSY) & 1;
    } while (data);


    /* write all data */
    for(i=0; i<len; ) {
        stat = readl(SPI_ADDR(ChipcommonG_SPI0_SSPSR, cs));
        if((stat >> ChipcommonG_SPI0_SSPSR__TNF) & 1) {
            writel( *(wr_ptr+i), SPI_ADDR(ChipcommonG_SPI0_SSPDR, cs));
            i++;
        }
        if((stat >> ChipcommonG_SPI0_SSPSR__RNE) & 1)
        {
            *(rd_ptr+j) = readl(SPI_ADDR(ChipcommonG_SPI0_SSPDR, cs));
            j++;
        }
    }

    /* read until receive fifo not empty */
    while (j < len) {
        if (readl(SPI_ADDR(ChipcommonG_SPI0_SSPSR, cs)) & (1 << ChipcommonG_SPI0_SSPSR__RNE)){
            data = readl(SPI_ADDR(ChipcommonG_SPI0_SSPDR, cs));
            //print_log("READ_DATA[%d] = 0x%08X\n", i, data);
            *(rd_ptr+j) = data;
            j++;
        }
    }

    /* disable ssp */
    writel(0, SPI_ADDR(ChipcommonG_SPI0_SSPCR1, cs));

    return 0;
}

/*
 * Support multiple invokes for one frame, device speed has to be low and no delay between
 * invokes.
 *
 * Flash api prefer to send cmd and receive separately, if read input after cmd at once,
 * PL022 will reach to end of tx buffer, deassert CS, then never read result...
 * DO NOT print to console between xfer of cmd and data, need to catch up sending cmd!
 *
 * if dout and din is null:
 *  don't read result
 * if din and dout is null:
 *  drop n bytes first, then read
 */
int pl022_spi_xfer(struct spi_slave *slave, unsigned int bitlen,
        const void *dout, void *din, unsigned long flags) {
    u32 len_tx = 0, len_rx = 0, len;
    u32 ret = 0, stats;
    const u8 *txp = dout;
    u8 *rxp = din, value;

    if (bitlen == 0)
        return ret;

    if (bitlen % 8) {
        ret = -1;
        return ret;
    }

    if (rxp && ((flags & SPI_XFER_END)==0)) {
        printf("PL022 will stop frame after all data read\n");
        ret = -1;
        return ret;
    }

    len = bitlen / 8;


    if (slave->bus == (CONFIG_PL022_SPI_BUS +1)){
        return ccg_spi_transfer(slave->cs, len, txp, rxp);
    } else if (slave->bus == (CONFIG_PL022_SPI_BUS + 2)) {
        return spi_long_transfer(slave->cs, len, txp, rxp);
    }

    if (len+pending > 16) {
        printf("exceed pl022 max tx fifo cmd:%d, data:%d\n", pending, len);
        return -1;
    }
    /* if has a command, clear buffer */
    if (flags & SPI_XFER_BEGIN) {
        do {
            stats = readl(SPI_ADDR(ChipcommonG_SPI0_SSPSR, slave->cs));
            if (stats & (1 << ChipcommonG_SPI0_SSPSR__RNE)) { /* receive fifo not empty */
                value = readl(SPI_ADDR(ChipcommonG_SPI0_SSPDR, slave->cs));
            }
        } while ((stats & (1 << ChipcommonG_SPI0_SSPSR__BSY)) || (stats & (1 << ChipcommonG_SPI0_SSPSR__RNE)));
        pending = 0;
    }


    /* Send */
    while (len_tx < len) {
            value = (txp != NULL) ? *txp++ : 0;
            writel(value, SPI_ADDR(ChipcommonG_SPI0_SSPDR, slave->cs));
            len_tx++;
    }

    if (rxp == NULL && (SPI_XFER_END & flags) == 0) {
        /* cmd, read result will cause CS end */
        pending = len;
    } else {
        /* rx, wait until got all */
        while (pending > 0 || len_rx < len_tx) {
            stats = readl(SPI_ADDR(ChipcommonG_SPI0_SSPSR, slave->cs));
            if (stats & (1 << ChipcommonG_SPI0_SSPSR__RNE)) { /* receive fifo not empty */
                value = readl(SPI_ADDR(ChipcommonG_SPI0_SSPDR, slave->cs));
                if (pending) {
                    pending--;
                }else{
                    if (rxp)
                        *rxp++ = value;
                    len_rx++;
                }
            } else if ((stats & (1 << ChipcommonG_SPI0_SSPSR__BSY)) == 0) {
                ret = -1;
                return ret;
            }
        }
    }

    if (flags & SPI_XFER_END) {
        do {
            stats = readl(SPI_ADDR(ChipcommonG_SPI0_SSPSR, slave->cs));
            if (stats & (1 << ChipcommonG_SPI0_SSPSR__RNE)) { /* receive fifo not empty */
                value = readl(SPI_ADDR(ChipcommonG_SPI0_SSPDR, slave->cs));
            }
        } while ((stats & (1 << ChipcommonG_SPI0_SSPSR__BSY)) || (stats & (1 << ChipcommonG_SPI0_SSPSR__RNE)));
    }

    return ret;
}
