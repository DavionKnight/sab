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
#include <mmc.h>

#if CONFIG_POST & CONFIG_SYS_POST_EMMC

/* Size in byte */
#define MMC_DATA_BUF_SIZE	32*1024

#define MMC_DATA_BUF_ADDR      0x89000000
#define MMC_DATA_BUF_ADDR_STR  "0x89000000"
#define MMC_DATA_DST_BUF_ADDR  0x8a000000

#define SD_BLK_SIZE         512
#define TEST_BLK_CNT        MMC_DATA_BUF_SIZE/SD_BLK_SIZE
#define SD_START_BLK        10     

int eMMC_post_test (int flags)
{
        char argv_buf[7*10];
        char * cmd_argv[7];
        int  i, n;
		void *addr = (void *)MMC_DATA_BUF_ADDR;
		u32 blk = 0x10000;
		int ret = 0;

        post_log("\neMMC not available on NS platform !\n");
        return -1;
}
#endif /* CONFIG_POST & CONFIG_SYS_POST_EMMC */
