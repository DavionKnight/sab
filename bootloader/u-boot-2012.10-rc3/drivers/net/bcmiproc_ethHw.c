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
/**
*  @file    ethHw.c
*
*  @brief   Low level ETH driver routines
*
*  @note
*
*   These routines provide basic ETH functionality only. Intended for use
*   with boot and other simple applications.
*/
/****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */
#include <config.h>
#include <common.h>
#include <net.h>
#include <asm/arch/bcmenetphy.h>
#include <asm/arch/iproc_regs.h>
#include <asm/arch/iproc_gmac_regs.h>
#include <asm/iproc/reg_utils.h>
#include <asm/arch/ethHw.h>
#include <asm/arch/ethHw_dma.h>
#include <asm/arch/bcmgmacmib.h>
#include <asm/arch/bcmutils.h>
#include <asm/armv7.h>
#include <asm/iproc/iproc_common.h>
#include "ethHw_data.h"
#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_SABER2))
#include "bcmiproc_serdes.h"
#include "bcmiproc_phy5461s.h"
#elif defined(CONFIG_HURRICANE2)
#include "bcmiproc_phy5221.h"
#elif (defined(CONFIG_GREYHOUND) || defined(CONFIG_HURRICANE3))
#include "bcmiproc_phy.h"
#include "bcmiproc_phy5481.h"
#endif 

/* ---- External Variable Declarations ----------------------------------- */
/* ---- External Function Prototypes ------------------------------------- */
/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */
/* ---- Private Variables ------------------------------------------------ */
/* debug/trace */
//#define BCMDBG
#define BCMDBG_ERR
#ifdef BCMDBG
#define	ET_ERROR(args) printf args
#define	ET_TRACE(args) printf args
#define BCMIPROC_DMA_DEBUG
#define BCMIPROC_ETH_DEBUG
#elif defined(BCMDBG_ERR)
#define	ET_ERROR(args) printf args
#define ET_TRACE(args)
#else
#define	ET_ERROR(args)
#define	ET_TRACE(args)
#endif /* BCMDBG */

bcm_eth_t g_eth_data[CONFIG_GMAC_NUM];

uint32_t rxDescBuf = 0;
uint32_t rxDescAlignedBuf = 0;
uint32_t rxDataBuf = 0;
uint32_t txDescBuf = 0;
uint32_t txDescAlignedBuf = 0;
uint32_t txDataBuf = 0;

#ifndef ASSERT
#define ASSERT(exp)
#endif

static int iproc_get_chipid(void);
static void gmac_dump_regs(bcm_eth_t *eth_data);
static void gmac_dump_txmib(bcm_eth_t *eth_data);
static void gmac_dump_rxmib(bcm_eth_t *eth_data);
#ifdef BCMIPROC_DMA_DEBUG
static void dma_txdump(bcm_eth_t *eth_data);
static void dma_rxdump(bcm_eth_t *eth_data);
#endif /* BCMIPROC_DMA_DEBUG */
static uint32_t dma_ctrlflags(dma_info_t *di, uint mask, uint flags);
static int dma_rxenable(dma_info_t *di);
static void dma_txinit(dma_info_t *di);
static void dma_rxinit(dma_info_t *di);
static bool dma_txreset(dma_info_t *di);
static bool dma_rxreset(dma_info_t *di);
static int dma_txload(int index, size_t len, uint8_t * tx_buf);
static void *dma_getnextrxp(dma_info_t *di, int *index, size_t *len, uint32_t *stat0, uint32_t *stat1);
static void dma_rxrefill(dma_info_t *di, int index);
#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_SABER2))
static void gmac_mdio_set(int en);
#endif /* (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_SABER2)) */
static uint32_t gmac_intevent_get(bcm_eth_t *eth_data, bool in_isr);
static void gmac_core_reset(void);
static void gmac_init_reset(bcm_eth_t *eth_data);
static void gmac_clear_reset(bcm_eth_t *eth_data);
static void gmac_reset(bcm_eth_t *eth_data);
static void gmac_clearmib(bcm_eth_t *eth_data);
static int gmac_speed(bcm_eth_t *eth_data, uint32_t speed);
static void gmac_tx_flowcontrol(bcm_eth_t *eth_data, bool on);
static void gmac_promisc(bcm_eth_t *eth_data, bool mode);
static void gmac_enable(bcm_eth_t *eth_data, bool en);
#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_SABER2))
static void gmac_serdes_init(bcm_eth_t *eth_data);
#endif /*(defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_SABER2))*/
static void chip_init(bcm_eth_t *eth_data, uint options);
static void chip_reset(bcm_eth_t *eth_data);

static int
iproc_get_chipid(void)
{
	volatile u32 val;
	val = reg32_read((volatile unsigned int *)ChipcommonA_ChipID);
	return (val  & 0xFFFF);
}

#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_SABER2))
static void 
gmac_mdio_set(int en)
{
	u32 tmp;
	
	tmp = reg32_read((volatile unsigned int *)IPROC_WRAP_MISC_CONTROL);

	if (en) {
		/* set bits 3&2 so AMAC can access the Serdes and Phy */
        tmp |= (1 << IPROC_WRAP_MISC_CONTROL__IPROC_MDIO_SEL);
#if defined(CONFIG_HELIX4)
		tmp |= ((1 << IPROC_WRAP_MISC_CONTROL__QUAD_SERDES_MDIO_SEL) |
                (1 << IPROC_WRAP_MISC_CONTROL__QUAD_SERDES_CTRL_SEL));
#elif defined(CONFIG_KATANA2)
		tmp |= ((1 << IPROC_WRAP_MISC_CONTROL__UNICORE_SERDES_MDIO_SEL) |
                (1 << IPROC_WRAP_MISC_CONTROL__UNICORE_SERDES_CTRL_SEL));
#endif
	} else {
		/* clear bits 3&2 so CMIC can access the Serdes and Phy */
        tmp &= ~(1 << IPROC_WRAP_MISC_CONTROL__IPROC_MDIO_SEL);
#if defined(CONFIG_HELIX4)
		tmp &= ~((1 << IPROC_WRAP_MISC_CONTROL__QUAD_SERDES_MDIO_SEL) |
                 (1 << IPROC_WRAP_MISC_CONTROL__QUAD_SERDES_CTRL_SEL));
#elif defined(CONFIG_KATANA2)
		tmp &= ~((1 << IPROC_WRAP_MISC_CONTROL__UNICORE_SERDES_MDIO_SEL) |
                 (1 << IPROC_WRAP_MISC_CONTROL__UNICORE_SERDES_CTRL_SEL));
#endif
	}

	reg32_write((volatile uint32_t *)IPROC_WRAP_MISC_CONTROL, tmp);
}
#endif /* (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_SABER2)) */

/* Get current and pending interrupt events */
static uint32_t
gmac_intevent_get(bcm_eth_t *eth_data, bool in_isr)
{
	gmacregs_t *regs = eth_data->regs;
	bcmgmac_t *ch = &eth_data->bcmgmac;
	uint32_t intstatus;

	/* Read the interrupt status register */
	intstatus = reg32_read(&regs->int_status);
	
	/* Clear the int bits */
	reg32_write(&regs->int_status, intstatus);
	reg32_read(&regs->int_status);

	/* Defer unsolicited interrupts */
	intstatus &= (in_isr ? ch->intmask : ch->def_intmask);
	intstatus = (ch->intstatus |= intstatus);

	/* Return intstatus */
	return intstatus;
}

/* reset all 4 GMAC Cores */
static void
gmac_core_reset(void)
{
	uint32 data;
	
#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
	/* Write 0 to core reset control reg */
	/* Write command config reg */
	reg32_write((uint32_t*)(AMAC_IDM0_IDM_RESET_CONTROL),0);
	reg32_write((uint32_t*)(AMAC_IDM1_IDM_RESET_CONTROL),0);
	data = reg32_read((uint32_t*)(AMAC_IDM0_IO_CONTROL_DIRECT));
	data &= ~(1 << AMAC_IDM0_IO_CONTROL_DIRECT__CLK_250_SEL);
	data |= (1 << AMAC_IDM0_IO_CONTROL_DIRECT__DIRECT_GMII_MODE);
	data |= (1 << AMAC_IDM0_IO_CONTROL_DIRECT__DEST_SYNC_MODE_EN);
	reg32_write((uint32_t*)(AMAC_IDM0_IO_CONTROL_DIRECT),data);
	
	data = reg32_read((uint32_t*)(AMAC_IDM1_IO_CONTROL_DIRECT));
	data &= ~(1 << AMAC_IDM1_IO_CONTROL_DIRECT__CLK_250_SEL);
	data |= (1 << AMAC_IDM1_IO_CONTROL_DIRECT__DIRECT_GMII_MODE);
	data |= (1 << AMAC_IDM1_IO_CONTROL_DIRECT__DEST_SYNC_MODE_EN);
	reg32_write((uint32_t*)(AMAC_IDM1_IO_CONTROL_DIRECT),data);
#elif (defined(CONFIG_HURRICANE2) || defined(CONFIG_GREYHOUND) || \
	defined(CONFIG_HURRICANE3))
	/* Write 0 to core reset control reg */
	/* Write command config reg */
	reg32_write((uint32_t*)(AMAC_IDM0_IDM_RESET_CONTROL),0);
	
	data = reg32_read((uint32_t*)(AMAC_IDM0_IO_CONTROL_DIRECT));
	data &= ~(1<<AMAC_IDM0_IO_CONTROL_DIRECT__CLK_250_SEL);
	data |= (1<<AMAC_IDM0_IO_CONTROL_DIRECT__DIRECT_GMII_MODE);
	data |= (1<<AMAC_IDM0_IO_CONTROL_DIRECT__DEST_SYNC_MODE_EN);
	reg32_write((uint32_t*)(AMAC_IDM0_IO_CONTROL_DIRECT),data);
#endif
#if defined(CONFIG_SABER2)
	/* Write 0 to core reset control reg */
	/* Write command config reg */

	reg32_write((uint32_t*)(AMAC_IDM0_IDM_RESET_CONTROL),0);
	reg32_write((uint32_t*)(AMAC_IDM1_IDM_RESET_CONTROL),0);
	
	data = reg32_read((uint32_t*)(AMAC_IDM0_IO_CONTROL_DIRECT));
	data &= ~(1<<AMAC_IDM0_IO_CONTROL_DIRECT__CLK_250_SEL);
	data |= (1<<AMAC_IDM0_IO_CONTROL_DIRECT__DIRECT_GMII_MODE);
	data |= (1<<AMAC_IDM0_IO_CONTROL_DIRECT__DEST_SYNC_MODE_EN);
	reg32_write((uint32_t*)(AMAC_IDM0_IO_CONTROL_DIRECT),data);
	
	data = reg32_read((uint32_t*)(AMAC_IDM1_IO_CONTROL_DIRECT));
	data &= ~(1<<AMAC_IDM1_IO_CONTROL_DIRECT__CLK_250_SEL);
	data |= (1<<AMAC_IDM1_IO_CONTROL_DIRECT__DIRECT_GMII_MODE);
	data |= (1<<AMAC_IDM1_IO_CONTROL_DIRECT__DEST_SYNC_MODE_EN);
	reg32_write((uint32_t*)(AMAC_IDM1_IO_CONTROL_DIRECT),data);
#endif
}

static void
gmac_init_reset(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;

	/* Set command config reg CC_SR */
	reg32_set_bits(&regs->cmd_cfg, CC_SR);
	udelay(GMAC_RESET_DELAY);
}


static void
gmac_clear_reset(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;

	/* Clear command config reg CC_SR */
	reg32_clear_bits(&regs->cmd_cfg, CC_SR);
	udelay(GMAC_RESET_DELAY);
}


static void
gmac_reset(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;
	uint32_t ocmdcfg, cmdcfg;

	/* Read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);
	ocmdcfg = cmdcfg;

	cmdcfg &= ~(CC_TE | CC_RE | CC_RPI | CC_TAI | CC_HD | CC_ML |
			CC_CFE | CC_RL | CC_RED | CC_PE | CC_TPI | CC_PAD_EN | CC_PF);
	cmdcfg |= (CC_PROM | CC_NLC | CC_CFE);

	/* Check if need to write register back */
	if (cmdcfg != ocmdcfg) {
		/* Put mac in reset */
		gmac_init_reset(eth_data);
		/* Write register */
		reg32_write(&regs->cmd_cfg, cmdcfg);
		/* Bring mac out of reset */
		gmac_clear_reset(eth_data);
	}
}


static void
gmac_clearmib(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;
	volatile uint32_t *ptr;

	/* Enable clear on read */
	reg32_set_bits(&regs->dev_ctl, DC_MROR);

	for (ptr = &regs->mib.tx_good_octets; ptr <= &regs->mib.rx_uni_pkts; ptr++) {
		reg32_read(ptr);
		if (ptr == &regs->mib.tx_q3_octets_high) {
			ptr++;
		}
	}

	return;
}


static int
gmac_speed(bcm_eth_t *eth_data, uint32_t speed)
{
	gmacregs_t *regs = eth_data->regs;
	uint32_t cmdcfg, ocmdcfg;
	uint32_t hd_ena = 0;
#if (defined(CONFIG_GREYHOUND) || defined(CONFIG_HURRICANE3))
    uint32_t sdctl;
#endif /* CONFIG_GREYHOUND */

	switch (speed) {
	case ET_10HALF:
		hd_ena = CC_HD;
		/* FALLTHRU */

	case ET_10FULL:
		speed = 0;
		break;

	case ET_100HALF:
		hd_ena = CC_HD;
		/* FALLTHRU */

	case ET_100FULL:
		speed = 1;
		break;

	case ET_1000FULL:
		speed = 2;
		break;

	case ET_1000HALF:
		ET_ERROR(("et%d: gmac_speed: supports 1000 mbps full duplex only\n",
				eth_data->unit));
		return (FAILURE);

	default:
		ET_ERROR(("et%d: gmac_speed: speed %d not supported\n",
				eth_data->unit, speed));
		return (FAILURE);
	}

	/* read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);
	ocmdcfg = cmdcfg;

	/* set the speed */
	cmdcfg &= ~(CC_ES_MASK | CC_HD);
	cmdcfg |= ((speed << CC_ES_SHIFT) | hd_ena);
#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_SABER2))
	cmdcfg |= CC_AE;
#endif

	if (cmdcfg != ocmdcfg) {
		serdes_init(speed); /*mac config update add by zhangjiajie 2017-3-1*/
		/* Put mac in reset */
		gmac_init_reset(eth_data);
		/* Write command config reg */
		reg32_write(&regs->cmd_cfg, cmdcfg);
		/* Bring mac out of reset */
		gmac_clear_reset(eth_data);
	}

#if (defined(CONFIG_GREYHOUND) || defined(CONFIG_HURRICANE3))
    sdctl = reg32_read(&regs->serdes_ctl);
    sdctl &= ~(SC_FORCE_SPD_STRAP_MASK);
    switch (speed) {
        case 2:
            sdctl |= SC_FORCE_SPD_1G_VAL;
            break;
        case 1:
            sdctl |= SC_FORCE_SPD_100M_VAL;
            break;
        default:
            break;
    }
	reg32_write(&regs->serdes_ctl, sdctl);

    udelay(1000);
#endif /* CONFIG_GREYHOUND */
	return (SUCCESS);
}


static void
gmac_tx_flowcontrol(bcm_eth_t *eth_data, bool on)
{
	gmacregs_t *regs = eth_data->regs;
	uint32_t cmdcfg, ocmdcfg;

	/* Read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);
	ocmdcfg = cmdcfg;

	/* Enable tx flow control clear the rx pause ignore bit */
	if (on) {
		cmdcfg &= ~CC_RPI;
	} else {
		cmdcfg |= CC_RPI;
    }
    
	if (cmdcfg != ocmdcfg) {
		/* Put the mac in reset */
		gmac_init_reset(eth_data);
		/* Write command config reg */
		reg32_write(&regs->cmd_cfg, cmdcfg);
		/* Bring mac out of reset */
		gmac_clear_reset(eth_data);
	}
}


static void
gmac_promisc(bcm_eth_t *eth_data, bool mode)
{
	gmacregs_t *regs = eth_data->regs;
	uint32_t cmdcfg, ocmdcfg;

	/* Read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);
	ocmdcfg = cmdcfg;

	/* Enable or disable promiscuous mode */
	if (mode) {
		cmdcfg |= CC_PROM;
	} else {
		cmdcfg &= ~CC_PROM;
    }
    
	if (cmdcfg != ocmdcfg) {
		/* Put the mac in reset */
		gmac_init_reset(eth_data);
		/* Write command config reg */
		reg32_write(&regs->cmd_cfg, cmdcfg);
		/* Bring mac out of reset */
		gmac_clear_reset(eth_data);
	}
}


static void
gmac_enable(bcm_eth_t *eth_data, bool en)
{
	gmacregs_t *regs = eth_data->regs;
	uint32_t cmdcfg;

	/* Read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);

	/* Put mac in reset */
	gmac_init_reset(eth_data);

	cmdcfg |= CC_SR;

	/* First deassert rx_ena and tx_ena while in reset */
	cmdcfg &= ~(CC_RE | CC_TE);
	/* Write command config reg */
	reg32_write(&regs->cmd_cfg, cmdcfg);

	/* Bring mac out of reset */
	gmac_clear_reset(eth_data);

	/* If not enable exit now */
	if (!en) {
		return;
	}

	/* Enable the mac transmit and receive paths now */
	udelay(2);
	cmdcfg &= ~CC_SR;
	cmdcfg |= (CC_RE | CC_TE);

	/* Assert rx_ena and tx_ena when out of reset to enable the mac */
	reg32_write(&regs->cmd_cfg, cmdcfg);

	return;
}


#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_SABER2))
static void
gmac_serdes_init(bcm_eth_t *eth_data)
{  
	/* bring serdes out of reset */
	gmacregs_t *regs =	eth_data->regs;
	uint32_t sdctl, sdstat0, sdstat1;

	sdctl = reg32_read(&regs->serdes_ctl);
	sdstat0 = reg32_read(&regs->serdes_status0);
	sdstat1 = reg32_read(&regs->serdes_status1);
	printf("et%d: %s() serdes_status0: 0x%x; serdes_status1: 0x%x\n", eth_data->unit, __FUNCTION__, sdstat0, sdstat1);

    /* Check is PLL already brought up */
	if (sdstat0 & SS0_TXPLL_LOCK) {
		printf("et%d: %s() PLL ready brought up exit\n", eth_data->unit, __FUNCTION__);
		return;
	}
  
    /*
     * Bring up both digital and analog clocks
     *
     * NOTE: Many MAC registers are not accessible until the PLL is locked.
     * An S-Channel timeout will occur before that.
     */

	sdctl = 0;
#if defined(CONFIG_HELIX4)
    sdctl |= (SC_TX1G_FIFO_RST_VAL|SC_FORCE_SPD_STRAP_VAL|SC_REFSEL_VAL|SC_REF_TERM_SEL_MASK); 
#elif defined(CONFIG_SABER2)
	sdctl |= (SC_TX1G_FIFO_RST_VAL|SC_FORCE_SPD_STRAP_VAL);
#else
	sdctl |= (SC_TX1G_FIFO_RST_VAL|SC_FORCE_SPD_STRAP_VAL|SC_REF_TERM_SEL_MASK);
#endif /* (defined(CONFIG_HELIX4) */

	reg32_write(&regs->serdes_ctl, sdctl);

	udelay(1000);

	sdctl = reg32_read(&regs->serdes_ctl);
	sdctl |= (SC_IDDQ_MASK|SC_PWR_DOWN_MASK);
	reg32_write(&regs->serdes_ctl, sdctl);

	sdctl = reg32_read(&regs->serdes_ctl);
	sdctl &= ~(SC_IDDQ_MASK|SC_PWR_DOWN_MASK);
	reg32_write(&regs->serdes_ctl, sdctl);

    /* Bring hardware out of reset */
	sdctl = reg32_read(&regs->serdes_ctl);
	sdctl |= (SC_RSTB_HW_MASK);
	reg32_write(&regs->serdes_ctl, sdctl);

    /* Bring MDIOREGS out of reset */
	sdctl = reg32_read(&regs->serdes_ctl);
	sdctl |= (SC_RSTB_MDIOREGS_MASK);
	reg32_write(&regs->serdes_ctl, sdctl);

	udelay(1000);

    /* Bring PLL out of reset */
	sdctl = reg32_read(&regs->serdes_ctl);
	sdctl |= (SC_RSTB_PLL_MASK);
	reg32_write(&regs->serdes_ctl, sdctl);

	udelay(1000);

	sdctl = reg32_read(&regs->serdes_ctl);
	sdstat0 = reg32_read(&regs->serdes_status0);
	sdstat1 = reg32_read(&regs->serdes_status1);

	return;
}
#endif /*(defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_SABER2)) */

#ifdef BCMIPROC_DMA_DEBUG
static void
dma_txdump(bcm_eth_t *eth_data)
{
	dma_info_t *dma = eth_data->bcmgmac.di[0];
	dma64dd_t *descp = NULL;
	uint8_t *bufp;
	int i;

	printf("TX DMA Register:\n");
	printf("control:0x%x; ptr:0x%x; addrl:0x%x; addrh:0x%x; stat0:0x%x, stat1:0x%x\n",
			reg32_read(&dma->d64txregs->control),
			reg32_read(&dma->d64txregs->ptr),
			reg32_read(&dma->d64txregs->addrlow),
			reg32_read(&dma->d64txregs->addrhigh),
			reg32_read(&dma->d64txregs->status0),
			reg32_read(&dma->d64txregs->status1));

	printf("TX Descriptors:\n");
	for (i = 0; i < TX_BUF_NUM; i++) {
		descp = (dma64dd_t *) TX_DESC(i);
		printf("ctrl1:0x%08x; ctrl2:0x%08x; addrhigh:0x%x; addrlow:0x%08x\n",
				descp->ctrl1, descp->ctrl2, descp->addrhigh, descp->addrlow);
	}

	printf("TX Buffers:\n");
	/* Initialize TX DMA descriptor table */
	for (i = 0; i < TX_BUF_NUM; i++) {
		bufp = (uint8_t *) TX_BUF(i);
		printf("buf%d:0x%x; ", i, (uint32_t)bufp);
	}
	printf("\n");
}

static void
dma_rxdump(bcm_eth_t *eth_data)
{
	dma_info_t *dma = eth_data->bcmgmac.di[0];
	dma64dd_t *descp = NULL;
	uint8_t *bufp;
	int i;

	printf("RX DMA Register:\n");
	printf("control:0x%x; ptr:0x%x; addrl:0x%x; addrh:0x%x; stat0:0x%x, stat1:0x%x\n",
			reg32_read(&dma->d64rxregs->control),
			reg32_read(&dma->d64rxregs->ptr),
			reg32_read(&dma->d64rxregs->addrlow),
			reg32_read(&dma->d64rxregs->addrhigh),
			reg32_read(&dma->d64rxregs->status0),
			reg32_read(&dma->d64rxregs->status1));

	printf("RX Descriptors:\n");
	for (i = 0; i < RX_BUF_NUM; i++) {
		descp = (dma64dd_t *) RX_DESC(i);
		printf("ctrl1:0x%08x; ctrl2:0x%08x; addrhigh:0x%x; addrlow:0x%08x\n",
				descp->ctrl1, descp->ctrl2, descp->addrhigh, descp->addrlow);
	}

	printf("RX Buffers:\n");
	for (i = 0; i < RX_BUF_NUM; i++) {
		bufp = (uint8_t *) RX_BUF(i);
		printf("buf%d:0x%x; ", i, (uint32_t)bufp);
	}
	printf("\n");
}
#endif /* BCMIPROC_DMA_DEBUG */

static uint32_t
dma_ctrlflags(dma_info_t *di, uint mask, uint flags)
{
	uint dmactrlflags;

	dmactrlflags = di->hnddma.dmactrlflags;
	ASSERT((flags & ~mask) == 0);

	dmactrlflags &= ~mask;
	dmactrlflags |= flags;

	/* If trying to enable parity, check if parity is actually supported */
	if (dmactrlflags & DMA_CTRL_PEN) {
		uint32 control;

		control = reg32_read(&di->d64txregs->control);
		reg32_write(&di->d64txregs->control, control | D64_XC_PD);
		if (reg32_read(&di->d64txregs->control) & D64_XC_PD) {
			/* We *can* disable it so it is supported,
			 * restore control register
			 */
			reg32_write(&di->d64txregs->control, control);
		} else {
			/* Not supported, don't allow it to be enabled */
			dmactrlflags &= ~DMA_CTRL_PEN;
		}
	}

	di->hnddma.dmactrlflags = dmactrlflags;

	return (dmactrlflags);
}


static int
dma_rxenable(dma_info_t *di)
{
	uint32_t dmactrlflags = di->hnddma.dmactrlflags;
	uint32_t rxoffset = di->rxoffset;
	uint32_t rxburstlen = di->rxburstlen;

	uint32 control = (reg32_read(&di->d64rxregs->control) & D64_RC_AE) | D64_RC_RE;

	if ((dmactrlflags & DMA_CTRL_PEN) == 0) {
		control |= D64_RC_PD;
	}

	if (dmactrlflags & DMA_CTRL_ROC) {
		control |= D64_RC_OC;
	}

	/* These bits 20:18 (burstLen) of control register can be written but will take
	 * effect only if these bits are valid. So this will not affect previous versions
	 * of the DMA. They will continue to have those bits set to 0.
	 */
	control &= ~D64_RC_BL_MASK;
	control |= (rxburstlen << D64_RC_BL_SHIFT);
	control |= (rxoffset << D64_RC_RO_SHIFT);

	reg32_write(&di->d64rxregs->control, control);
	return 0;
}


static void
dma_txinit(dma_info_t *di)
{
	uint32 control;

	if (di->ntxd == 0) {
		return;
	}

	di->txin = di->txout = 0;
	di->hnddma.txavail = di->ntxd - 1;

	/* These bits 20:18 (burstLen) of control register can be written but will take
	 * effect only if these bits are valid. So this will not affect previous versions
	 * of the DMA. They will continue to have those bits set to 0.
	 */
	control = reg32_read(&di->d64txregs->control);

	control |= D64_XC_XE;
	if (di->dmalb) {
	    control |= D64_XC_LE;
	}
	if ((di->hnddma.dmactrlflags & DMA_CTRL_PEN) == 0) {
		control |= D64_XC_PD;
	}

	reg32_write(&di->d64txregs->control, control);

	/* Initailize the DMA channel */
	reg32_write(&di->d64txregs->addrlow, TX_DESC_BASE);
	reg32_write(&di->d64txregs->addrhigh, 0);
}


static void
dma_rxinit(dma_info_t *di)
{
	di->rxin = di->rxout = 0;

	dma_rxenable(di);

	/* The rx descriptor ring should have the addresses set properly */
	/* Set the lastdscr for the rx ring */
	reg32_write(&di->d64rxregs->ptr, ((uint32_t)RX_DESC(RX_BUF_NUM)&D64_XP_LD_MASK));

}


static bool
dma_txreset(dma_info_t *di)
{
	uint32 status;

	if (di->ntxd == 0) {
		return TRUE;
	}

	/* Suspend tx DMA first */
	reg32_write(&di->d64txregs->control, D64_XC_SE);
	SPINWAIT(((status = (reg32_read(&di->d64txregs->status0) & D64_XS0_XS_MASK)) !=
	          D64_XS0_XS_DISABLED) &&
	         (status != D64_XS0_XS_IDLE) &&
	         (status != D64_XS0_XS_STOPPED),
	         10000);

	reg32_write(&di->d64txregs->control, 0);
	SPINWAIT(((status = (reg32_read(&di->d64txregs->status0) & D64_XS0_XS_MASK)) !=
	          D64_XS0_XS_DISABLED),
	         10000);

	/* Wait for the last transaction to complete */
	udelay(300);

	return (status == D64_XS0_XS_DISABLED);
}


static bool
dma_rxreset(dma_info_t *di)
{
	uint32 status;

	if (di->nrxd == 0) {
		return TRUE;
	}

	reg32_write(&di->d64rxregs->control, 0);
	SPINWAIT(((status = (reg32_read(&di->d64rxregs->status0) & D64_RS0_RS_MASK)) !=
	          D64_RS0_RS_DISABLED), 10000);

	return (status == D64_RS0_RS_DISABLED);
}


static int
dma_txload(int index, size_t len, uint8_t * tx_buf)
{
	/* Copy buffer */
	memcpy((uint8_t *)TX_BUF(index), tx_buf, len);

#if (defined(CONFIG_GREYHOUND) || defined(CONFIG_SABER2) || defined(CONFIG_HURRICANE3))
	/* The Ethernet packet has to be >= 64 bytes required by switch 
	* padding it with zeros
	*/
	if (len < 60) {
		memset((uint8_t *)TX_BUF(index) + len, 0, 60 - len);
		len = 60;
	}
#endif

	/* Flush data, config, and descriptors to external memory */
	TX_FLUSH_CACHE();

	return len;
}


/* this api will check if a packet has been received.  If so it will return the
   address of the buffer and enter a bogus address into the descriptor table
   rxin will be incremented to the next descriptor.
   Once done with the frame the buffer should be added back onto the descriptor
   and the lastdscr should be updated to this descriptor. */
static void *
dma_getnextrxp(dma_info_t *di, int *index, size_t *len, uint32_t *stat0, uint32_t *stat1)
{
	dma64dd_t *descp = NULL;
	uint i, curr, active;
	void *rxp;

	/* initialize return parameters */
	*index = 0;
	*len = 0;
	*stat0 = 0;
	*stat1 = 0;

	i = di->rxin;

	curr = B2I(((reg32_read(&di->d64rxregs->status0) & D64_RS0_CD_MASK) -
		di->rcvptrbase) & D64_RS0_CD_MASK, dma64dd_t);
	active = B2I(((reg32_read(&di->d64rxregs->status1) & D64_RS0_CD_MASK) -
		di->rcvptrbase) & D64_RS0_CD_MASK, dma64dd_t);

	ET_TRACE(("rxin(0x%x) curr(0x%x) active(0x%x)\n", i, curr, active));

	/* Check if any frame */
	if (i == curr) {
		return (NULL);
    }
    
	/* Remove warning */
	if (i == active);

	/* Get the packet pointer that corresponds to the rx descriptor */
	rxp = (void*)RX_BUF(i);

	descp = (dma64dd_t *)RX_DESC(i);

	descp->addrlow = 0xdeadbeef;
	descp->addrhigh = 0xdeadbeef;

	*index = i;
	*len = (descp->ctrl2&D64_CTRL2_BC_MASK);
	*stat0 = reg32_read(&di->d64rxregs->status0);
	*stat1 = reg32_read(&di->d64rxregs->status1);

	di->rxin = NEXTRXD(i);

	return (rxp);
}


/* Restore the buffer back on to the descriptor and
   then lastdscr should be updated to this descriptor. */
static void
dma_rxrefill(dma_info_t *di, int index)
{
	dma64dd_t *descp = NULL;
	void *bufp;

	/* Get the packet pointer that corresponds to the rx descriptor */
	bufp = (void*)RX_BUF(index);
	descp = (dma64dd_t *)RX_DESC(index);

	/* Update descriptor that is being added back on ring */
	descp->ctrl2 = cpu_to_le32(RX_BUF_SIZE);
	descp->addrlow = cpu_to_le32((uint32_t)bufp);
	descp->addrhigh = 0;
}

static void
gmac_dump_regs(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;

	printf("GMAC Registers:\n");
	printf("Device control          (0x%.8x): 0x%.8x\n", (uint32_t)&regs->dev_ctl, 
	                                                reg32_read(&regs->dev_ctl));
	printf("Device status           (0x%.8x): 0x%.8x\n", (uint32_t)&regs->dev_status, 
	                                                reg32_read(&regs->dev_status));
	printf("Bist status             (0x%.8x): 0x%.8x\n", (uint32_t)&regs->bist_status, 
	                                                reg32_read(&regs->bist_status));
	printf("Interrupt mask          (0x%.8x): 0x%.8x\n", (uint32_t)&regs->int_mask, 
	                                                reg32_read(&regs->int_mask));
	printf("GP timer                (0x%.8x): 0x%.8x\n", (uint32_t)&regs->gp_timer, 
	                                                reg32_read(&regs->gp_timer));
	printf("Interrupt receive lazy  (0x%.8x): 0x%.8x\n", (uint32_t)&regs->int_recv_lazy, 
	                                                reg32_read(&regs->int_recv_lazy));
	printf("Idle threshold          (0x%.8x): 0x%.8x\n", (uint32_t)&regs->gmac_idle_cnt_thresh, 
	                                                reg32_read(&regs->gmac_idle_cnt_thresh));
	printf("PHY access              (0x%.8x): 0x%.8x\n", (uint32_t)&regs->phy_access, 
	                                                reg32_read(&regs->phy_access));
	printf("PHY control             (0x%.8x): 0x%.8x\n", (uint32_t)&regs->phy_ctl, 
	                                                reg32_read(&regs->phy_ctl));
	printf("GPIO select             (0x%.8x): 0x%.8x\n", (uint32_t)&regs->gpio_select, 
	                                                reg32_read(&regs->gpio_select));
	printf("GPIO output enable      (0x%.8x): 0x%.8x\n", (uint32_t)&regs->gpio_output_en, 
	                                                reg32_read(&regs->gpio_output_en));
	printf("TXQ/RxQ memory control  (0x%.8x): 0x%.8x\n", (uint32_t)&regs->txq_rxq_mem_ctl, 
	                                                reg32_read(&regs->txq_rxq_mem_ctl));
	printf("Memory ECC status       (0x%.8x): 0x%.8x\n", (uint32_t)&regs->memory_ecc_status, 
	                                                reg32_read(&regs->memory_ecc_status));
#if !defined(CONFIG_HURRICANE2)
	printf("SERDES control          (0x%.8x): 0x%.8x\n", (uint32_t)&regs->serdes_ctl, 
	                                                reg32_read(&regs->serdes_ctl));
	printf("SERDES status 0         (0x%.8x): 0x%.8x\n", (uint32_t)&regs->serdes_status0, 
	                                                reg32_read(&regs->serdes_status0));
	printf("SERDES status 1         (0x%.8x): 0x%.8x\n", (uint32_t)&regs->serdes_status1, 
	                                                reg32_read(&regs->serdes_status1));
#endif /* !defined(CONFIG_HURRICANE2) */
	printf("Clock control status    (0x%.8x): 0x%.8x\n", (uint32_t)&regs->clk_ctl_status, 
	                                                reg32_read(&regs->clk_ctl_status));
	printf("Power control           (0x%.8x): 0x%.8x\n", (uint32_t)&regs->pwr_ctl, 
	                                                reg32_read(&regs->pwr_ctl));
	
	printf("UNIMAC Registers:\n");
	printf("Command config          (0x%.8x): 0x%.8x\n", (uint32_t)&regs->cmd_cfg,  
	                                                reg32_read(&regs->cmd_cfg));
	printf("MAC address high        (0x%.8x): 0x%.8x\n", (uint32_t)&regs->mac_addr_high,  
	                                                reg32_read(&regs->mac_addr_high));
	printf("MAC address low         (0x%.8x): 0x%.8x\n", (uint32_t)&regs->mac_addr_low,  
	                                                reg32_read(&regs->mac_addr_low));
	printf("MAC mode                (0x%.8x): 0x%.8x\n", (uint32_t)&regs->mac_mode,  
	                                                reg32_read(&regs->mac_mode));
	printf("VLAN outer tag          (0x%.8x): 0x%.8x\n", (uint32_t)&regs->outer_tag,  
	                                                reg32_read(&regs->outer_tag));
	printf("VLAN inner tag          (0x%.8x): 0x%.8x\n", (uint32_t)&regs->inner_tag,  
	                                                reg32_read(&regs->inner_tag));
#if defined(CONFIG_HURRICANE3)
	printf("Rx packet drop status   (0x%.8x): 0x%.8x\n", (uint32_t)&regs->rx_pkt_drop,  
	                                                reg32_read(&regs->rx_pkt_drop));
	printf("Revision version        (0x%.8x): 0x%.8x\n", (uint32_t)&regs->revision_id,  
	                                                reg32_read(&regs->revision_id));
#endif /* defined(CONFIG_HURRICANE3) */
	printf("Rx FIFO status          (0x%.8x): 0x%.8x\n", (uint32_t)&regs->rx_fifo_status,  
	                                                reg32_read(&regs->rx_fifo_status));
	printf("Tx FIFO status          (0x%.8x): 0x%.8x\n", (uint32_t)&regs->tx_fifo_status,  
	                                                reg32_read(&regs->tx_fifo_status));
}

static void
gmac_dump_txmib(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;

	printf("GMAC TX MIB:\n");
	printf("Tx good octets          (0x%.8x): %d\n", (uint32_t)&regs->mib.tx_good_octets, 
	                                reg32_read(&regs->mib.tx_good_octets));
	printf("Tx good packets         (0x%.8x): %d\n", (uint32_t)&regs->mib.tx_good_pkts, 
	                                reg32_read(&regs->mib.tx_good_pkts));
	printf("Tx octets               (0x%.8x): %d\n", (uint32_t)&regs->mib.tx_octets, 
	                                reg32_read(&regs->mib.tx_octets));
	printf("Tx packets              (0x%.8x): %d\n", (uint32_t)&regs->mib.tx_pkts, 
	                                reg32_read(&regs->mib.tx_pkts));
	printf("Tx broadcast packets    (0x%.8x): %d\n", (uint32_t)&regs->mib.tx_broadcast_pkts, 
	                                reg32_read(&regs->mib.tx_broadcast_pkts));
	printf("Tx multicast packets    (0x%.8x): %d\n", (uint32_t)&regs->mib.tx_multicast_pkts, 
	                                reg32_read(&regs->mib.tx_multicast_pkts));
	printf("Tx jabber packets       (0x%.8x): %d\n", (uint32_t)&regs->mib.tx_jabber_pkts, 
	                                reg32_read(&regs->mib.tx_jabber_pkts));
	printf("Tx oversize packets     (0x%.8x): %d\n", (uint32_t)&regs->mib.tx_oversize_pkts, 
	                                reg32_read(&regs->mib.tx_oversize_pkts));
	printf("Tx fragment packets     (0x%.8x): %d\n", (uint32_t)&regs->mib.tx_fragment_pkts, 
	                                reg32_read(&regs->mib.tx_fragment_pkts));
	printf("Tx underruns            (0x%.8x): %d\n", (uint32_t)&regs->mib.tx_underruns, 
	                                reg32_read(&regs->mib.tx_underruns));
	printf("Tx total collisions     (0x%.8x): %d\n", (uint32_t)&regs->mib.tx_total_cols, 
	                                reg32_read(&regs->mib.tx_total_cols));
	printf("Tx single collision     (0x%.8x): %d\n", (uint32_t)&regs->mib.tx_single_cols, 
	                                reg32_read(&regs->mib.tx_single_cols));
	printf("Tx multiple collisions  (0x%.8x): %d\n", (uint32_t)&regs->mib.tx_multiple_cols, 
	                                reg32_read(&regs->mib.tx_multiple_cols));
	printf("Tx execssive collisions (0x%.8x): %d\n", (uint32_t)&regs->mib.tx_excessive_cols, 
	                                reg32_read(&regs->mib.tx_excessive_cols));
	printf("Tx late collisions      (0x%.8x): %d\n", (uint32_t)&regs->mib.tx_late_cols, 
	                                reg32_read(&regs->mib.tx_late_cols));
	printf("Tx defered packets      (0x%.8x): %d\n", (uint32_t)&regs->mib.tx_defered, 
	                                reg32_read(&regs->mib.tx_defered));
	printf("Tx carrier lost         (0x%.8x): %d\n", (uint32_t)&regs->mib.tx_carrier_lost, 
	                                reg32_read(&regs->mib.tx_carrier_lost));
	printf("Tx pause packets        (0x%.8x): %d\n", (uint32_t)&regs->mib.tx_pause_pkts, 
	                                reg32_read(&regs->mib.tx_pause_pkts));
	printf("Tx unicast packets      (0x%.8x): %d\n", (uint32_t)&regs->mib.tx_uni_pkts, 
	                                reg32_read(&regs->mib.tx_uni_pkts));
	printf("\n");
}


static void
gmac_dump_rxmib(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;

	printf("GMAC RX MIB:\n");
	printf("Rx good octets          (0x%.8x): %d\n", (uint32_t)&regs->mib.rx_good_octets, 
	                                reg32_read(&regs->mib.rx_good_octets));
	printf("Rx good packets         (0x%.8x): %d\n", (uint32_t)&regs->mib.rx_good_pkts, 
	                                reg32_read(&regs->mib.rx_good_pkts));
	printf("Rx octets               (0x%.8x): %d\n", (uint32_t)&regs->mib.rx_octets, 
	                                reg32_read(&regs->mib.rx_octets));
	printf("Rx packets              (0x%.8x): %d\n", (uint32_t)&regs->mib.rx_pkts, 
	                                reg32_read(&regs->mib.rx_pkts));
	printf("Rx broadcast packets    (0x%.8x): %d\n", (uint32_t)&regs->mib.rx_broadcast_pkts, 
	                                reg32_read(&regs->mib.rx_broadcast_pkts));
	printf("Rx multicast packets    (0x%.8x): %d\n", (uint32_t)&regs->mib.rx_multicast_pkts, 
	                                reg32_read(&regs->mib.rx_multicast_pkts));
	printf("Rx jabber packets       (0x%.8x): %d\n", (uint32_t)&regs->mib.rx_jabber_pkts, 
	                                reg32_read(&regs->mib.rx_jabber_pkts));
	printf("Rx oversize packets     (0x%.8x): %d\n", (uint32_t)&regs->mib.rx_oversize_pkts, 
	                                reg32_read(&regs->mib.rx_oversize_pkts));
	printf("Rx fragment packets     (0x%.8x): %d\n", (uint32_t)&regs->mib.rx_fragment_pkts, 
	                                reg32_read(&regs->mib.rx_fragment_pkts));
	printf("Rx missed packets       (0x%.8x): %d\n", (uint32_t)&regs->mib.rx_missed_pkts, 
	                                reg32_read(&regs->mib.rx_missed_pkts));
	printf("Rx CRC alignment errors (0x%.8x): %d\n", (uint32_t)&regs->mib.rx_crc_align_errs, 
	                                reg32_read(&regs->mib.rx_crc_align_errs));
	printf("Rx undersize            (0x%.8x): %d\n", (uint32_t)&regs->mib.rx_undersize, 
	                                reg32_read(&regs->mib.rx_undersize));
	printf("Rx CRC errors           (0x%.8x): %d\n", (uint32_t)&regs->mib.rx_crc_errs, 
	                                reg32_read(&regs->mib.rx_crc_errs));
	printf("Rf alignment errors     (0x%.8x): %d\n", (uint32_t)&regs->mib.rx_align_errs, 
	                                reg32_read(&regs->mib.rx_align_errs));
	printf("Rx symbol errors        (0x%.8x): %d\n", (uint32_t)&regs->mib.rx_symbol_errs, 
	                                reg32_read(&regs->mib.rx_symbol_errs));
	printf("Rx pause packets        (0x%.8x): %d\n", (uint32_t)&regs->mib.rx_pause_pkts, 
	                                reg32_read(&regs->mib.rx_pause_pkts));
	printf("Rx nonpause packets     (0x%.8x): %d\n", (uint32_t)&regs->mib.rx_nonpause_pkts, 
	                                reg32_read(&regs->mib.rx_nonpause_pkts));
	printf("Rx SA changes           (0x%.8x): %d\n", (uint32_t)&regs->mib.rx_sachanges, 
	                                reg32_read(&regs->mib.rx_sachanges));
	printf("Rx unicast packets      (0x%.8x): %d\n", (uint32_t)&regs->mib.rx_uni_pkts, 
	                                reg32_read(&regs->mib.rx_uni_pkts));
	printf("\n");
}

void 
ethHw_dump(int pidx, int type)
{
	bcm_eth_t *eth_data;

    if (pidx >= CONFIG_GMAC_NUM) {
        ET_ERROR(("%s: port index error, %d.\n", __FUNCTION__, pidx));
        return;
    }

    eth_data = &g_eth_data[pidx];

    switch(type) {
        case DUMP_TX_MIB:
            gmac_dump_txmib(eth_data);
            break;
        case DUMP_RX_MIB:
            gmac_dump_rxmib(eth_data);
            break;
        case DUMP_REGS:
            gmac_dump_regs(eth_data);
            break;
        default:
            break;
    }
}    


static int
ethHw_dmaInit(bcm_eth_t *eth_data)
{
	int buflen;
	uint32_t alloc_ptr = IPROC_ETH_MALLOC_BASE;

	/* Allocate rx descriptors buffer */
	buflen = RX_DESC_LEN + 0x10;
	
	rxDescBuf = alloc_ptr;
	alloc_ptr += buflen;
	if (rxDescBuf == 0) {
		ET_ERROR(("%s: Failed to allocate RX Descriptor memory\n", __FUNCTION__));
		return -1;
	}
	rxDescAlignedBuf = rxDescBuf;
	ET_TRACE(("RX Descriptor Buffer: 0x%x; length: 0x%x\n", rxDescBuf, buflen));
	
	/* Check if need to align buffer */
	if (rxDescAlignedBuf & 0x0f) {
		/* Align buffer */
		rxDescAlignedBuf = (rxDescBuf + 0x10) & 0xfffffff0;
	}
	ET_TRACE(("RX Descriptor Buffer (aligned): 0x%x; length: 0x%x\n", rxDescAlignedBuf, RX_DESC_LEN));

	/* Allocate rx data buffer */
	buflen = RX_BUF_LEN;

	rxDataBuf = alloc_ptr;
	alloc_ptr += buflen;
	if (rxDataBuf == 0) {
		ET_ERROR(("%s: Failed to allocate RX Data Buffer memory\n", __FUNCTION__));
		return -1;
	}
	ET_TRACE(("RX Data Buffer: 0x%x; length: 0x%x\n", rxDataBuf, buflen));

	/* Allocate tx descriptors buffer */
	buflen = TX_DESC_LEN + 0x10;

	txDescBuf = alloc_ptr;
	alloc_ptr += buflen;
	if (txDescBuf == 0) {
		ET_ERROR(("%s: Failed to allocate TX Descriptor memory\n", __FUNCTION__));
		return -1;
	}
	txDescAlignedBuf = txDescBuf;
	
	ET_TRACE(("TX Descriptor Buffer: 0x%x; length: 0x%x\n", txDescBuf, buflen));
	/* Check if need to align buffer */
	if (txDescAlignedBuf & 0x0f) {
		/* Align buffer */
		txDescAlignedBuf = (txDescBuf + 0x10) & 0xfffffff0;
	}
	ET_TRACE(("TX Descriptor Buffer (aligned): 0x%x; length: 0x%x\n", txDescAlignedBuf, TX_DESC_LEN));

	/* Allocate tx data buffer */
	buflen = TX_BUF_LEN;

	txDataBuf = alloc_ptr;
	alloc_ptr += buflen;
	if (txDataBuf == 0) {
		ET_ERROR(("%s: Failed to allocate TX Data Buffer memory\n", __FUNCTION__));
		return -1;
	}
	ET_TRACE(("TX Data Buffer: 0x%x; length: 0x%x\n", txDataBuf, buflen));

	return 0;
}

static int
ethHw_dmaRxInit(bcm_eth_t *eth_data)
{
	dma_info_t *dma = eth_data->bcmgmac.di[0];
	uint32_t lastDscr;
	dma64dd_t *descp = NULL;
	uint8_t *bufp;
	int i;

	/* Clear descriptor memory */
	BZERO_SM((void*)RX_DESC_BASE, RX_DESC_LEN);
	/* Clear buffer memory */
	BZERO_SM((void*)RX_BUF_BASE, RX_BUF_LEN);

	/* Initialize RX DMA descriptor table */
	for (i = 0; i < RX_BUF_NUM; i++) {
		uint32_t ctrl;
		bufp = (uint8_t *) RX_BUF(i);
		descp = (dma64dd_t *) RX_DESC(i);
		ctrl = 0;
		
		/* if last descr set endOfTable */
		if (i==RX_BUF_NUM - 1) {
			ctrl = D64_CTRL1_EOT;
		}
		descp->ctrl1 = cpu_to_le32(ctrl);
		descp->ctrl2 = cpu_to_le32(RX_BUF_SIZE);
		descp->addrlow = cpu_to_le32((uint32_t)bufp);
		descp->addrhigh = 0;

		lastDscr = ((uint32_t)(descp) & D64_XP_LD_MASK) + sizeof(dma64dd_t);
	}

	/* Initailize the DMA channel */
	reg32_write(&dma->d64rxregs->addrlow, RX_DESC_BASE);
	reg32_write(&dma->d64rxregs->addrhigh, 0);

	/* Now update the dma last descriptor */
	reg32_write(&dma->d64rxregs->ptr, lastDscr);

	dma->rcvptrbase = RX_DESC_BASE;

	return 0;
}

static int
ethHw_dmaTxInit(bcm_eth_t *eth_data)
{
	dma_info_t *dma = eth_data->bcmgmac.di[0];
	dma64dd_t *descp = NULL;
	uint8_t *bufp;
	int i;

	/* Clear descriptor memory */
	BZERO_SM((void*)TX_DESC_BASE, TX_DESC_LEN);
	/* Clear buffer memory */
	BZERO_SM((void*)TX_BUF_BASE, TX_BUF_LEN);

	/* Initialize TX DMA descriptor table */
	for (i = 0; i < TX_BUF_NUM; i++) {
		uint32_t ctrl;
		bufp = (uint8_t *) TX_BUF(i);
		descp = (dma64dd_t *) TX_DESC(i);
		ctrl = 0;
		
		/* If last descr set endOfTable */
		if (i == (TX_BUF_NUM - 1)) {
			ctrl = D64_CTRL1_EOT;
		}
		descp->ctrl1 = cpu_to_le32(ctrl);
		descp->ctrl2 = 0;
		descp->addrlow = cpu_to_le32((uint32_t)bufp);
		descp->addrhigh = 0;
	}

	/* Initailize the DMA channel */
	reg32_write(&dma->d64txregs->addrlow, TX_DESC_BASE);
	reg32_write(&dma->d64txregs->addrhigh, 0);

	dma->xmtptrbase = TX_DESC_BASE;

	/* Now update the dma last descriptor */
	reg32_write(&dma->d64txregs->ptr, TX_DESC_BASE&D64_XP_LD_MASK);

	return 0;
}


static int
ethHw_dmaAttach(bcm_eth_t *eth_data)
{
	bcmgmac_t *ch = &eth_data->bcmgmac;
	dma_info_t *di = NULL;
	char name[16];

	/* Allocate private info structure */
	if ((di = MALLOC(0, sizeof (dma_info_t))) == NULL) {
		ET_ERROR(("%s: out of memory, malloced %d bytes\n", __FUNCTION__, MALLOCED(0)));
		return -1;
	}

	BZERO_SM(di, sizeof(dma_info_t));

	di->dma64 = 1;
	di->d64txregs = (dma64regs_t *)&eth_data->regs->dma_regs[TX_Q0].dmaxmt;
	di->d64rxregs = (dma64regs_t *)&eth_data->regs->dma_regs[RX_Q0].dmarcv;
	
	/* Default flags: For backwards compatibility both Rx Overflow Continue
	 * and Parity are DISABLED.
	 * supports it.
	 */
	dma_ctrlflags(di, DMA_CTRL_ROC | DMA_CTRL_PEN, 0);

	di->rxburstlen = (reg32_read(&di->d64rxregs->control) & D64_RC_BL_MASK) >> D64_RC_BL_SHIFT;
	di->txburstlen = (reg32_read(&di->d64txregs->control) & D64_XC_BL_MASK) >> D64_XC_BL_SHIFT;
	ET_TRACE(("rx burst len 0x%x\n", di->rxburstlen));
	ET_TRACE(("tx burst len 0x%x\n", di->txburstlen));

	di->ntxd = NTXD;
	di->nrxd = NRXD;

	di->rxbufsize = RX_BUF_SIZE;

	di->nrxpost = NRXBUFPOST;
	di->rxoffset = HWRXOFF;

	sprintf(name, "et%d", eth_data->unit);
	strncpy(di->name, name, MAXNAMEL);

	/* Load dma struc addr */
	ch->di[0] = di;

	return 0;
}

static int
ethHw_dmaDetach(bcm_eth_t *eth_data)
{
    dma_info_t *di = eth_data->bcmgmac.di[0];
    
	if (di) {
		/* Free our private info structure */
		MFREE(0, (void *)di, sizeof(dma_info_t));
	    eth_data->bcmgmac.di[0] = NULL;
	}

	return 0;
}


int
ethHw_dmaTx(int pidx, uint8_t *tx_buf, size_t len)
{
	bcm_eth_t *eth_data;
	dma_info_t *di;
	int txout, ntxd;
	uint32_t flags;
	dma64dd_t *descp = NULL;
	uint8_t *bufp;
	uint32_t ctrl2;
	uint32_t lastDscr = ((uint32_t)(TX_DESC(1)) & D64_XP_LD_MASK);
	size_t buflen;
#ifdef BCMIPROC_ETH_DEBUG
    int bdx;
#endif /* BCMIPROC_ETH_DEBUG */
	 
    if (pidx >= CONFIG_GMAC_NUM) {
        ET_ERROR(("%s: port index error, %d.\n", __FUNCTION__, pidx));
        return -1;
    }

    eth_data = &g_eth_data[pidx];
	di = eth_data->bcmgmac.di[0];
	txout = di->txout;
	ntxd = di->ntxd;

	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __FUNCTION__));

	if (txout != 0 && txout != 1) {
		ET_ERROR(("%s: ERROR - invalid txout 0x%x\n", __FUNCTION__, txout));
		return -1;
	}

#ifdef BCMIPROC_ETH_DEBUG
    gmac_dump_regs(eth_data);
    gmac_dump_txmib(eth_data);
    gmac_dump_rxmib(eth_data);
            
    printf("\nSend packet, length:%d\n", len);
    for(bdx = 0 ; bdx < len; bdx++) {
        printf("%.2x ", tx_buf[bdx]);
        if ((bdx % 8) == 7) {
            printf("\n");
        }
    }
    printf("\n");
#endif /* BCMIPROC_ETH_DEBUG */

	/* Load the buffer */
	buflen = dma_txload(txout, len, tx_buf);
	ctrl2 = (buflen & D64_CTRL2_BC_MASK);

	/* The transmit will only be one frame or set SOF, EOF */
	/* Also set int on completion */
	flags = D64_CTRL1_SOF | D64_CTRL1_IOC | D64_CTRL1_EOF;

	/* Txout points to the descriptor to uset */
	/* If last descriptor then set EOT */
	if (txout == (ntxd - 1)) {
		flags |= D64_CTRL1_EOT;
		lastDscr = ((uint32_t)(TX_DESC(0)) & D64_XP_LD_MASK);
	}

	/* Write the descriptor */
	bufp = (uint8_t *) TX_BUF(txout);
	descp = (dma64dd_t *) TX_DESC(txout);
	descp->addrlow = cpu_to_le32((uint32_t)bufp);
	descp->addrhigh = 0;
	descp->ctrl1 = cpu_to_le32(flags);
	descp->ctrl2 = cpu_to_le32(ctrl2);

	/* Now update the dma last descriptor */
	reg32_write(&di->d64txregs->ptr, lastDscr);

	/* Update txout */
	di->txout = NEXTTXD(txout);

	return 0;
}


int
ethHw_dmaTxWait(int pidx)
{
	bcm_eth_t *eth_data;
	bcmgmac_t *ch;
	uint32_t intstatus;
	int xfrdone = false;
	int i = 0;

    if (pidx >= CONFIG_GMAC_NUM) {
        ET_ERROR(("%s: port index error, %d.\n", __FUNCTION__, pidx));
        return -1;
    }

    eth_data = &g_eth_data[pidx];
	ch = &eth_data->bcmgmac;

	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __FUNCTION__));

	/* Clear stored intstatus */
	ch->intstatus = 0;

#ifdef BCMIPROC_DMA_DEBUG
    dma_txdump(eth_data);
#endif /* BCMIPROC_DMA_DEBUG */

	intstatus = gmac_intevent_get(eth_data, true);
	ET_TRACE(("Interrupt status: 0x%.8x.\n", intstatus));
	if (intstatus & (I_XI0 | I_XI1 | I_XI2 | I_XI3)) {
		xfrdone = true;
	}

	while (!xfrdone) {
		udelay(1000);

		i++;
		if (i > 20) {
			printf("\nbcm iproc ethernet Tx failure! Already retried 20 times\n");
			return -1;
		}
		intstatus = gmac_intevent_get(eth_data, true);
	    ET_TRACE(("Interrupt status: 0x%.8x.\n", intstatus));
		if (intstatus & (I_XI0 | I_XI1 | I_XI2 | I_XI3)) {
			xfrdone = true;
		} else if (intstatus) {
			printf("int(0x%x)", intstatus);
		}
	}

	return 0;
}

int
ethHw_dmaRx(int pidx, uint8_t *buf, size_t *len)
{
	bcm_eth_t *eth_data;
	bcmgmac_t *ch;
    dma_info_t *di;
	void *bufp, *datap;
	int index;
	size_t rcvlen, buflen;
	uint32_t stat0, stat1;
	int rc = 0;
	uint32_t control, offset;
	uint8_t statbuf[HWRXOFF * 2];
#ifdef BCMIPROC_ETH_DEBUG
    int bdx;
#endif /* BCMIPROC_ETH_DEBUG */

    if (pidx >= CONFIG_GMAC_NUM) {
        ET_ERROR(("%s: port index error, %d.\n", __FUNCTION__, pidx));
        return -1;
    }
	
    eth_data = &g_eth_data[pidx];
	ch = &eth_data->bcmgmac;
	di = ch->di[0];
	
	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __FUNCTION__));

#ifdef BCMIPROC_DMA_DEBUG
    dma_rxdump(eth_data);
#endif /* BCMIPROC_DMA_DEBUG */

#ifdef BCMIPROC_ETH_DEBUG
    udelay(1000000);
    gmac_dump_regs(eth_data);
    gmac_dump_txmib(eth_data);
    gmac_dump_rxmib(eth_data);
#endif /* BCMIPROC_ETH_DEBUG */

	bufp = dma_getnextrxp(ch->di[0], &index, &buflen, &stat0, &stat1);
	if (stat1 & 0xf0000000) {
        ET_TRACE(("RX DMA status failed, status 0:%.8x, status 1:%.8x\n", stat0, stat1)); 
	}
	
	if (bufp) {
		control = reg32_read(&di->d64rxregs->control);
		offset = (control&D64_RC_RO_MASK)>>D64_RC_RO_SHIFT;
		rcvlen = ltoh16(*(uint16 *)bufp);

		rcvlen = le16_to_cpu(rcvlen);
		if ((rcvlen == 0) || (rcvlen > RX_BUF_SIZE)) {
			ET_ERROR(("Wrong RX packet size 0x%x drop it\n", rcvlen));
			/* Refill buffre & descriptor */
			dma_rxrefill(ch->di[0], index);
            return rc;
		}
		ET_TRACE(("Received %d bytes\n", rcvlen));
		/* copy status into temp buf (need to copy all data out of buffer) */
		memcpy(statbuf, bufp, offset);
		datap = (void*)((uint32_t)bufp + offset);
		memcpy(buf, datap, rcvlen);

#ifdef BCMIPROC_ETH_DEBUG
        printf("Rx packet, length: %d\n", rcvlen);
        for(bdx = 0 ; bdx < rcvlen; bdx++) {
            printf("%.2x ", buf[bdx]);
            if ((bdx % 8) == 7) {
                printf("\n");
            }
        }
        printf("\n");
#endif /* BCMIPROC_ETH_DEBUG */

        *len = rcvlen;

		/* Refill buffre & descriptor */
		dma_rxrefill(ch->di[0], index);
	} else {
        ET_TRACE(("Received packet failed.\n"));
	    return -1;
	}

	return rc;
}


int
ethHw_dmaDisable(int pidx, int dir)
{
	bcm_eth_t *eth_data;
	bcmgmac_t *ch;

    if (pidx >= CONFIG_GMAC_NUM) {
        ET_ERROR(("%s: port index error, %d.\n", __FUNCTION__, pidx));
        return -1;
    }

    eth_data = &g_eth_data[pidx];
    ch = &eth_data->bcmgmac;
    
	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __FUNCTION__));

	if (dir == DMA_TX) {
		return dma_txreset(ch->di[0]);
	} else {
		return dma_rxreset(ch->di[0]);
    }
    
    return 0;
}

int
ethHw_dmaEnable(int pidx, int dir)
{
	bcm_eth_t *eth_data;
	bcmgmac_t *ch;

    if (pidx >= CONFIG_GMAC_NUM) {
        ET_ERROR(("%s: port index error, %d.\n", __FUNCTION__, pidx));
        return -1;
    }

    eth_data = &g_eth_data[pidx];
    ch = &eth_data->bcmgmac;

	ET_TRACE(("et%d: %s enter, dir:%s.\n", eth_data->unit, __FUNCTION__, (dir == DMA_TX) ? "Tx" : "Rx"));

	if (dir == DMA_TX) {
		dma_txinit(ch->di[0]);
	} else {
		dma_rxinit(ch->di[0]);
    }

    return 0;
}


int
ethHw_dmaLoopbackSet(int pidx, int en)
{
	bcm_eth_t *eth_data;
    dma_info_t *di;

    if (pidx >= CONFIG_GMAC_NUM) {
        ET_ERROR(("%s: port index error, %d.\n", __FUNCTION__, pidx));
        return -1;
    }
    
    eth_data = &g_eth_data[pidx];
    di = eth_data->bcmgmac.di[0];

	di->dmalb = false;
	if (en) {
	    di->dmalb = true;
    }
	
	return 0;
}


static int
ethHw_chipAttach(bcm_eth_t *eth_data)
{
	bcmgmac_t *ch = &eth_data->bcmgmac;
	int stat;
	int chipid;
	char name[16];
	char *strptr;
	
	BZERO_SM((char *)ch, sizeof(eth_data->bcmgmac));

	/* Get our phyaddr value */
#if (defined(CONFIG_GREYHOUND) || defined(CONFIG_HURRICANE3))
    ch->phyaddr = CONFIG_EXTERNAL_PHY_DEV_ID;
#else
	ch->phyaddr = eth_data->mac + 1;
#endif
	
	/* Get our phyaddr value */
	sprintf(name, "et%dphyaddr", eth_data->mac);
	if ((strptr = getenv (name)) != NULL) {
		ch->phyaddr = simple_strtoul(strptr, &strptr, 10) & 0x1f;
	}
	else
	{
		ch->phyaddr = VSC8211_ADDR;
	}

	stat = ethHw_dmaAttach(eth_data);
	if (stat) {
		ET_ERROR(("et%d: ethHw_dmaAttach failed\n", eth_data->unit));
		return -1;
	}

	/* Reset the gmac core */
	chip_reset(eth_data);

	ethHw_dmaTxInit(eth_data);
	ethHw_dmaRxInit(eth_data);

	/* Set default sofware intmask */
	ch->def_intmask = DEF_INTMASK;
	ch->intmask = ch->def_intmask;

	/* Reset phy: reset it once now */
	chipid = iproc_get_chipid();
	printf("et%d: %s: Chip ID: 0x%x; phyaddr: 0x%x\n", eth_data->unit, __FUNCTION__, chipid, eth_data->bcmgmac.phyaddr);

#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
	/* Flip switch so AMAC can access serdes */
	gmac_mdio_set(1);
	gmac_serdes_init(eth_data);
	phy5461_init(ch->phyaddr);
    serdes_reset(ch->phyaddr);
	
	/* Reset core from lane 0 */
	serdes_reset_core(1);
	serdes_init(ch->phyaddr);
	/* Start pll from lane 0 */
	serdes_start_pll(1);
#elif defined(CONFIG_SABER2)
	/* Flip switch so AMAC can access serdes */
	gmac_mdio_set(1);
ch->phyaddr &= 0xf;
	gmac_serdes_init(eth_data);
	phy5461_init(ch->phyaddr);
	//serdes_init(2);
#elif defined(CONFIG_HURRICANE2)
	phy5221_init(ch->phyaddr);
	phy5221_enable_set(ch->phyaddr, 1);
#elif (defined(CONFIG_GREYHOUND) || defined(CONFIG_HURRICANE3))
    phy5481_init(ch->phyaddr);
#endif 

	return 0;
}

static void
ethHw_chipDetach(bcm_eth_t *eth_data)
{
	ethHw_dmaDetach(eth_data);
}

int
ethHw_macEnableSet(int pidx, int en)
{
	bcm_eth_t *eth_data;
	gmacregs_t *regs;

    if (pidx >= CONFIG_GMAC_NUM) {
        ET_ERROR(("%s: port index error, %d.\n", __FUNCTION__, pidx));
        return -1;
    }

    eth_data = &g_eth_data[pidx];
    regs = eth_data->regs;

	gmac_enable(eth_data, en);

	if (en) {
		/* Clear interrupts */
		reg32_write(&regs->int_status, I_INTMASK);
	}

    return ETHHW_RC_NONE;
}


int
ethHw_macEnableGet(int pidx, int *txp, int *rxp)
{
	bcm_eth_t *eth_data;
	gmacregs_t *regs;
	uint32_t cmdcfg;

    if (pidx >= CONFIG_GMAC_NUM) {
        ET_ERROR(("%s: port index error, %d.\n", __FUNCTION__, pidx));
        return -1;
    }

    eth_data = &g_eth_data[pidx];
    regs = eth_data->regs;

	/* Read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);
	*txp = ((cmdcfg | CC_TE) ? 1 : 0);
	*rxp = ((cmdcfg | CC_RE) ? 1 : 0);

	return ETHHW_RC_NONE;
}


int
ethHw_macLoopbackSet(int pidx, int en)
{
	bcm_eth_t *eth_data;
	gmacregs_t *regs;
	uint32_t ocmdcfg, cmdcfg;

    if (pidx >= CONFIG_GMAC_NUM) {
        ET_ERROR(("%s: port index error, %d.\n", __FUNCTION__, pidx));
        return -1;
    }
    
    eth_data = &g_eth_data[pidx];
    regs = eth_data->regs;
    
    ET_TRACE(("et%d: %s enter\n", eth_data->unit, __FUNCTION__));

	/* Read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);
	ocmdcfg = cmdcfg;

	/* Set/clear the mac loopback mode */
    cmdcfg &= ~CC_ML;
	eth_data->maclb = false;
	if (en) {
		cmdcfg |= CC_ML;
	    eth_data->maclb = true;
    }

	/* Check if need to write register back */
	if (cmdcfg != ocmdcfg) {
		/* Put mac in reset */
		gmac_init_reset(eth_data);
		/* Write register */
		reg32_write(&regs->cmd_cfg, cmdcfg);
		/* Bring mac out of reset */
		gmac_clear_reset(eth_data);
	}
	
	return 0;
}

int
ethHw_Init(int pidx)
{
	bcm_eth_t *eth_data;
#ifndef CONFIG_IPROC_EMULATION
	char envname[32];
#endif /* CONFIG_IPROC_EMULATION */

    if (pidx >= CONFIG_GMAC_NUM) {
        ET_ERROR(("%s: port index error, %d.\n", __FUNCTION__, pidx));
        return -1;
    }
    
    eth_data = &g_eth_data[pidx];
	memset(eth_data, 0, sizeof(bcm_eth_t));

	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __FUNCTION__));

	/* load mac number  */
	eth_data->unit = pidx;
	eth_data->mac = pidx;

	/* Configure egister base address */
	switch (eth_data->mac) {
		case (ETHHW_MAC_0):
			eth_data->regs = (gmacregs_t *)IPROC_GMAC0_REG_BASE;
			break;
#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_SABER2))
        case (ETHHW_MAC_1):
			eth_data->regs = (gmacregs_t *)IPROC_GMAC1_REG_BASE;
			break;
#endif /* (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_SABER2)) */
		default:
			ET_ERROR(("et%d: Invalid GMAC specified\n", eth_data->unit));
	}

	printf("et%d: Initial GMAC %d, base address:0x%.0x\n", 
	            eth_data->unit, eth_data->mac, (unsigned int)eth_data->regs);

#ifdef CONFIG_IPROC_EMULATION
    /* Assign the MAC address */
    eth_data->enetaddr[0] = (unsigned char)0x00;
    eth_data->enetaddr[1] = (unsigned char)0x01;
    eth_data->enetaddr[2] = (unsigned char)0x02;
    eth_data->enetaddr[3] = (unsigned char)0x03;
    eth_data->enetaddr[4] = (unsigned char)0x04;
    eth_data->enetaddr[5] = (unsigned char)(0x00 + eth_data->mac);
#else
    /* Assign the MAC address */
    memset(envname, 0, 32);
    if (eth_data->mac == 0) {
        sprintf(envname, "ethaddr");
    } else {
        sprintf(envname, "eth%daddr", eth_data->mac);
    }
        
	if (getenv(envname)) {
		if (!eth_getenv_enetaddr(envname, eth_data->enetaddr)) {
			ET_ERROR(("et%d: get the ethaddr from environment failed.\n", eth_data->unit));
		}
	} else {
	    ET_ERROR(("et%d: no ethaddr in environment.\n", eth_data->unit));
	}
#endif /* CONFIG_IPROC_EMULATION */

	if (ethHw_dmaInit(eth_data) < 0) {
	    ET_ERROR(("et%d: dma initial failed.\n", eth_data->unit));
		return -1;
	}

	/* Reset cores */
	gmac_core_reset();

	if (ethHw_chipAttach(eth_data) < 0) {
	    ET_ERROR(("et%d: chip attach failed.\n", eth_data->unit));
		return -1;
    }
    
	chip_init(eth_data, ET_INIT_FULL);

	return ETHHW_RC_NONE;
}

int
ethHw_Exit(void)
{
	ET_TRACE(("%s enter\n", __FUNCTION__));

	/* Free rx descriptors buffer */
	if (rxDescBuf) {
		MFREE(0, (void*)rxDescBuf, 0);
		rxDescBuf = 0;
		rxDescAlignedBuf = 0;
	}

	/* Allocate rx data buffer */
	if (rxDataBuf) {
		MFREE(0, (void*)rxDataBuf, 0);
		rxDataBuf = 0;
	}

	/* Free tx descriptors buffer */
	if (txDescBuf) {
		MFREE(0, (void*)txDescBuf, 0);
		txDescBuf = 0;
		txDescAlignedBuf = 0;
	}

	/* Allocate tx data buffer */
	if (txDataBuf) {
		MFREE(0, (void*)txDataBuf, 0);
		txDataBuf = 0;
	}

	return ETHHW_RC_NONE;
}


int
ethHw_miiphy_read(uint32_t const phyaddr, uint32_t const reg, uint16_t *const value)
{
	uint16 tmp16 = 0;
#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_SABER2))
	uint32 addr, ext, bank, flags;

	addr = phyaddr & 0xf;
	ext = phyaddr & 0xf0;
	bank = (phyaddr & 0x1f00) >> 8;
	flags = (phyaddr & 0x10000) ? SOC_PHY_REG_1000X : 0;

	if (!ext) { /* Internal serdes */
		tmp16 = serdes_rd_reg(addr, reg);
	} else {    /* External phy */
		phy5461_rd_reg(addr, flags, bank, reg, &tmp16);
	}
	printf("phyaddr(0x%x) ext(0x%x) bank(0x%x) flags(0x%x) reg(0x%x) data(0x%x)\n",
			 addr, ext, bank, flags, reg, tmp16);
#elif defined(CONFIG_HURRICANE2)
	uint32 addr, bank;

	addr = phyaddr & 0xf;
	bank = (phyaddr & 0x1f00) >> 8;

	phy5221_rd_reg(addr, bank, reg, &tmp16);
	printf("phyaddr(0x%x) bank(0x%x) reg(0x%x) data(0x%x)\n", addr, bank, reg, tmp16);
#elif (defined(CONFIG_GREYHOUND) || defined(CONFIG_HURRICANE3))
    uint32 addr, bank, flags;

    addr = phyaddr & 0x1f;
    bank = (phyaddr & 0x1f00) >> 8;
    flags = (phyaddr & 0x10000) ? SOC_PHY_REG_1000X : 0;

    phy5481_rd_reg(addr, flags, bank, reg, &tmp16);
    printf("phyaddr(0x%x) bank(0x%x) reg(0x%x) data(0x%x)\n", addr, bank, reg, tmp16);
#endif

	*value = tmp16;
	return 0;
}


int
ethHw_miiphy_write(uint32_t const phyaddr, uint32_t const reg, uint16_t *const value)
{
	uint16 tmp16 = *value;
#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_SABER2))
	uint32 addr, ext, bank, flags;

	addr = phyaddr & 0xf;
	ext = phyaddr & 0xf0;
	bank = (phyaddr & 0x1f00) >> 8;
	flags = (phyaddr & 0x10000) ? SOC_PHY_REG_1000X : 0;

	printf("phyaddr(0x%x) ext(0x%x) bank(0x%x) flags(0x%x) reg(0x%x) data(0x%x)\n",
			 addr, ext, bank, flags, reg, *value);
	if (!ext) { /* Internal serdes */
		serdes_wr_reg(addr, reg, tmp16);
	} else {    /* External phy */
		phy5461_wr_reg(addr, flags, bank, reg, &tmp16);
	}
#elif defined(CONFIG_HURRICANE2)
	uint32 addr, bank;

	addr = phyaddr & 0xf;
	bank = (phyaddr & 0x1f00) >> 8;

	printf("phyaddr(0x%x) bank(0x%x) reg(0x%x) data(0x%x)\n", addr, bank, reg, *value);
	phy5221_wr_reg(addr, bank, reg, &tmp16);
#elif (defined(CONFIG_GREYHOUND) || defined(CONFIG_HURRICANE3))
    uint32 addr, bank, flags;

    addr = phyaddr & 0x1f;
    bank = (phyaddr & 0x1f00) >> 8;
    flags = (phyaddr & 0x10000) ? SOC_PHY_REG_1000X : 0;

    printf("phyaddr(0x%x) bank(0x%x) reg(0x%x) data(0x%x)\n", addr, bank, reg, *value);
    phy5481_wr_reg(addr, flags, bank, reg, &tmp16);

#endif
	return 0;
}

int
ethHw_portLinkUp(int pidx)
{
	int link = 0;
	bcm_eth_t *eth_data;
	bcmgmac_t *ch;

    if (pidx >= CONFIG_GMAC_NUM) {
        ET_ERROR(("%s: port index error, %d.\n", __FUNCTION__, pidx));
        return -1;
    }

    eth_data = &g_eth_data[pidx];
    ch = &eth_data->bcmgmac;

#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_SABER2))
	phy5461_link_get(ch->phyaddr, &link);
#elif defined(CONFIG_HURRICANE2)
	phy5221_link_get(ch->phyaddr, &link);
#elif (defined(CONFIG_GREYHOUND) || defined(CONFIG_HURRICANE3))
	phy5481_link_get(ch->phyaddr, &link);
#endif 

#ifdef CONFIG_IPROC_EMULATION
    link = 1;
#endif

	return link;
}


void
ethHw_portSpeedCheck(int pidx)
{
	bcm_eth_t *eth_data;
	bcmgmac_t *ch;
	int speed = 0, duplex = 0, speedcfg;
	static int orgspd, orgdpx;

    if (pidx >= CONFIG_GMAC_NUM) {
        ET_ERROR(("%s: port index error, %d.\n", __FUNCTION__, pidx));
        return -1;
    }

    eth_data = &g_eth_data[pidx];
    ch = &eth_data->bcmgmac;

#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_SABER2))
	phy5461_speed_get(ch->phyaddr, &speed, &duplex);
#elif defined(CONFIG_HURRICANE2)
	phy5221_speed_get(ch->phyaddr, &speed, &duplex);
#elif (defined(CONFIG_GREYHOUND) || defined(CONFIG_HURRICANE3))
	phy5481_speed_get(ch->phyaddr, &speed, &duplex);
#endif 

#ifdef CONFIG_IPROC_EMULATION
    speed = 1000;
    duplex = 1;
#endif
#if 0
printf("get speed =%d,%d\n",speed, duplex);
speed = 100;
duplex = 1;
#endif
	if (speed) {
		if (speed == 1000) {
			if (duplex) {
				speedcfg = ET_1000FULL;
			} else {
				speedcfg = ET_1000HALF;
			}
		} else if (speed == 100) {
			if (duplex) {
				speedcfg = ET_100FULL;
			} else {
				speedcfg = ET_100HALF;
			}
		} else if (speed == 10) {
			if (duplex) {
				speedcfg = ET_10FULL;
			} else {
				speedcfg = ET_10HALF;
			}
		} else {
			printf("ERROR: unknown speed %d\n", speed);
			return;
		}
		gmac_speed(eth_data, speedcfg);
	}
	if ( orgspd != speed || orgdpx != duplex ) {
		printf("ETH LINK UP: %d%s\n", speed, duplex ? "FD" : "HD");
		orgspd = speed;
		orgdpx = duplex;
	}
}


#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
void 
ethHw_serdesEarlyInit(int pidx)
{
    bcm_eth_t eth_data;

    gmac_mdio_set(1);

	/* Initialize GMAC0 serdes control */
    if (pidx == 0) {
        eth_data.regs = (gmacregs_t *)IPROC_GMAC0_REG_BASE;
    }
#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_SABER2))
    else if (pidx == 1) {
        eth_data.regs = (gmacregs_t *)IPROC_GMAC1_REG_BASE;
    }
#endif /* (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_SABER2)) */
    else {
        ET_ERROR(("%s: port index error, %d.\n", __FUNCTION__, pidx));
    }

    eth_data.unit = pidx;

    /* Reset cores */
    gmac_core_reset();

    gmac_serdes_init(&eth_data);
}
#endif /* (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2)) */


#if (defined(CONFIG_GREYHOUND) || defined(CONFIG_HURRICANE3))
/* Implement the PHY read/write via CMICD */
#define CMICD_BASE_ADDRESS		0x3200000
#define CMC2_OFFSET			    0x33000

#define MIIM_PARAM_REG 			CMIC_CMC2_MIIM_PARAM_BASE
#define MIIM_PARAM__MIIM_CYCLE_SHIFT    CMIC_CMC2_MIIM_PARAM__MIIM_CYCLE_R
#define MIIM_PARAM__MIIM_CYCLE_MASK     ((1 << CMIC_CMC2_MIIM_PARAM__MIIM_CYCLE_WIDTH) - 1)
#define MIIM_PARAM__INTERNAL_SEL_SHIFT  CMIC_CMC2_MIIM_PARAM__INTERNAL_SEL
#define MIIM_PARAM__INTERNAL_SEL_MASK   ((1 << CMIC_CMC2_MIIM_PARAM__INTERNAL_SEL_WIDTH) - 1)
#define MIIM_PARAM__BUS_ID_SHIFT        CMIC_CMC2_MIIM_PARAM__BUS_ID_R
#define MIIM_PARAM__BUS_ID_MASK         ((1 << CMIC_CMC2_MIIM_PARAM__BUS_ID_WIDTH) - 1)
#define MIIM_PARAM__C45_SEL_SHIFT       CMIC_CMC2_MIIM_PARAM__C45_SEL
#define MIIM_PARAM__C45_SEL_MASK        ((1 << CMIC_CMC2_MIIM_PARAM__C45_SEL_WIDTH) - 1)
#define MIIM_PARAM__PHY_ID_SHIFT        CMIC_CMC2_MIIM_PARAM__PHY_ID_R
#define MIIM_PARAM__PHY_ID_MASK         ((1 << CMIC_CMC2_MIIM_PARAM__PHY_ID_WIDTH) - 1)
#define MIIM_PARAM__PHY_DATA_SHIFT      CMIC_CMC2_MIIM_PARAM__PHY_DATA_R
#define MIIM_PARAM__PHY_DATA_MASK       ((1 << CMIC_CMC2_MIIM_PARAM__PHY_DATA_WIDTH) - 1)

#define MIIM_READ_DATA_REG              CMIC_CMC2_MIIM_READ_DATA_BASE
#define MIIM_READ_DATA__DATA_SHIFT      CMIC_CMC2_MIIM_READ_DATA__DATA_R
#define MIIM_READ_DATA__DATA_MASK       ((1 << CMIC_CMC2_MIIM_READ_DATA__DATA_WIDTH) - 1)

#define MIIM_ADDRESS_REG                        CMIC_CMC2_MIIM_ADDRESS_BASE
#define MIIM_ADDRESS__CLAUSE_45_DTYPE_SHIFT     CMIC_CMC2_MIIM_ADDRESS__CLAUSE_45_DTYPE_R
#define MIIM_ADDRESS__CLAUSE_45_DTYPE_MASK      ((1 << CMIC_CMC2_MIIM_ADDRESS__CLAUSE_45_DTYPE_WIDTH) - 1)
#define MIIM_ADDRESS__CLAUSE_45_REGADR_SHIFT    CMIC_CMC2_MIIM_ADDRESS__CLAUSE_45_REGADR_R
#define MIIM_ADDRESS__CLAUSE_45_REGADR_MASK     ((1 << CMIC_CMC2_MIIM_ADDRESS__CLAUSE_45_REGADR_WIDTH) - 1)
#define MIIM_ADDRESS__CLAUSE_22_REGADR_SHIFT    CMIC_CMC2_MIIM_ADDRESS__CLAUSE_22_REGADR_R
#define MIIM_ADDRESS__CLAUSE_22_REGADR_MASK     ((1 << CMIC_CMC2_MIIM_ADDRESS__CLAUSE_22_REGADR_WIDTH) - 1)

#define MIIM_CTRL_REG                   CMIC_CMC2_MIIM_CTRL_BASE
#define MIIM_CTRL__MIIM_RD_START_SHIFT  CMIC_CMC2_MIIM_CTRL__MIIM_RD_START
#define MIIM_CTRL__MIIM_RD_START_MASK   ((1 << CMIC_CMC2_MIIM_CTRL__MIIM_RD_START_WIDTH) - 1)
#define MIIM_CTRL__MIIM_WR_START_SHIFT  CMIC_CMC2_MIIM_CTRL__MIIM_WR_START
#define MIIM_CTRL__MIIM_WR_START_MASK   ((1 << CMIC_CMC2_MIIM_CTRL__MIIM_WR_START_WIDTH) - 1)

#define MIIM_STAT_REG                   CMIC_CMC2_MIIM_STAT_BASE
#define MIIM_STAT__MIIM_OPN_DONE_SHIFT  CMIC_CMC2_MIIM_STAT__MIIM_OPN_DONE
#define MIIM_STAT__MIIM_OPN_DONE_MASK   ((1 << CMIC_CMC2_MIIM_STAT__MIIM_OPN_DONE_WIDTH) - 1)

#define SET_REG_FIELD(reg_value, fshift, fmask, fvalue)	\
            (reg_value) = ((reg_value) & ~((fmask) << (fshift))) |  \
            (((fvalue) & (fmask)) << (fshift))
#define ISET_REG_FIELD(reg_value, fshift, fmask, fvalue) \
            (reg_value) = (reg_value) | (((fvalue) & (fmask)) << (fshift))			
#define GET_REG_FIELD(reg_value, fshift, fmask)	\
            (((reg_value) & ((fmask) << (fshift))) >> (fshift))

#define MIIM_OP_MAX_HALT_USEC	500	

enum {
    MIIM_OP_MODE_READ,
    MIIM_OP_MODE_WRITE,
    MIIM_OP_MODE_MAX	
};

struct cmicd_miim_cmd {
    int bus_id;
    int int_sel;
    int phy_id;
    int regnum;
    int c45_sel;
    uint16_t op_mode;
    uint16_t val;
};

static inline uint32_t 
cmicd_miim_reg_read(uint32_t reg)
{
    uint32_t addr;
    volatile uint32_t value;
    
    addr = (CMICD_BASE_ADDRESS + CMC2_OFFSET) + reg;
    value = reg32_read((volatile uint32_t *)addr);
    
    return  (uint32_t)value;
}

static inline void 
cmicd_miim_reg_write(uint32_t reg, uint32_t data)
{
    uint32_t addr;
    
    addr = (CMICD_BASE_ADDRESS + CMC2_OFFSET) + reg;
    reg32_write((volatile uint32_t *)addr, data);
}

static inline void 
cmicd_miim_set_op_read(uint32_t *data, uint32_t set)
{
	SET_REG_FIELD(*data, MIIM_CTRL__MIIM_RD_START_SHIFT, 
			MIIM_CTRL__MIIM_RD_START_MASK, set);
}

static inline void 
cmicd_miim_set_op_write(uint32_t *data, uint32_t set)
{
	SET_REG_FIELD(*data, MIIM_CTRL__MIIM_WR_START_SHIFT, 
			MIIM_CTRL__MIIM_WR_START_MASK, set);
}

static inline int 
do_cmicd_miim_op(uint32_t op, uint32_t param, uint32_t addr, uint16_t *reg_val)
{
	uint32_t val, op_done;
	int ret = 0;
	int usec = MIIM_OP_MAX_HALT_USEC;

	if (op >= MIIM_OP_MODE_MAX) {
        ET_ERROR(("%s : invalid op code %d\n",__FUNCTION__,op));
		return SOC_E_INIT;
	}

	cmicd_miim_reg_write(MIIM_PARAM_REG, param);
	cmicd_miim_reg_write(MIIM_ADDRESS_REG, addr);
	val = cmicd_miim_reg_read(MIIM_CTRL_REG);
	if(op == MIIM_OP_MODE_READ) {
		cmicd_miim_set_op_read(&val, 1);
	} else {
		cmicd_miim_set_op_write(&val, 1);
	}
	cmicd_miim_reg_write(MIIM_CTRL_REG, val);

	do {
		op_done = GET_REG_FIELD(cmicd_miim_reg_read(MIIM_STAT_REG),
				MIIM_STAT__MIIM_OPN_DONE_SHIFT, MIIM_STAT__MIIM_OPN_DONE_MASK);
		if (op_done) {
			break;
		}
		
		udelay(1);
		usec--;
	} while (usec > 0);

	if (op_done) {
		if(op == MIIM_OP_MODE_READ) {
			*reg_val = cmicd_miim_reg_read(MIIM_READ_DATA_REG);
		}
	} else {
		ret = SOC_E_TIMEOUT;
	}

	val = cmicd_miim_reg_read(MIIM_CTRL_REG);
	if(op == MIIM_OP_MODE_READ) {
		cmicd_miim_set_op_read(&val, 0);
	} else {
		cmicd_miim_set_op_write(&val, 0);
	}
	cmicd_miim_reg_write(MIIM_CTRL_REG, val);

	return ret;
}


static int 
cmicd_miim_op(struct cmicd_miim_cmd *cmd)
{
	uint32_t miim_param =0, miim_addr = 0;
    int rv = 0;
	
	ISET_REG_FIELD(miim_param, MIIM_PARAM__BUS_ID_SHIFT, 
			MIIM_PARAM__BUS_ID_MASK, cmd->bus_id);

	if (cmd->int_sel) {
		ISET_REG_FIELD(miim_param, MIIM_PARAM__INTERNAL_SEL_SHIFT, 
				MIIM_PARAM__INTERNAL_SEL_MASK, 1);
    }
    
	ISET_REG_FIELD(miim_param, MIIM_PARAM__PHY_ID_SHIFT, 
			MIIM_PARAM__PHY_ID_MASK, cmd->phy_id);

	if (cmd->op_mode == MIIM_OP_MODE_WRITE) {
		ISET_REG_FIELD(miim_param, MIIM_PARAM__PHY_DATA_SHIFT, 
				MIIM_PARAM__PHY_DATA_MASK, cmd->val);
    }
    
	if (cmd->c45_sel) {
		ISET_REG_FIELD(miim_param, MIIM_PARAM__C45_SEL_SHIFT, 
				MIIM_PARAM__C45_SEL_MASK, 1);

		ISET_REG_FIELD(miim_addr, MIIM_ADDRESS__CLAUSE_45_REGADR_SHIFT, 
				MIIM_ADDRESS__CLAUSE_45_REGADR_MASK, cmd->regnum);
		ISET_REG_FIELD(miim_addr, MIIM_ADDRESS__CLAUSE_45_DTYPE_SHIFT, 
				MIIM_ADDRESS__CLAUSE_45_REGADR_MASK, cmd->regnum >> 16);
	} else {
		ISET_REG_FIELD(miim_addr, MIIM_ADDRESS__CLAUSE_22_REGADR_SHIFT, 
				MIIM_ADDRESS__CLAUSE_22_REGADR_MASK, cmd->regnum);
	}

    rv = do_cmicd_miim_op(cmd->op_mode, miim_param, miim_addr, &cmd->val);

    return rv;
}


void
chip_phy_wr(uint ext, uint phyaddr, uint reg, uint16_t v)
{
	struct cmicd_miim_cmd cmd = {0};
    int rv;

	ASSERT(phyaddr < MAXEPHY);
	ASSERT(reg < MAXPHYREG);
    
	cmd.bus_id = CONFIG_EXTERNAL_PHY_BUS_ID;
	if (!ext) {
		cmd.int_sel = 1;
    }
	cmd.phy_id = phyaddr;
	cmd.regnum = reg;
	cmd.val = v;

	cmd.op_mode = MIIM_OP_MODE_WRITE;

    rv = cmicd_miim_op(&cmd); 
    if (rv < 0) {
        ET_ERROR(("phyaddr %x: PHY register write is failed! error code is %d\n", phyaddr, rv)); 
    }
	
	return;
}


uint16_t
chip_phy_rd(uint ext, uint phyaddr, uint reg)
{
	struct cmicd_miim_cmd cmd = {0};
    int rv;

    ASSERT(phyaddr < MAXEPHY);
	ASSERT(reg < MAXPHYREG);
    
	cmd.bus_id = CONFIG_EXTERNAL_PHY_BUS_ID;
	if (!ext) {
		cmd.int_sel = 1;
    }
	cmd.phy_id = phyaddr;
	cmd.regnum = reg; 

	cmd.op_mode = MIIM_OP_MODE_READ;

    rv = cmicd_miim_op(&cmd); 
    if (rv < 0) {
        ET_ERROR(("phyaddr %x: PHY register read is failed! error code is %d\n", phyaddr, rv)); 
    }
	return cmd.val;
    
}

#else /* !CONFIG_GREYHOUND */

void
chip_phy_wr(uint ext, uint phyaddr, uint reg, uint16_t v)
{
	uint32_t tmp;
	volatile uint32_t *phy_ctrl = (uint32_t *)ChipcommonB_MII_Management_Control;
	volatile uint32_t *phy_data = (uint32_t *)ChipcommonB_MII_Management_Command_Data;

	ASSERT(phyaddr < MAXEPHY);
	ASSERT(reg < MAXPHYREG);

	/* Wait until Mii mgt interface not busy */
    SPINWAIT((reg32_read(phy_ctrl) & (1 << ChipcommonB_MII_Management_Control__BSY)), 1000);
	tmp = reg32_read(phy_ctrl);
//printf("phy_ctrl=0x%x\n",tmp);
	if (tmp & (1 << ChipcommonB_MII_Management_Control__BSY)) {
		ET_ERROR(("phyaddr %x: busy\n", phyaddr));
	}

	/* Set preamble and MDCDIV */
	tmp = (1 << ChipcommonB_MII_Management_Control__PRE) | 0x1a;													/* MDCDIV */
	if (ext) {  /* Ext phy */
		tmp |= (1 << ChipcommonB_MII_Management_Control__EXT);
	}  else {
		tmp &= ~(1 << ChipcommonB_MII_Management_Control__EXT);
	}
	reg32_write(phy_ctrl, tmp);
//printf("after write phy_ctrl=0x%x\n",tmp);

	/* Wait for it to complete */
    SPINWAIT((reg32_read(phy_ctrl) & (1 << ChipcommonB_MII_Management_Control__BSY)), 1000);
	tmp = reg32_read(phy_ctrl);
//printf("11phy_ctrl=0x%x\n",tmp);
	if (tmp & (1 << ChipcommonB_MII_Management_Control__BSY)) {
		ET_ERROR(("phyaddr %x: ChipcommonB_MII_Management_Control did not complete\n", phyaddr));
	}

	/* Issue the write */
	/* Set start bit, write op, phy addr, phy reg & data */
    tmp = ((1 << ChipcommonB_MII_Management_Command_Data__SB_R) |       /* SB */
           (1 << ChipcommonB_MII_Management_Command_Data__OP_R) |       /* OP - wrt */
           (phyaddr << ChipcommonB_MII_Management_Command_Data__PA_R) | /* PA */
           (reg << ChipcommonB_MII_Management_Command_Data__RA_R) |		/* RA */
           (2 << ChipcommonB_MII_Management_Command_Data__TA_R) |       /* TA */
	        v);                                                         /* Data */
//printf("phy_data=0x%x\n",tmp);
	reg32_write(phy_data, tmp);

	/* Wait for it to complete */
    SPINWAIT((reg32_read(phy_ctrl) & (1 << ChipcommonB_MII_Management_Control__BSY)), 1000);
	tmp = reg32_read(phy_ctrl);
	if (tmp & (1 << ChipcommonB_MII_Management_Control__BSY)) {
		ET_ERROR(("phyaddr %x: ChipcommonB_MII_Management_Command_Data did not complete\n", phyaddr));
	}
}


uint16_t
chip_phy_rd(uint ext, uint phyaddr, uint reg)
{
	uint32_t tmp;
	volatile uint32_t *phy_ctrl = (uint32_t *)ChipcommonB_MII_Management_Control;
	volatile uint32_t *phy_data = (uint32_t *)ChipcommonB_MII_Management_Command_Data;

	ASSERT(phyaddr < MAXEPHY);
	ASSERT(reg < MAXPHYREG);

	/* Wait until Mii mgt interface not busy */
    SPINWAIT((reg32_read(phy_ctrl) & (1 << ChipcommonB_MII_Management_Control__BSY)), 1000);
	tmp = reg32_read(phy_ctrl);
	if (tmp & (1 << ChipcommonB_MII_Management_Control__BSY)) {
		ET_ERROR(("phyaddr %x: busy\n", phyaddr));
	}

	/* Set preamble and MDCDIV */
	tmp = (1 << ChipcommonB_MII_Management_Control__PRE) | 0x1a;													/* MDCDIV */
	if (ext) {  /* ext phy */
		tmp |= (1 << ChipcommonB_MII_Management_Control__EXT);
	}  else {
		tmp &= ~(1 << ChipcommonB_MII_Management_Control__EXT);
	}
	reg32_write(phy_ctrl, tmp);

	/* Wait for it to complete */
    SPINWAIT((reg32_read(phy_ctrl) & (1 << ChipcommonB_MII_Management_Control__BSY)), 1000);
	tmp = reg32_read(phy_ctrl);
	if (tmp & (1 << ChipcommonB_MII_Management_Control__BSY)) {
		ET_ERROR(("phyaddr %x: ChipcommonB_MII_Management_Control did not complete\n", phyaddr));
	}

	/* Issue the read */
	/* Set start bit, write op, phy addr, phy reg & data */
	tmp = 	(1 << ChipcommonB_MII_Management_Command_Data__SB_R) |			/* SB */
			(2 << ChipcommonB_MII_Management_Command_Data__OP_R) |			/* OP - rd*/
			(phyaddr << ChipcommonB_MII_Management_Command_Data__PA_R) |	/* PA */
			(reg << ChipcommonB_MII_Management_Command_Data__RA_R) |		/* RA */
			(2 << ChipcommonB_MII_Management_Command_Data__TA_R);			/* TA */
	reg32_write(phy_data, tmp);

	/* Wait for it to complete */
    SPINWAIT((reg32_read(phy_ctrl) & (1 << ChipcommonB_MII_Management_Control__BSY)), 1000);
	tmp = reg32_read(phy_ctrl);
	if (tmp & (1 << ChipcommonB_MII_Management_Control__BSY)) {
		ET_ERROR(("phyaddr %x: ChipcommonB_MII_Management_Command_Data did not complete\n", phyaddr));
	}

	/* Read data */
	tmp = reg32_read(phy_data);

	return (tmp & 0xffff);
}

#endif /* CONFIG_GREYHOUND */

/*
 * Initialize all the chip registers.  If dma mode, init tx and rx dma engines
 * but leave the devcontrol tx and rx (fifos) disabled.
 */
static void
chip_init(bcm_eth_t *eth_data, uint options)
{
	gmacregs_t *regs = eth_data->regs;
	bcmgmac_t *ch = &eth_data->bcmgmac;
	int speed = ET_1000FULL;
    uint32_t *ptr;
    
	/* Enable one rx interrupt per received frame */
	reg32_write(&regs->int_recv_lazy, (1 << IRL_FC_SHIFT));

	/* Enable 802.3x tx flow control (honor received PAUSE frames) */
	gmac_tx_flowcontrol(eth_data, TRUE);

	/* Disable promiscuous mode */
	gmac_promisc(eth_data, TRUE);

	/* Set our local address */
	ET_TRACE(("\nMAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
			eth_data->enetaddr[0], eth_data->enetaddr[1], eth_data->enetaddr[2],
			eth_data->enetaddr[3], eth_data->enetaddr[4], eth_data->enetaddr[5]));
	
	ptr = (uint32_t *)&eth_data->enetaddr[0];
	reg32_write(&regs->mac_addr_high, htonl(*ptr));
	ptr = (uint32_t *)&eth_data->enetaddr[4];
	reg32_write(&regs->mac_addr_low,  htons(*ptr));

	/* Optionally enable mac-level loopback */
	ethHw_macLoopbackSet(eth_data->mac, false);

	/* Set max frame lengths - account for possible vlan tag */
	reg32_write(&regs->rx_max_length, PKTSIZE + 32);

	/* Set phy speed/duplex */
#if defined(CONFIG_HURRICANE2)
	speed = ET_100FULL;
#endif
	gmac_speed(eth_data, speed);

	/* Enable the overflow continue feature and disable parity */
	dma_ctrlflags(ch->di[0], DMA_CTRL_ROC | DMA_CTRL_PEN ,DMA_CTRL_ROC);
}

static void
chip_reset(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;
	bcmgmac_t *ch = &eth_data->bcmgmac;
	int speed = ET_1000FULL;

	/* Reset the tx dma engines */
	dma_txreset(ch->di[TX_Q0]);

	/* Set eth_data into loopback mode to ensure no rx traffic */
	ethHw_macLoopbackSet(eth_data->mac, true);
	udelay(1);

	/* Reset the rx dma engine */
	dma_rxreset(ch->di[RX_Q0]);

	/* Reset gmac */
	gmac_reset(eth_data);

	/* Clear mib */
	gmac_clearmib(eth_data);

	/* Set smi_master to drive mdc_clk */
	reg32_set_bits(&eth_data->regs->phy_ctl, PC_MTE);

	/* Set gmac speed */
#if defined(CONFIG_HURRICANE2)
	speed = ET_100FULL;
#endif
	gmac_speed(eth_data, speed);

	/* Clear persistent sw intstatus */
	reg32_write(&regs->int_status, 0);
}
