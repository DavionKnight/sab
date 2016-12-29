/******************************************************************************/
/*                                                                            */
/*  Copyright 2015  Broadcom Corporation                                      */
/*                                                                            */
/*     Unless you and Broadcom execute a separate written software license    */
/*     agreement governing  use of this software, this software is licensed   */
/*     to you under the terms of the GNU General Public License version 2     */
/*     (the GPL), available at                                                */
/*                                                                            */
/*          http://www.broadcom.com/licenses/GPLv2.php                        */
/*                                                                            */
/*     with the following added to such license:                              */
/*                                                                            */
/*     As a special exception, the copyright holders of this software give    */
/*     you permission to link this software with independent modules, and to  */
/*     copy and distribute the resulting executable under terms of your       */
/*     choice, provided that you also meet, for each linked independent       */
/*     module, the terms and conditions of the license of that module.        */
/*     An independent module is a module which is not derived from this       */
/*     software.  The special exception does not apply to any modifications   */
/*     of the software.                                                       */
/*     Notwithstanding the above, under no circumstances may you combine      */
/*     this software in any way with any other Broadcom software provided     */
/*     under a license other than the GPL, without Broadcom's express prior   */
/*     written consent.                                                       */
/*                                                                            */

#ifndef LCD_COMMON_H
#define LCD_COMMON_H

typedef enum
{
	BPP_1 = 0,
	BPP_2,
	BPP_4,
	BPP_8,
	BPP_16,
	BPP_24,
	BPP_565,
	BPP_12
}bits_per_pixel_t;

typedef struct 
{
	uint32_t PPL;   // pixel per line
	uint32_t LPF;   //line per frame
	uint32_t HSW;   //horizontal pulse width
	uint32_t HBP;   //horziontal back porch
	uint32_t HFP;   //horziontal front porch 
	uint32_t VSW;   //vertical pulse width
	uint32_t VBP;   //vertical back porch
	uint32_t VFP;   //vertical front porch
	uint32_t LED;   //line-end signal delay
    uint8_t  IHS;   //invert horrizontal sync polarity 
    uint8_t  IVS;  //invert vertical sync polarity
    uint8_t  IPC;   //invert panel clock
}lcd_param_t;

#endif // #ifndef LCD_COMMON.h
