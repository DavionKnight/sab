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
#include <asm/io.h>
#include <asm/mach-types.h>
#include <config.h>
#include <asm/arch/iproc.h>
#include <asm/system.h>
#include "asm/iproc/reg_utils.h"
#include "asm/iproc/iproc_common.h"

DECLARE_GLOBAL_DATA_PTR;

/*****************************************
 * board_init -early hardware init
 *****************************************/
int board_init (void)
{

#if !defined(CONFIG_SYS_NO_FLASH) && !defined(CONFIG_NOR_IPROC_BOOT) && \
	!defined(CONFIG_NAND_IPROC_BOOT)
    
    /* If booting from PNOR, PNOR is already selected, no override required.
     * If booting from NAND, we shouldn't switch to PNOR.
     */
    *(volatile int *)ICFG_IPROC_IOPAD_SW_OVERRIDE_CTRL |= cpu_to_le32(
        (1 << ICFG_IPROC_IOPAD_SW_OVERRIDE_CTRL__iproc_pnor_sel) |
        (1 << ICFG_IPROC_IOPAD_SW_OVERRIDE_CTRL__iproc_pnor_sel_sw_ovwr));
        
    /* Configure controller memory width based on mw strap */
    if (*(volatile int *)ICFG_PNOR_STRAPS & 
        cpu_to_le32(1 << ICFG_PNOR_STRAPS__PNOR_SRAM_MW_R)) {
        /* 16-bit */
        *(volatile int *)PNOR_set_opmode |= 
                cpu_to_le32(1 << PNOR_set_opmode__set_mw_R);
    } else {
        /* 8-bit */
        *(volatile int *)PNOR_set_opmode &= 
                cpu_to_le32(~(1 << PNOR_set_opmode__set_mw_R));
    }
    *(volatile int *)PNOR_direct_cmd |= 
            cpu_to_le32(2 << PNOR_direct_cmd__cmd_type_R);
    
#endif /* !CONFIG_SYS_NO_FLASH && !CONFIG_NOR_IPROC_BOOT */

    gd->bd->bi_arch_number = CONFIG_MACH_TYPE;      /* board id for linux */
    gd->bd->bi_boot_params = LINUX_BOOT_PARAM_ADDR; /* adress of boot parameters */

    return 0;
}

/*****************************************************************
 * misc_init_r - miscellaneous platform dependent initializations
 ******************************************************************/
int misc_init_r (void)
{
    return(0);
}

/**********************************************
 * dram_init - sets uboots idea of sdram size
 **********************************************/
int dram_init (void)
{
#if !defined(CONFIG_IPROC_NO_DDR) && !defined(CONFIG_NO_CODE_RELOC)
    ddr_init3();
#endif

    gd->ram_size = CONFIG_PHYS_SDRAM_1_SIZE - CONFIG_PHYS_SDRAM_RSVD_SIZE;

    return 0;
}

int board_early_init_f (void)
{
    uint32_t sku;
    sku = reg32_read((volatile uint32_t *)ICFG_CHIP_ID_REG);
    if ((sku & 0xfff0) == 0xb060) {
        /* Ranger2 - 5606x: CPU 1250MHz, AXI 400MHz, DDR 800MHz */
        iproc_config_armpll(1250);
        iproc_config_genpll(0);
    } else {
        /* Greyhound - 5341x: CPU 600MHz, AXI 200MHz, DDR 667MHz */
        iproc_config_armpll(600);
        iproc_config_genpll(1);
    }
    
#ifndef CONFIG_IPROC_NO_DDR
    /* Initialize ATCM and BTCM for SHMOO (clear to avoid ECC error) */
    {
        int i;
        volatile int *p;
        p = (volatile int *)0x01000000;
        for(i=0; i<64*1024/4; i++, p++)
            *p = 0;
        p = (volatile int *)0x01080000;
        for(i=0; i<128*1024/4; i++, p++)
            *p = 0;
    }
#endif

    return 0;
}

int board_late_init (void) 
{
    int status = 0;

#if defined(CONFIG_L2C_AS_RAM) && !defined(CONFIG_NO_CODE_RELOC)
    extern ulong mmu_table_addr;    /* MMU on flash fix: 16KB aligned */
    
    printf("Unlocking L2 Cache ...");
    l2cc_unlock();
    printf("Done\n");

    /* 
     * Relocate MMU table from flash to DDR since flash may not be always accessable.
     * eg. When QSPI controller is in MSPI mode.
     */
    asm volatile ("mcr p15, 0, %0, c2, c0, 0"::"r"(mmu_table_addr)); /* Update TTBR0 */
    asm volatile ("mcr p15, 0, r1, c8, c7, 0");  /* Invalidate TLB*/
    asm volatile ("mcr p15, 0, r1, c7, c10, 4"); /* DSB */
    asm volatile ("mcr p15, 0, r0, c7, c5, 4"); /* ISB */
    
    /* Invalidate L2 cache after MMU table relocated */
    l2cc_invalidate();
    
#endif

    disable_interrupts();
    iproc_clk_enum();
    
#ifdef STDK_BUILD
    enable_interrupts();
#endif /* STDK_BUILD */

#if !defined(CONFIG_IPROC_NO_DDR) && defined(CONFIG_SHMOO_AND28_REUSE)
     save_shmoo_to_flash();
#endif

    return status;
}

int board_eth_init(bd_t *bis)
{
	int rc = -1;

#ifdef CONFIG_BCMIPROC_ETH
    int idx;
    
    for (idx = 0; idx < CONFIG_GMAC_NUM; idx++) {
    	printf("Registering eth%d\n", idx);
	    rc = bcmiproc_eth_register(idx);
	    if (rc != 0) {
	        return rc;
	    }
	}
#endif

	return rc;
}
