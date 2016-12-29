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
 
#include <common.h>
#include "phy_reg_access.h"

uint32 REGRD (uint32 address) {

  volatile unsigned long data;
  
  data = cpu_to_le32(* (volatile uint32 *) ( ((uint32)GLOBAL_REG_RBUS_START) | (address)));
  //printf("REGRD %08X=%08X\n", address, data);
  return data;
}

uint32 REGWR (uint32 address, uint32 data) {

  ((* (volatile uint32 *) ( ((uint32)GLOBAL_REG_RBUS_START) | (address))) = cpu_to_le32(data));
  //printf("REGWR %08X=%08X\n", address, data);
//  return SOC_E_NONE;
   return 0;
}
