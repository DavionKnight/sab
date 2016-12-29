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
#include <post.h>


#if CONFIG_POST & CONFIG_SYS_POST_KEYPAD

#include "asm/arch/socregs.h"

#define KEYPAD_EN 1
#define KP_PULL_DOWN 0
#define KP_NO_SWAP_RC 0
#define KP_USE_GPIO_DIS 0
#define KP_MAX_ROWS 4
#define KP_MAX_COLS 4
#define KP_32MS_FILT_DUR 0x5
#define KP_COLUMN_FILTER_EN 1
#define KP_STAT_FILTER_EN 1
#define KP_RAISING_EDGE 0x00550055 /* For a 4x4 Keypad. For a larger matrix set additional bits as required */
#define KP_FALLING_EDGE 0x00AA00AA /* For a 4x4 Keypad. For a larger matrix set additional bits as required */
#define KP_CLEAR_ALL_INTs 0xFFFFFFFF
#define KP_INTERRUPT_MASK 0X0F0F0F0F /*Doc says Mask bit=0 => No interrupt. */


int KEYPAD_post_test(int flags);
static void KEYPAD_post_init(void);

/* Howz the following used 
 * CRMU_ASIU_KEYPAD_CLK_DIV 
*/
static void KEYPAD_post_init() {
	post_log("Initializing the DUT \n");
        /* Select Function 2 for GPIO 8  - 11 */
        CPU_RMW_AND_32(CRMU_IOMUX_CTRL1_OFFSET, 0xFFFF0000);
        CPU_RMW_OR_32(CRMU_IOMUX_CTRL1_OFFSET, 0x1111);
        /* Select Function 2 for GPIO 16 - 19 */
        CPU_RMW_AND_32(CRMU_IOMUX_CTRL2_OFFSET, 0xFFFF0000);
        CPU_RMW_OR_32(CRMU_IOMUX_CTRL2_OFFSET, 0x1111);
        /* Enable Clock gate for KeyPad */
        CPU_RMW_OR_32(ASIU_TOP_CLK_GATING_CTRL, (1 << ASIU_TOP_CLK_GATING_CTRL__KEYPAD_CLK_GATE_EN));
}

static void keypad_ctrlr_config(uint32_t KeypadCtrlEn, uint32_t ScanMode, uint32_t SwapRc, uint32_t IoMode, uint32_t MaxRows, uint32_t MaxCols, uint32_t ColFilType, uint32_t ColFilEn, uint32_t StatFilType, uint32_t StatFilEn);


static void keypad_ctrlr_config(uint32_t KeypadCtrlEn, uint32_t ScanMode, uint32_t SwapRc, uint32_t IoMode, uint32_t MaxRows, uint32_t MaxCols, uint32_t ColFilType, uint32_t ColFilEn, uint32_t StatFilType, uint32_t StatFilEn)
{
	uint32_t reg_data;
	uint32_t ColIoCtrl, RowIoCtrl;


	reg_data = (((MaxRows-1) << KEYPAD_TOP_REGS_KPCR__RowWidth_R) | 
			((MaxCols-1) << KEYPAD_TOP_REGS_KPCR__ColumnWidth_R) | 
			(StatFilEn << KEYPAD_TOP_REGS_KPCR__StatusFilterEnable) | 
			(StatFilType << KEYPAD_TOP_REGS_KPCR__StatusFilterType_R) | 
			(ColFilEn << KEYPAD_TOP_REGS_KPCR__ColFilterEnable) | 
			(ColFilType << KEYPAD_TOP_REGS_KPCR__ColumnFilterType_R) | 
			(IoMode << KEYPAD_TOP_REGS_KPCR__soft) | 
			(SwapRc << KEYPAD_TOP_REGS_KPCR__SwapRowColumn) | 
			(ScanMode << KEYPAD_TOP_REGS_KPCR__Mode) | 
			(KeypadCtrlEn << KEYPAD_TOP_REGS_KPCR__Enable));

	CPU_WRITE_32(KEYPAD_TOP_REGS_KPCR, reg_data);

	reg_data = 0;
        /* The description of KPIOR fields are ambiguous in the doc. Need to revisit this*/
	if (!SwapRc) {
	        ColIoCtrl = !(0xFF << KEYPAD_TOP_REGS_KPIOR__ColumnOContrl_L);
	        RowIoCtrl =  (0xFF << KEYPAD_TOP_REGS_KPIOR__RowOContrl_L);

	        reg_data &= ColIoCtrl;
	        reg_data |= RowIoCtrl;
	} else {
	        ColIoCtrl =  (0xFF << KEYPAD_TOP_REGS_KPIOR__ColumnOContrl_L);
	        RowIoCtrl = !(0xFF << KEYPAD_TOP_REGS_KPIOR__RowOContrl_L);

	        reg_data |= ColIoCtrl;
	        reg_data &= RowIoCtrl;
	}

	CPU_WRITE_32(KEYPAD_TOP_REGS_KPIOR, reg_data);

	if (!ScanMode) {
		CPU_WRITE_32(KEYPAD_TOP_REGS_KPEMR0, KP_RAISING_EDGE);
		CPU_WRITE_32(KEYPAD_TOP_REGS_KPEMR1, KP_RAISING_EDGE);
	} else {
		CPU_WRITE_32(KEYPAD_TOP_REGS_KPEMR0, KP_FALLING_EDGE);
		CPU_WRITE_32(KEYPAD_TOP_REGS_KPEMR1, KP_FALLING_EDGE);
	}

	/* The doc says Mask = 0 is no interrupt; It appears Mask = 1 for interrupt to be generated.Needs clarification*/
	CPU_WRITE_32(KEYPAD_TOP_REGS_KPIMR0, KP_INTERRUPT_MASK);
       
	/* Clear all the interrupts */
	CPU_WRITE_32(KEYPAD_TOP_REGS_KPICR0, KP_CLEAR_ALL_INTs);
	CPU_WRITE_32(KEYPAD_TOP_REGS_KPICR1, KP_CLEAR_ALL_INTs);
	post_log("Cleared all the interrupts \n");

	/*Check to see there are no active interrupts at the start*/
	reg_data = 0xFFFFFFFF;

	reg_data = CPU_READ_32(KEYPAD_TOP_REGS_KPISR0);
	reg_data |= CPU_READ_32(KEYPAD_TOP_REGS_KPISR1);

	if(reg_data != 0x0) {
		post_log("There are active interrupts at the start after clearing all the interrupts\n");
		post_log("KPISR0 | KPISR1 reads : 0x%x\n",reg_data);
	}


	reg_data = CPU_READ_32(KEYPAD_TOP_REGS_KPSSR0);
	post_log("Initial Value of KPSSR0 Register : 0x%x \n", reg_data);

	reg_data = CPU_READ_32(KEYPAD_TOP_REGS_KPSSR1);
	post_log("Initial Value of KPSSR1 Register : 0x%x \n", reg_data);

}

static uint32_t get_kp_scan_status (uint32_t kp_row_mask, uint32_t kp_shift);

static uint32_t get_kp_scan_status (uint32_t kp_row_mask, uint32_t kp_shift) {
    uint32_t kp_intr_stat = 0x0;
    uint32_t kp_status;


	while (kp_intr_stat == 0x0) {
                kp_intr_stat = CPU_READ_32(KEYPAD_TOP_REGS_KPISR0);
	}

	if ((kp_row_mask & kp_intr_stat) != 0x0) {
		post_log("Error: An interrupt corresponding to a Key that was not pressed is active \n");
		post_log("KPISR0 value : 0x%x\n", kp_intr_stat);
	}
	
       	/*Adding a 32ms delay to accomodate key status filtering duration*/
/*	__udelay(32*1000);

	key_status = CPU_READ_32(KEYPAD_TOP_REGS_KPSSR0);

	post_log("Key status register KPSSR0 value read : 0x%x\n", key_status);

        kp_status = (((key_status & !kp_row_mask) & (kp_intr_stat & !kp_row_mask)) >> kp_shift);
*/

	kp_status = ((kp_intr_stat & !kp_row_mask) >> kp_shift);
	return kp_status;
}

int KEYPAD_post_test(int flags)
{   
    int iter = 0;
    uint32_t kp_row_mask; 
    uint32_t kp_shift;
    uint32_t kp_press_status;

    KEYPAD_post_init();

    post_log("\n Configuring KeyPad Controller \n");

    keypad_ctrlr_config(KEYPAD_EN, KP_PULL_DOWN, KP_NO_SWAP_RC, KP_USE_GPIO_DIS, KP_MAX_ROWS, KP_MAX_COLS, KP_32MS_FILT_DUR, KP_COLUMN_FILTER_EN, KP_32MS_FILT_DUR, KP_STAT_FILTER_EN);

    while (iter < 4 ) {
	    switch (iter) {
		    case 0:
		       	    	post_log("Press any Key on First Row \n");

				kp_row_mask = 0xFFFFFFF0;
				kp_shift = 0;
				kp_press_status = get_kp_scan_status(kp_row_mask, kp_shift);

                                switch (kp_press_status) {
					case 1:
						post_log("Registered key press is 1\n");
	                                        CPU_WRITE_32(KEYPAD_TOP_REGS_KPICR0, KP_CLEAR_ALL_INTs);
					        break;	
					case 2:
						post_log("Registered key press is 2\n");
	                                        CPU_WRITE_32(KEYPAD_TOP_REGS_KPICR0, KP_CLEAR_ALL_INTs);
					        break;	
					case 4:
						post_log("Registered key press is 3\n");
	                                        CPU_WRITE_32(KEYPAD_TOP_REGS_KPICR0, KP_CLEAR_ALL_INTs);
					        break;	
					case 8:
						post_log("Registered key press is A\n");
	                                        CPU_WRITE_32(KEYPAD_TOP_REGS_KPICR0, KP_CLEAR_ALL_INTs);
					        break;	
					default:
						post_log("Key Not recognized \n");
						return 1;


				}
				break;
		    case 1:
			    	post_log("Press any Key on Second Row \n");
				kp_row_mask = 0xFFFFF0FF;
				kp_shift = 8;
				kp_press_status = get_kp_scan_status(kp_row_mask, kp_shift);

                                switch (kp_press_status) {
					case 1:
						post_log("Registered key press is 4\n");
	                                        CPU_WRITE_32(KEYPAD_TOP_REGS_KPICR0, KP_CLEAR_ALL_INTs);
					        break;	
					case 2:
						post_log("Registered key press is 5\n");
	                                        CPU_WRITE_32(KEYPAD_TOP_REGS_KPICR0, KP_CLEAR_ALL_INTs);
					        break;	
					case 4:
						post_log("Registered key press is 6\n");
	                                        CPU_WRITE_32(KEYPAD_TOP_REGS_KPICR0, KP_CLEAR_ALL_INTs);
					        break;	
					case 8:
						post_log("Registered key press is B\n");
	                                        CPU_WRITE_32(KEYPAD_TOP_REGS_KPICR0, KP_CLEAR_ALL_INTs);
					        break;	
					default:
						post_log("Key Not recognized \n");
						return 1;


				}
				break;
		    case 2:
			    	post_log("Press any Key on Third Row \n");
				kp_row_mask = 0xFFF0FFFF;
				kp_shift = 16;
				kp_press_status = get_kp_scan_status(kp_row_mask, kp_shift);

                                switch (kp_press_status) {
					case 1:
						post_log("Registered key press is 7\n");
	                                        CPU_WRITE_32(KEYPAD_TOP_REGS_KPICR0, KP_CLEAR_ALL_INTs);
					        break;	
					case 2:
						post_log("Registered key press is 8\n");
	                                        CPU_WRITE_32(KEYPAD_TOP_REGS_KPICR0, KP_CLEAR_ALL_INTs);
					        break;	
					case 4:
						post_log("Registered key press is 9\n");
	                                        CPU_WRITE_32(KEYPAD_TOP_REGS_KPICR0, KP_CLEAR_ALL_INTs);
					        break;	
					case 8:
						post_log("Registered key press is C\n");
	                                        CPU_WRITE_32(KEYPAD_TOP_REGS_KPICR0, KP_CLEAR_ALL_INTs);
					        break;	
					default:
						post_log("Key Not recognized \n");
						return 1;


				}
				break;
		    case 3:
			    	post_log("Press any Key on Fourth Row \n");
				kp_row_mask = 0xF0FFFFFF;
				kp_shift = 24;
				kp_press_status = get_kp_scan_status(kp_row_mask, kp_shift);

                                switch (kp_press_status) {
					case 1:
						post_log("Registered key press is *\n");
	                                        CPU_WRITE_32(KEYPAD_TOP_REGS_KPICR0, KP_CLEAR_ALL_INTs);
					        break;	
					case 2:
						post_log("Registered key press is 0\n");
	                                        CPU_WRITE_32(KEYPAD_TOP_REGS_KPICR0, KP_CLEAR_ALL_INTs);
					        break;	
					case 4:
						post_log("Registered key press is #\n");
	                                        CPU_WRITE_32(KEYPAD_TOP_REGS_KPICR0, KP_CLEAR_ALL_INTs);
					        break;	
					case 8:
						post_log("Registered key press is D\n");
	                                        CPU_WRITE_32(KEYPAD_TOP_REGS_KPICR0, KP_CLEAR_ALL_INTs);
					        break;	
					default:
						post_log("Key Not recognized \n");
						return 1;


				}
				break;
		    default:
				post_log("This should never happen \n ");

	    }
 
    iter++;
    }

    post_log("\n\r KEYPAD Diag Passed\n");
    return 0;
    
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_KEYPAD */
