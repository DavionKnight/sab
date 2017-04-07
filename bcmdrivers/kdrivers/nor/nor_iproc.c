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

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/slab.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <mach/hardware.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <mach/reg_utils.h>

#include "smc-reg.h"

static struct mtd_info *nor_mtd;

static struct map_info s29gl_map = {
	.name =		"S29GL",
	.bankwidth =	4,
	.size =		S29GL_FLASH_SIZE,
	.phys =		S29GL_FLASH_PHYS,
};

static struct mtd_partition s29gl_partitions[] = {
        {       /* 768 KB */
                .name = "boot",
                .size = 0x00C0000,
                .offset = 0,
                .mask_flags = MTD_WRITEABLE    /* Read only partition */
        },{
                /* 128 KB */
                .name = "env",
                .size = 0x40000,
                .offset = 0xc0000,
                .mask_flags = MTD_WRITEABLE    /* Read only partition */
        },{
                /* 15 MB */
                .name = "system",
                .size = 0xF00000,
                .offset = 0x100000,
                .mask_flags = MTD_WRITEABLE     /* Read only partition */ 
        },{
                /* 16 MB */
                .name = "rootfs",
                .size =   0x1000000,
                .offset = 0x1000000
        },{
                /* 32 MB */
                .name = "custfs",
                .size = MTDPART_SIZ_FULL,
                .offset = MTDPART_OFS_APPEND
        }
};

#define NUM_PARTITIONS ARRAY_SIZE(s29gl_partitions)

/*
 * Initialize FLASH support
 */
static int __init s29gl_mtd_init(void)
{
    volatile int *nor_enable = NULL;
    u32 straps, val;
    
    /* Check boot device */
    straps = readl_relaxed(IPROC_DMU_BASE_VA + IPROC_DMU_STRAPS_OFFSET) & 
            (7 << IPROC_STRAP_BOOT_DEV_SHIFT);
    if (straps == IPROC_STRAP_BOOT_DEV_NAND) {
        /* If booting from NAND, PNOR cannot be used */
        return -ENODEV;
    } else if (straps != IPROC_STRAP_BOOT_DEV_PNOR) {
        
        /* Switching to PNOR only if not booting from PNOR */
        nor_enable = (volatile int *)HW_IO_PHYS_TO_VIRT(PNOR_NAND_SEL_REG);
        val = readl_relaxed(nor_enable);
        val |= 
            (1UL << PNOR_NAND_SEL_REG_OVERRIDE) | 
            (1UL << PNOR_NAND_SEL_REG_PNOR_SEL);
        writel_relaxed(val, nor_enable);
           
#ifdef CONFIG_MACH_IPROC_P7
        /* Configure controller memory width based on strap */
        straps = readl_relaxed(HW_IO_PHYS_TO_VIRT(ICFG_PNOR_STRAPS)) & 
                (1 << ICFG_PNOR_STRAPS__PNOR_SRAM_MW_R);
        if (straps) {
            /* 16-bit */
            val = readl_relaxed(HW_IO_PHYS_TO_VIRT(PNOR_set_opmode));
            val |= (1 << PNOR_set_opmode__set_mw_R);
            writel_relaxed(val, HW_IO_PHYS_TO_VIRT(PNOR_set_opmode));
        } else {
            /* 8-bit */
            val = readl_relaxed(HW_IO_PHYS_TO_VIRT(PNOR_set_opmode));
            val &= ~(1 << PNOR_set_opmode__set_mw_R);
            writel_relaxed(val, HW_IO_PHYS_TO_VIRT(PNOR_set_opmode));
        }
        val = readl_relaxed(HW_IO_PHYS_TO_VIRT(PNOR_direct_cmd));
        val |= (2 << PNOR_direct_cmd__cmd_type_R);
        writel_relaxed(val, HW_IO_PHYS_TO_VIRT(PNOR_direct_cmd));
#endif 
    }

    printk(KERN_INFO "S29GL-MTD: NOR_INTERFACE Enabled\n");

    udelay(1000);

    s29gl_map.virt = ioremap(s29gl_map.phys, s29gl_map.size);

    if (!s29gl_map.virt) {
        printk(KERN_ERR "S29GL-MTD: ioremap failed\n");
        if (nor_enable) {
            /* revert to NAND mode */
            val = readl_relaxed(nor_enable);
            val &= ~(1UL << PNOR_NAND_SEL_REG_PNOR_SEL);
            writel_relaxed(val, nor_enable);
        }
        return -EIO;
    }

    simple_map_init(&s29gl_map);

    // Probe for flash bankwidth 4
    printk (KERN_INFO "S29GL-MTD probing 32bit FLASH\n");
    nor_mtd = do_map_probe("cfi_probe", &s29gl_map);
    if (!nor_mtd) {
        printk (KERN_INFO "S29GL-MTD probing 16bit FLASH\n");
        // Probe for bankwidth 2
        s29gl_map.bankwidth = 2;
        nor_mtd = do_map_probe("cfi_probe", &s29gl_map);
    }

    if (nor_mtd) {
        nor_mtd->owner = THIS_MODULE;
        mtd_device_parse_register(nor_mtd, NULL, NULL,
					  s29gl_partitions, NUM_PARTITIONS);
        printk (KERN_INFO "S29GL-MTD MTD partitions parsed!\n");
	return 0;
    }

    printk (KERN_INFO "S29GL-MTD NO FLASH found!\n");
    if (nor_enable) {
        /* revert to NAND mode */
        val = readl_relaxed(nor_enable);
        val &= ~(1UL << PNOR_NAND_SEL_REG_PNOR_SEL);
        writel_relaxed(val, nor_enable);
    }
    iounmap((void *)s29gl_map.virt);
    return -ENXIO;
}

/*
 * Cleanup
 */
static void __exit s29gl_mtd_cleanup(void)
{

    if (nor_mtd) {
        mtd_device_unregister(nor_mtd);
        map_destroy(nor_mtd);
    }

    if (s29gl_map.virt) {
        iounmap((void *)s29gl_map.virt);
        s29gl_map.virt = 0;
    }
}


module_init(s29gl_mtd_init);
module_exit(s29gl_mtd_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haifeng Jiang <haifengj@broadcom.com>");
MODULE_DESCRIPTION("MTD map driver for Hurricane2 Deerhound evaluation boards");
