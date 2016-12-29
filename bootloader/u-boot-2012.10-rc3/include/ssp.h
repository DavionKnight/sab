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

/*******************************************************************
 *
 *  Copyright 2014
 *  Broadcom Corporation
 *  16215 Alton Parkway
 *  Irvine CA 92619-7013
 *
 *  Broadcom provides the current code only to licensees who
 *  may have the non-exclusive right to use or modify this
 *  code according to the license.
 *  This program may be not sold, resold or disseminated in
 *  any form without the explicit consent from Broadcom Corp
 *
 *******************************************************************
 *
 *  Broadcom Cygnus Project
 *  File: ssp.h
 *  Description: the SPI header file
 *
 *  Copyright (C) 2013, Broadcom Corporation
 *  All Rights Reserved.
 *  
 *  This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 *  the contents of this file may not be disclosed to third parties, copied
 *  or duplicated in any form, in whole or in part, without the prior
 *  written permission of Broadcom Corporation.
 *
 *  Version: $Id: $
 *
 *****************************************************************************/

#ifndef _SSP_H_
#define _SSP_H_

#define SPI0_REG_BASE_ADDR ChipcommonG_SPI0_SSPCR0
#define SPI1_REG_BASE_ADDR ChipcommonG_SPI1_SSPCR0
#define SPI2_REG_BASE_ADDR ChipcommonG_SPI2_SSPCR0
#define SPI3_REG_BASE_ADDR ChipcommonG_SPI3_SSPCR0
#define SPI4_REG_BASE_ADDR ChipcommonG_SPI4_SSPCR0
#define SPI5_REG_BASE_ADDR ChipcommonG_SPI5_SSPCR0

/*======================== API ===========================*/

#define SPI_CLOCK_PHASE_0      0 /* use as default */
#define SPI_CLOCK_PHASE_1      1
#define SPI_CLOCK_POLARITY_0   0 /* use as default */
#define SPI_CLOCK_POLARITY_1   1


#define SPI_BASE (spi_id == 0) ? SPI0_REG_BASE_ADDR : ((spi_id == 1) ? SPI1_REG_BASE_ADDR : ((spi_id == 2) ? SPI2_REG_BASE_ADDR : ((spi_id == 3) ? SPI3_REG_BASE_ADDR : ((spi_id == 4) ? SPI4_REG_BASE_ADDR : SPI5_REG_BASE_ADDR))))

/*----- function definitions -----*/
#define cls_spi_byte_write(outByte, spi_id) cls_spi_byte_operation(outByte, NULL, spi_id)
#define cls_spi_byte_read(inByte, spi_id)   cls_spi_byte_operation(0, inByte, spi_id)



#define SSP_BASE            SPI_BASE  


#define SSPCR0              ((volatile unsigned int *)(SSP_BASE + 0x00))
#define SSPCR1              ((volatile unsigned int *)(SSP_BASE + 0x04))
#define SSPDR               ((volatile unsigned int *)(SSP_BASE + 0x08))
#define SSPSR               ((volatile unsigned int *)(SSP_BASE + 0x0C))
#define SSPCPSR             ((volatile unsigned int *)(SSP_BASE + 0x10))
#define SSPIMSC             ((volatile unsigned int *)(SSP_BASE + 0x14))
#define SSPRIS              ((volatile unsigned int *)(SSP_BASE + 0x18))
#define SSPMIS              ((volatile unsigned int *)(SSP_BASE + 0x1C))
#define SSPICR              ((volatile unsigned int *)(SSP_BASE + 0x20))
#define SSPDMACR            ((volatile unsigned int *)(SSP_BASE + 0x24))


// SSPCR0 Control register 0
#define SSPCR0_SCR_DFLT     0x0300      // Serial Clock Rate (divide), default set at 3
#define SSPCR0_SCR_MIN      0
#define SSPCR0_SCR_MAX      255
#define SSPCR0_SCR_MASK     0x00FF
#define SSPCR0_SPH          0x0080      // SSPCLKOUT phase
#define SSPCR0_SPO          0x0040      // SSPCLKOUT polarity
#define SSPCR0_FRF_MOT      0x0000      // Frame format, Motorola
#define SSPCR0_FRF_TI       0x0001      // Frame format, Texas Instruments
#define SSPCR0_FRF_NAT      0x0002      // Frame format, National Microwire
#define SSPCR0_DSS_4        0x0003      // Data packet size, 4bits
#define SSPCR0_DSS_8        0x0007      // Data packet size, 8bits
#define SSPCR0_DSS_16       0x000F      // Data packet size, 16bits

// SSPCR1 Control register 1
#define SSPCR1_SOD          0x0008      // Slave Output mode Disable
#define SSPCR1_MS           0x0000      // Master mode
#define SSPCR1_SL           0x0004      // Slave mode
#define SSPCR1_SSE          0x0002      // Serial port enable
#define SSPCR1_LBM          0x0001      // Loop Back Mode

// SSPSR Status register
#define SSPSR_BSY           0x0010      // Busy
#define SSPSR_RFF           0x0008      // Receive  FIFO full
#define SSPSR_RNE           0x0004      // Receive  FIFO not empty
#define SSPSR_TNF           0x0002      // Transmit FIFO not full
#define SSPSR_TFE           0x0001      // Transmit FIFO empty

// SSPCPSR Clock prescale register
#define SSPCPSR_DFLT        0x0000      // Clock prescale (use with SCR)
#define SSPCPSR_MIN         2
#define SSPCPSR_MAX         254
#define SSPCPSR_MASK        0x00FF		// only 8 bits are valid

// SSPIMSC Interrupt mask set and clear register
#define SSPIMSC_TXIM        0x0008      // Transmit FIFO not Masked
#define SSPIMSC_RXIM        0x0004      // Receive  FIFO not Masked
#define SSPIMSC_RTIM        0x0002      // Receive timeout not Masked
#define SSPIMSC_RORIM       0x0001      // Receive overrun not Masked

// SSPRIS Raw interrupt status register
#define SSPRIS_TXRIS        0x0008      // Raw Transmit interrupt flag
#define SSPRIS_RXRIS        0x0004      // Raw Receive  interrupt flag
#define SSPRIS_RTRIS        0x0002      // Raw Timemout interrupt flag
#define SSPRIS_RORRIS       0x0001      // Raw Overrun  interrupt flag

// SSPMIS Masked interrupt status register
#define SSPMIS_TXMIS        0x0008      // Masked Transmit interrupt flag
#define SSPMIS_RXMIS        0x0004      // Masked Receive  interrupt flag
#define SSPMIS_RTMIS        0x0002      // Masked Timemout interrupt flag
#define SSPMIS_RORMIS       0x0001      // Masked Overrun  interrupt flag

// SSPICR Interrupt clear register
#define SSPICR_RTIC         0x0002      // Clears Timeout interrupt flag
#define SSPICR_RORIC        0x0001      // Clears Overrun interrupt flag

// SSPDMACR DMA control register
#define SSPDMACR_TXDMAE     0x0002      // Enable Transmit FIFO DMA
#define SSPDMACR_RXDMAE     0x0001      // Enable Receive  FIFO DMA

// SPI defaults
#define SSPMAXTIME          1000        // Maximum time to wait for SSP (10*10uS)

#define SSP_FIFO_LEN        8           /* 8 entries in the SSP Tx/Rx FIFO */

#endif
