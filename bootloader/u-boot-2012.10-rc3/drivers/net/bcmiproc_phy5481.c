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

/* ---- Include Files ---------------------------------------------------- */
#include "bcmiproc_phy.h"
#include "bcmiproc_phy5481.h"

/* ---- External Variable Declarations ----------------------------------- */
/* ---- External Function Prototypes ------------------------------------- */
/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */
/* ---- Private Variables ------------------------------------------------ */

/* debug/trace */
//#define BCMDBG
#define BCMDBG_ERR
#ifdef BCMDBG
#define	NET_ERROR(args) printf args
#define	NET_TRACE(args) printf args
#elif defined(BCMDBG_ERR)
#define	NET_ERROR(args) printf args
#define NET_TRACE(args)
#else
#define	NET_ERROR(args)
#define	NET_TRACE(args)
#endif /* BCMDBG */
#define	NET_REG_TRACE(args)


#ifndef ASSERT
#define ASSERT(exp)
#endif

#define	PHY_EXTERNAL	1

extern void chip_phy_wr(uint ext, uint phyaddr, uint reg, uint16_t v);
extern uint16_t chip_phy_rd(uint ext, uint phyaddr, uint reg);


/* ==== Public Functions ================================================= */

int
phy5481_wr_reg(uint phyaddr, uint32 flags, uint16 reg_bank, uint8 reg_addr, uint16 *data)
{
	int     rv = SOC_E_NONE;
	uint16  wr_data=*data;

    NET_TRACE(("phyaddr %d: %s enter\n", phyaddr, __FUNCTION__));

	NET_REG_TRACE(("%s going to write phyaddr(0x%x) flags(0x%x) reg_bank(0x%x) reg_addr(0x%x) data(0x%x)\n",
		 __FUNCTION__, phyaddr, flags, reg_bank, reg_addr, wr_data));

    if (flags & SOC_PHY_REG_1000X) {
        if (reg_addr <= 0x000f) {
            uint16 blk_sel;

            /* Map 1000X page */
			chip_phy_wr(PHY_EXTERNAL, phyaddr, 0x1c, 0x7c00);
			blk_sel = chip_phy_rd(PHY_EXTERNAL, phyaddr, 0x1c);
			chip_phy_wr(PHY_EXTERNAL, phyaddr, 0x1c, blk_sel | 0x8001);

            /* write 1000X IEEE register */
			chip_phy_wr(PHY_EXTERNAL, phyaddr, reg_addr, wr_data);

           /* Restore IEEE mapping */
			chip_phy_wr(PHY_EXTERNAL, phyaddr, 0x1c, (blk_sel & 0xfffe) | 0x8000);
        } else if (flags & _SOC_PHY_REG_DIRECT) {
			chip_phy_wr(PHY_EXTERNAL, phyaddr, reg_addr, wr_data);
        } else {
            rv = SOC_E_PARAM;
        }
    } else {
        switch(reg_addr) {
        /* Map shadow registers */
        case 0x18:
            if (reg_bank <= 0x0007) {
                if (reg_bank == 0x0007) {
                    wr_data |= 0x8000;
                }
                wr_data = (wr_data & ~(0x0007)) | reg_bank;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0x1C:
            if (reg_bank <= 0x001F) {
                wr_data = 0x8000 | (reg_bank << 10) | (wr_data & 0x03FF);
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        default:
            if (!(flags & SOC_PHY_REG_RESERVE_ACCESS)) {
                /* Must not write to reserved registers */ 
                if (reg_addr > 0x001e) {
                    rv = SOC_E_PARAM;
                }
            }
            break;
        }
        if (SOC_SUCCESS(rv)) {
			chip_phy_wr(PHY_EXTERNAL, phyaddr, reg_addr, wr_data);
        }
    } 
    if (SOC_FAILURE(rv)) {
		NET_ERROR(("%s ERROR phyaddr(0x%x) reg_bank(0x%x) reg_addr(0x%x) rv(%d)\n",
				 __FUNCTION__, phyaddr, reg_bank, reg_addr, rv));
    }
    return rv;
}


int
phy5481_rd_reg(uint phyaddr, uint32 flags, uint16 reg_bank, uint8 reg_addr, uint16 *data)
{
	int     rv = SOC_E_NONE;

	NET_TRACE(("phyaddr %d: %s enter\n", phyaddr, __FUNCTION__));

	NET_REG_TRACE(("%s going to read phyaddr(0x%x) flags(0x%x) reg_bank(0x%x) reg_addr(0x%x)\n",
			 __FUNCTION__, phyaddr, flags, reg_bank, reg_addr));
    if (flags & SOC_PHY_REG_1000X) {
        if (reg_addr <= 0x000f) {
            uint16 blk_sel;

            /* Map 1000X page */
			chip_phy_wr(PHY_EXTERNAL, phyaddr, 0x1c, 0x7c00);
			blk_sel = chip_phy_rd(PHY_EXTERNAL, phyaddr, 0x1c);
			chip_phy_wr(PHY_EXTERNAL, phyaddr, 0x1c, blk_sel | 0x8001);

            /* Read 1000X IEEE register */
			*data = chip_phy_rd(PHY_EXTERNAL, phyaddr, reg_addr);
			NET_REG_TRACE(("%s rd phyaddr(0x%x) flags(0x%x) reg_bank(0x%x) reg_addr(0x%x) data(0x%x)\n",
					 __FUNCTION__, phyaddr, flags, reg_bank, reg_addr, *data));

           /* Restore IEEE mapping */
			chip_phy_wr(PHY_EXTERNAL, phyaddr, 0x1c, (blk_sel & 0xfffe) | 0x8000);
        } else {
            rv = SOC_E_PARAM;
        }
    } else {
        switch(reg_addr) {
        /* Map shadow registers */
        case 0x18:
            if (reg_bank <= 0x0007) {
				chip_phy_wr(PHY_EXTERNAL, phyaddr, reg_addr, (reg_bank << 12) | 0x7);
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0x1C:
            if (reg_bank <= 0x001F) {
				chip_phy_wr(PHY_EXTERNAL, phyaddr, reg_addr, (reg_bank << 10));
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        default:
            if (!(flags & SOC_PHY_REG_RESERVE_ACCESS)) {
                /* Must not read from reserved registers */ 
                if (reg_addr > 0x001e) {
                   rv = SOC_E_PARAM;
                }
            }
            break;
        }
        if (SOC_SUCCESS(rv)) {
			*data = chip_phy_rd(PHY_EXTERNAL, phyaddr, reg_addr);
			NET_REG_TRACE(("%s rd phyaddr(0x%x) flags(0x%x) reg_bank(0x%x) reg_addr(0x%x) data(0x%x)\n",
					 __FUNCTION__, phyaddr, flags, reg_bank, reg_addr, *data));
        }
    } 
    if (SOC_FAILURE(rv)) {
		NET_ERROR(("%s ERROR phyaddr(0x%x) reg_bank(0x%x) reg_addr(0x%x) rv(%d)\n",
				 __FUNCTION__, phyaddr, reg_bank, reg_addr, rv));
    }
	
    return rv;
}


int
phy5481_mod_reg(uint phyaddr, uint32 flags, uint16 reg_bank,
			uint8 reg_addr, uint16 data, uint16 mask)
{
	int     rv = SOC_E_NONE;
	uint16  rd_data;

	NET_TRACE(("phyaddr %d: %s enter\n", phyaddr, __FUNCTION__));

	NET_REG_TRACE(("%s going to modify phyaddr(0x%x) flags(0x%x) reg_bank(0x%x) reg_addr(0x%x) data(0x%x) mask(0x%x)\n",
		 __FUNCTION__, phyaddr, flags, reg_bank, reg_addr, data, mask));

    if (flags & SOC_PHY_REG_1000X) {
        if (reg_addr <= 0x000f) {
            uint16 blk_sel;

            /* Map 1000X page */
			chip_phy_wr(PHY_EXTERNAL, phyaddr, 0x1c, 0x7c00);
			blk_sel = chip_phy_rd(PHY_EXTERNAL, phyaddr, 0x1c);
			chip_phy_wr(PHY_EXTERNAL, phyaddr, 0x1c, blk_sel | 0x8001);

            /* Modify 1000X IEEE register */
			rd_data = chip_phy_rd(PHY_EXTERNAL, phyaddr, reg_addr);
			NET_REG_TRACE(("%s rd phyaddr(0x%x) reg_bank(0x%x) reg_addr(0x%x) data(0x%x)\n",
					 __FUNCTION__, phyaddr, reg_bank, reg_addr, rd_data));
			rd_data &= ~(mask);
			rd_data |= data;
			chip_phy_wr(PHY_EXTERNAL, phyaddr, reg_addr, rd_data);
			NET_REG_TRACE(("%s wrt phyaddr(0x%x) reg_bank(0x%x) reg_addr(0x%x) data(0x%x)\n",
					 __FUNCTION__, phyaddr, reg_bank, reg_addr, rd_data));

           /* Restore IEEE mapping */
			chip_phy_wr(PHY_EXTERNAL, phyaddr, 0x1c, (blk_sel & 0xfffe) | 0x8000);
        } else {
            rv = SOC_E_PARAM;
        }
    } else {
        switch(reg_addr) {
        /* Map shadow registers */
        case 0x18:
            if (reg_bank <= 0x0007) {
				chip_phy_wr(PHY_EXTERNAL, phyaddr, reg_addr, (reg_bank << 12) | 0x7);

                if (reg_bank == 0x0007) {
                    data |= 0x8000;
                    mask |= 0x8000;
                }
                mask &= ~(0x0007);
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0x1C:
            if (reg_bank <= 0x001F) {
				chip_phy_wr(PHY_EXTERNAL, phyaddr, reg_addr, (reg_bank << 10));
                data |= 0x8000;
                mask |= 0x8000;
                mask &= ~(0x1F << 10);
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        default:
            if (!(flags & SOC_PHY_REG_RESERVE_ACCESS)) {
                /* Must not write to reserved registers */ 
                if (reg_addr > 0x001e) {
                    rv = SOC_E_PARAM;
                }
            }
            break;
        }
        if (SOC_SUCCESS(rv)) {
			rd_data = chip_phy_rd(PHY_EXTERNAL, phyaddr, reg_addr);
			NET_REG_TRACE(("%s rd phyaddr(0x%x) reg_bank(0x%x) reg_addr(0x%x) data(0x%x)\n",
					 __FUNCTION__, phyaddr, reg_bank, reg_addr, rd_data));
			rd_data &= ~(mask);
			rd_data |= data;
			chip_phy_wr(PHY_EXTERNAL, phyaddr, reg_addr, rd_data);
			NET_REG_TRACE(("%s wrt phyaddr(0x%x) reg_bank(0x%x) reg_addr(0x%x) data(0x%x)\n",
					 __FUNCTION__, phyaddr, reg_bank, reg_addr, rd_data));
        }
    } 

    if (SOC_FAILURE(rv)) {
		NET_ERROR(("%s ERROR phyaddr(0x%x) reg_bank(0x%x) reg_addr(0x%x) rv(%d)\n",
				 __FUNCTION__, phyaddr, reg_bank, reg_addr, rv));
    }
    return rv;
}


void
phy5481_ge_reset(uint phyaddr)
{
	uint16 ctrl;
	unsigned long init_time;

	NET_TRACE(("phyaddr %d: %s enter\n", phyaddr, __FUNCTION__));

	/* set reset flag */
	phy5481_rd_reg(phyaddr, PHY_MII_CTRLr_FLAGS, PHY_MII_CTRLr_BANK, PHY_MII_CTRLr_ADDR, &ctrl);
	ctrl |= MII_CTRL_RESET;
	phy5481_wr_reg(phyaddr, PHY_MII_CTRLr_FLAGS, PHY_MII_CTRLr_BANK, PHY_MII_CTRLr_ADDR, &ctrl);
	init_time = get_timer(0);
    for (;;) {

		udelay(100);

		/* check if out of reset */
		if (!(phy5481_rd_reg(phyaddr, PHY_MII_CTRLr_FLAGS, PHY_MII_CTRLr_BANK, PHY_MII_CTRLr_ADDR, &ctrl) & MII_CTRL_RESET)) {
			NET_TRACE(("phyaddr %d: %s reset complete\n", phyaddr, __FUNCTION__));
			return;
		}

		if (get_timer(init_time) > 10) {
			/* timeout */
			NET_ERROR(("phyaddr %d: %s reset not complete\n", phyaddr, __FUNCTION__));
			return;
		}
	}
}


/*
 * Function:
 *  phy5481_ge_init
 * Purpose: 
 *  Initialize the PHY (MII mode) to a known good state.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 * Returns: 
 *  SOC_E_XXX

 * Notes: 
 *  No synchronization performed at this level.
 */
int
phy5481_ge_init(uint phyaddr)
{
    uint16          mii_ctrl, mii_gb_ctrl;
    uint16          mii_ana;

    /* Reset PHY */
	phy5481_ge_reset(phyaddr);

	/* set advertized bits */
	phy5481_rd_reg(phyaddr, PHY_MII_ANAr_FLAGS, PHY_MII_ANAr_BANK, PHY_MII_ANAr_ADDR, &mii_ana);
	mii_ana |= MII_ANA_FD_100 | MII_ANA_FD_10;
	mii_ana |= MII_ANA_HD_100 | MII_ANA_HD_10;
	phy5481_wr_reg(phyaddr, PHY_MII_ANAr_FLAGS, PHY_MII_ANAr_BANK, PHY_MII_ANAr_ADDR, &mii_ana);

    mii_ctrl = MII_CTRL_FD | MII_CTRL_SS_1000 | MII_CTRL_AE | MII_CTRL_RAN;
    mii_gb_ctrl = MII_GB_CTRL_ADV_1000FD | MII_GB_CTRL_PT;

	phy5481_wr_reg(phyaddr, PHY_MII_GB_CTRLr_FLAGS, PHY_MII_GB_CTRLr_BANK, PHY_MII_GB_CTRLr_ADDR, &mii_gb_ctrl);
	phy5481_wr_reg(phyaddr, PHY_MII_CTRLr_FLAGS, PHY_MII_CTRLr_BANK, PHY_MII_CTRLr_ADDR, &mii_ctrl);

    return(SOC_E_NONE);
}




void
phy5481_reset_setup(uint phyaddr)
{
	NET_TRACE(("phyaddr %d: %s enter\n", phyaddr, __FUNCTION__));

    phy5481_ge_init(phyaddr);

    /* remove power down */
	phy5481_mod_reg(phyaddr, PHY_MII_CTRLr_FLAGS, PHY_MII_CTRLr_BANK, PHY_MII_CTRLr_ADDR, 0, MII_CTRL_PD);
    /* Disable super-isolate */
	phy5481_mod_reg(phyaddr, PHY_MII_POWER_CTRLr_FLAGS, PHY_MII_POWER_CTRLr_BANK, PHY_MII_POWER_CTRLr_ADDR, 0, 1U<<5);
    /* Enable extended packet length */
	phy5481_mod_reg(phyaddr, PHY_MII_AUX_CTRLr_FLAGS, PHY_MII_AUX_CTRLr_BANK, PHY_MII_AUX_CTRLr_ADDR, 0x4000, 0x4000);

    return;
}

/*
 * Function:
 *      phy5481_init
 * Purpose:
 *      Initialize xgxs6 phys
 * Parameters:
 *      phyaddr - physical address
 * Returns:
 *      0
 */
int
phy5481_init(uint phyaddr)
{
	uint16	phyid0, phyid1;

	NET_TRACE(("phyaddr %d: %s enter\n", phyaddr, __FUNCTION__));

    phyid0 = 0;
	phy5481_rd_reg(phyaddr, 
        PHY_MII_PHY_ID0r_FLAGS, PHY_MII_PHY_ID0r_BANK, 
        PHY_MII_PHY_ID0r_ADDR, &phyid0);
	phy5481_rd_reg(phyaddr, PHY_MII_PHY_ID1r_FLAGS, PHY_MII_PHY_ID1r_BANK, PHY_MII_PHY_ID1r_ADDR, &phyid1);

	phy5481_reset_setup(phyaddr);

	return 0;
}


/*
 * Function:    
 *  phy5481_link_get
 * Purpose: 
 *  Determine the current link up/down status
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  link - (OUT) Boolean, true indicates link established.
 * Returns:
 *  SOC_E_XXX
 * Notes: 
 *  No synchronization performed at this level.
 */
int
phy5481_link_get(uint phyaddr, int *link)
{
	uint16        mii_ctrl, mii_stat;
	unsigned long init_time;

    *link = FALSE;      /* Default */

	phy5481_rd_reg(phyaddr, PHY_MII_STATr_FLAGS, PHY_MII_STATr_BANK, PHY_MII_STATr_ADDR, &mii_stat);
	/* the first read of status register will not show link up, second read will show link up */
    if (!(mii_stat & MII_STAT_LA) ) {
		phy5481_rd_reg(phyaddr, PHY_MII_STATr_FLAGS, PHY_MII_STATr_BANK, PHY_MII_STATr_ADDR, &mii_stat);
	}

    if (!(mii_stat & MII_STAT_LA) || (mii_stat == 0xffff)) {
    /* mii_stat == 0xffff check is to handle removable PHY daughter cards */
        return SOC_E_NONE;
    }

    /* Link appears to be up; we are done if autoneg is off. */

	phy5481_rd_reg(phyaddr, PHY_MII_CTRLr_FLAGS, PHY_MII_CTRLr_BANK, PHY_MII_CTRLr_ADDR, &mii_ctrl);

    if (!(mii_ctrl & MII_CTRL_AE)) {
		*link = TRUE;
		return SOC_E_NONE;
    }

    /*
     * If link appears to be up but autonegotiation is still in
     * progress, wait for it to complete.  For BCM5228, autoneg can
     * still be busy up to about 200 usec after link is indicated.  Also
     * continue to check link state in case it goes back down.
     */
	init_time = get_timer(0);
    for (;;) {

		phy5481_rd_reg(phyaddr, PHY_MII_STATr_FLAGS, PHY_MII_STATr_BANK, PHY_MII_STATr_ADDR, &mii_stat);

	    if (!(mii_stat & MII_STAT_LA)) {
			/* link is down */
	        return SOC_E_NONE;
	    }

	    if (mii_stat & MII_STAT_AN_DONE) {
			/* AutoNegotiation done */
	        break;
	    }

		if(get_timer(init_time) > 1) {
			/* timeout */
	        return SOC_E_BUSY;
		}
    }

    /* Return link state at end of polling */
    *link = ((mii_stat & MII_STAT_LA) != 0);

    return SOC_E_NONE;
}




/*
 * Function:     
 *    phy5481_auto_negotiate_gcd (greatest common denominator).
 * Purpose:    
 *    Determine the current greatest common denominator between 
 *    two ends of a link
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *    speed - (OUT) greatest common speed.
 *    duplex - (OUT) greatest common duplex.
 *    link - (OUT) Boolean, true indicates link established.
 * Returns:    
 *    SOC_E_XXX
 * Notes: 
 *    No synchronization performed at this level.
 */
static int
phy5481_auto_negotiate_gcd(uint phyaddr, int *speed, int *duplex)
{
    int        t_speed, t_duplex;
    uint16     mii_ana, mii_anp, mii_stat;
    uint16     mii_gb_stat, mii_esr, mii_gb_ctrl;

    mii_gb_stat = 0;            /* Start off 0 */
    mii_gb_ctrl = 0;            /* Start off 0 */

	phy5481_rd_reg(phyaddr, PHY_MII_ANAr_FLAGS, PHY_MII_ANAr_BANK, PHY_MII_ANAr_ADDR, &mii_ana);
	phy5481_rd_reg(phyaddr, PHY_MII_ANPr_FLAGS, PHY_MII_ANPr_BANK, PHY_MII_ANPr_ADDR, &mii_anp);
	phy5481_rd_reg(phyaddr, PHY_MII_STATr_FLAGS, PHY_MII_STATr_BANK, PHY_MII_STATr_ADDR, &mii_stat);

    if (mii_stat & MII_STAT_ES) {    /* Supports extended status */
        /*
         * If the PHY supports extended status, check if it is 1000MB
         * capable.  If it is, check the 1000Base status register to see
         * if 1000MB negotiated.
         */
		phy5481_rd_reg(phyaddr, PHY_MII_ESRr_FLAGS, PHY_MII_ESRr_BANK, PHY_MII_ESRr_ADDR, &mii_esr);

        if (mii_esr & (MII_ESR_1000_X_FD | MII_ESR_1000_X_HD | 
                       MII_ESR_1000_T_FD | MII_ESR_1000_T_HD)) {
			phy5481_rd_reg(phyaddr, PHY_MII_GB_STATr_FLAGS, PHY_MII_GB_STATr_BANK, PHY_MII_GB_STATr_ADDR, &mii_gb_stat);
			phy5481_rd_reg(phyaddr, PHY_MII_GB_CTRLr_FLAGS, PHY_MII_GB_CTRLr_BANK, PHY_MII_GB_CTRLr_ADDR, &mii_gb_ctrl);
        }
    }

    /*
     * At this point, if we did not see Gig status, one of mii_gb_stat or 
     * mii_gb_ctrl will be 0. This will cause the first 2 cases below to 
     * fail and fall into the default 10/100 cases.
     */

    mii_ana &= mii_anp;

    if ((mii_gb_ctrl & MII_GB_CTRL_ADV_1000FD) &&
        (mii_gb_stat & MII_GB_STAT_LP_1000FD)) {
        t_speed  = 1000;
        t_duplex = 1;
    } else if ((mii_gb_ctrl & MII_GB_CTRL_ADV_1000HD) &&
               (mii_gb_stat & MII_GB_STAT_LP_1000HD)) {
        t_speed  = 1000;
        t_duplex = 0;
    } else if (mii_ana & MII_ANA_FD_100) {         /* [a] */
        t_speed = 100;
        t_duplex = 1;
    } else if (mii_ana & MII_ANA_T4) {            /* [b] */
        t_speed = 100;
        t_duplex = 0;
    } else if (mii_ana & MII_ANA_HD_100) {        /* [c] */
        t_speed = 100;
        t_duplex = 0;
    } else if (mii_ana & MII_ANA_FD_10) {        /* [d] */
        t_speed = 10;
        t_duplex = 1 ;
    } else if (mii_ana & MII_ANA_HD_10) {        /* [e] */
        t_speed = 10;
        t_duplex = 0;
    } else {
        return(SOC_E_FAIL);
    }

    if (speed)  *speed  = t_speed;
    if (duplex)    *duplex = t_duplex;

    return(SOC_E_NONE);
}

/*
 * Function:     
 *    phy5481_auto_negotiate_ew (Autoneg-ed mode with E@W on).
 * Purpose:    
 *    Determine autoneg-ed mode between
 *    two ends of a link
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *    speed - (OUT) greatest common speed.
 *    duplex - (OUT) greatest common duplex.
 *    link - (OUT) Boolean, true indicates link established.
 * Returns:    
 *    SOC_E_XXX
 * Notes: 
 *    No synchronization performed at this level.
 */
static int
phy5481_auto_negotiate_ew(uint phyaddr, int *speed, int *duplex)
{
    int        t_speed, t_duplex;
    uint16     mii_assr;

    phy5481_rd_reg(phyaddr, 0x00, 0x0000, 0x19, &mii_assr);

    switch ((mii_assr >> 8) & 0x7) {
    case 0x7:
        t_speed = 1000;
        t_duplex = TRUE;
        break;
    case 0x6:
        t_speed = 1000;
        t_duplex = FALSE;
        break;
    case 0x5:
        t_speed = 100;
        t_duplex = TRUE;
        break;
    case 0x3:
        t_speed = 100;
        t_duplex = FALSE;
        break;
    case 0x2:
        t_speed = 10;
        t_duplex = TRUE;
        break;
    case 0x1:
        t_speed = 10;
        t_duplex = FALSE;
        break;
    default:
        t_speed = 0; /* 0x4 is 100BASE-T4 which is not supported */
        t_duplex = FALSE;
        break;
    }

    if (speed)  *speed  = t_speed;
    if (duplex)    *duplex = t_duplex;

    return(SOC_E_NONE);
}


/*
 * Function:
 *      phy5481_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      phyaddr - physical address
 *      speed - current link speed in Mbps
 * Returns:
 *      0
 */
int
phy5481_speed_get(uint phyaddr, int *speed, int *duplex)
{
    int     rv;
    uint16  mii_ctrl, mii_stat, misc_ctrl;

	NET_TRACE(("phyaddr %d: %s enter\n", phyaddr, __FUNCTION__));

	phy5481_rd_reg(phyaddr, PHY_MII_CTRLr_FLAGS, PHY_MII_CTRLr_BANK, PHY_MII_CTRLr_ADDR, &mii_ctrl);
	phy5481_rd_reg(phyaddr, PHY_MII_STATr_FLAGS, PHY_MII_STATr_BANK, PHY_MII_STATr_ADDR, &mii_stat);

    *speed = 0;
    *duplex = 0;
    if (mii_ctrl & MII_CTRL_AE) {   /* Auto-negotiation enabled */
        if (!(mii_stat & MII_STAT_AN_DONE)) { /* Auto-neg NOT complete */
            rv = SOC_E_NONE;
        } else {
            /* First check for Ethernet@Wirespeed */
            phy5481_rd_reg(phyaddr, 0x0, 0x0007, 0x18, &misc_ctrl);
            if (misc_ctrl & (1U << 4)) {   /* Ethernet@Wirespeed enabled */
                rv = phy5481_auto_negotiate_ew(phyaddr, speed, duplex);
            } else {
                rv = phy5481_auto_negotiate_gcd(phyaddr, speed, duplex);
            }
		}
    } else {                /* Auto-negotiation disabled */
	    /*
	     * Simply pick up the values we force in CTRL register.
	     */
		if (mii_ctrl & MII_CTRL_FD)
			*duplex = 1;

	    switch(MII_CTRL_SS(mii_ctrl)) {
	    case MII_CTRL_SS_10:
	        *speed = 10;
	        break;
	    case MII_CTRL_SS_100:
	        *speed = 100;
	        break;
	    case MII_CTRL_SS_1000:
	        *speed = 1000;
	        break;
	    default:            /* Just pass error back */
	        return(SOC_E_UNAVAIL);
	    }
    	rv = SOC_E_NONE;
    }

    return(rv);
}
