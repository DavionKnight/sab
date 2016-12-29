/***************************************************************************
 * Copyright (C) 1999-2013, Broadcom Corporation
 * 
 *      Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2 (the "GPL"),
 * available at http://www.broadcom.com/licenses/GPLv2.php, with the
 * following added to such license:
 * 
 *      As a special exception, the copyright holders of this software give you
 * permission to link this software with independent modules, and to copy and
 * distribute the resulting executable under terms of your choice, provided that
 * you also meet, for each linked independent module, the terms and conditions of
 * the license of that module.  An independent module is a module which is not
 * derived from this software.  The special exception does not apply to any
 * modifications of the software.
 * 
 *      Notwithstanding the above, under no circumstances may you combine this
 * software in any way with any other Broadcom software provided under a license
 * other than the GPL, without Broadcom's express prior written consent.
 *
 ***************************************************************************/
 
#define GLOBAL_REG_RBUS_START   0x0000000000000000LL
#define uint32                  unsigned long

uint32 REGRD (uint32 address) ;
uint32 REGWR (uint32 address, uint32 data) ;

#define DDR_PHY_REG_READ(_unit, _pc, flags, _reg_addr, _val) \
            *(uint32 *)_val = REGRD((_pc) + (_reg_addr))
/*            soc_ddr40_phy_reg_ci_read((_unit), (_pc), (_reg_addr), (_val)) */
#define DDR_PHY_REG_WRITE(_unit, _pc, _flags, _reg_addr, _val) \
            REGWR((_pc) + (_reg_addr), (_val))
/*            soc_ddr40_phy_reg_ci_write((_unit), (_pc), (_reg_addr), (_val)) */
#define DDR_PHY_REG_MODIFY(_unit, _pc, _flags, _reg_addr, _val, _mask) \
            REGWR((_pc) + (_reg_addr), (REGRD((_pc) + (_reg_addr)) & ~(_mask)) | ((_val) & (_mask)))
/*            soc_ddr40_phy_reg_ci_modify((_unit), (_pc), (_reg_addr), (_val), (_mask)) */
#define DDR_PHY_GET_FIELD(m,c,r,f) \
            GET_FIELD(m,c,r,f)
#define DDR_PHY_SET_FIELD(m,c,r,f,d) \
            SET_FIELD(m,c,r,f,d)
