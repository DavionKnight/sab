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

/*

 */

#include <common.h>
#include <post.h>

#if CONFIG_POST & CONFIG_SYS_POST_NAND
/* Size in byte */
#define NAND_DATA_BUF_SIZE	 1024*1024

#if defined(CONFIG_SABER2)

#define NAND_DEVICE_OFFSET     0x0000
#define NAND_DEVICE_OFFSET_STR "0x0000"
#define NAND_TEST_LENGTH       0x100000     /* equals to NAND_DATA_BUF_SIZE */
#define NAND_TEST_LENGTH_STR   "0x100000"    /* equals to NAND_DATA_BUF_SIZE */

#define NAND_STORE_ADDR        0x62000000
#define NAND_STORE_ADDR_STR    "0x62000000"
#define NAND_READ_BUF_ADDR     0x62800000
#define NAND_READ_ADDR_STR     "0x62800000"
#define NAND_WRITE_BUF_ADDR    0x63000000
#define NAND_WRITE_ADDR_STR    "0x63000000"

#else
#define NAND_READ_BUF_ADDR   0x8A000000
#define NAND_WRITE_BUF_ADDR  0x89000000
#define NAND_WRITE_ADDR_STR  "89000000"
#define NAND_READ_ADDR_STR   "8a000000"
#endif

extern void iproc_nand_iomux(int op);
extern int do_nand(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[]);
int NAND_post_test (int flags)
{
#if defined(CONFIG_SABER2)
    int  i, j, rc = 0;
    u32  rd, wr;
    char *cmd_argv[7][6];
    char argv_buf[7][6][20] = 
    {
    	 /* Print NAND chip information */
       "nand", "info",  "",                  "",                     "",                   "INFO",
       /* Copy content of NAND tested area to DDR */
       "nand", "read",  NAND_STORE_ADDR_STR, NAND_DEVICE_OFFSET_STR, NAND_TEST_LENGTH_STR, "BACKUP",
       /* Erase content of NAND tested area       */
       "nand", "erase", NAND_DEVICE_OFFSET_STR, NAND_TEST_LENGTH_STR, "",                  "ERASE",
       /* Write tested data from DDR into NAND tested area */
       "nand", "write", NAND_WRITE_ADDR_STR, NAND_DEVICE_OFFSET_STR, NAND_TEST_LENGTH_STR, "WRITE",
       /* Read tested area of NAND to DDR */
       "nand", "read",  NAND_READ_ADDR_STR,  NAND_DEVICE_OFFSET_STR, NAND_TEST_LENGTH_STR, "READ",
       /* Erase content of NAND tested area       */
       "nand", "erase", NAND_DEVICE_OFFSET_STR, NAND_TEST_LENGTH_STR, "",                  "ERASE",
       /* Restore data back to NAND */
       "nand", "write", NAND_STORE_ADDR_STR, NAND_DEVICE_OFFSET_STR, NAND_TEST_LENGTH_STR, "RESTORE"
    };

    /* Memory Fill */
    post_log("\nFill memory with incremental data in DDR addr=0x%8.8x\n", NAND_WRITE_BUF_ADDR);
    for (i=0; i<NAND_DATA_BUF_SIZE/4; i++)
    {
       *(unsigned long *)(NAND_WRITE_BUF_ADDR + i*4) = i; 
	  }
	  
    for (i=0; i<7; i++)
    {
      for (j=0; j<6; j++)
	      cmd_argv[i][j] = &argv_buf[i][j];
	    
	    post_log("\nNAND %s", cmd_argv[i][5]);
	    if (i == 0)   /* NAND info */
	    {
	       rc = do_nand(0, 0, 2, cmd_argv[i]);
	    } else if ((i == 2) || (i == 5))  /* NAND erase */
	    {
	    	    post_log(" --- from NAND offset=%s cnt=0x%x Bytes\n",
                     cmd_argv[i][2], NAND_TEST_LENGTH);
	    	    rc = do_nand(0, 0, 4, cmd_argv[i]);
	    } else
	    {
	       if ((i == 1) || (i == 4))
	    	    post_log(" --- from NAND offset=0x%8.8x to DDR addr=%s cnt=0x%x Bytes\n",
                     NAND_DEVICE_OFFSET, cmd_argv[i][2], NAND_TEST_LENGTH);
	       else
	    	    post_log(" --- from DDR addr=%s to NAND offset=0x%8.8x cnt=0x%x Bytes\n",
                     cmd_argv[i][2], NAND_DEVICE_OFFSET, NAND_TEST_LENGTH);
	       rc = do_nand(0, 0, 5, cmd_argv[i]);
            }
            if (rc != 0) goto failed;
    } 
#else
        char argv_buf[7*10];
        char * cmd_argv[7];
        int  i, rc = 0;
        u32  rd, wr;

		if ( (flags & POST_AUTO) !=  POST_AUTO )
		{

			post_log("\n\nPlease set strap pin: A1=0xfc, A2=0xa", 0);
			post_getConfirmation("\nReady? (Y/N)");
		}
		else
		{
			post_set_strappins("A1 fc%A2 a%");
		}

        memset(&argv_buf, 0, 7*10);
        for(i=0; i<7; i++)
    		cmd_argv[i] = &argv_buf[i*10];

        /* NAND Info */
        printf("\n\nNAND Info\n");
        strcpy(cmd_argv[0], "nand");
        strcpy(cmd_argv[1], "info");
        do_nand(0, 0, 2, cmd_argv);
		udelay(200*1000);

        /* Memory Fill */
        post_log("\n\nFill memory with incremental data\n");
        for(i=0; i<NAND_DATA_BUF_SIZE/4; i++)
        {
		    *(unsigned long *)(NAND_WRITE_BUF_ADDR + i*4) = i; 
	    }

        /* NAND write */
        post_log("\n\nNAND data write, 1Mbyte\n");
        post_log("NAND write: from ddr_addr=0x%x to NAND offset=0x4000000 cnt=1MByte\n",NAND_WRITE_BUF_ADDR);
        strcpy(cmd_argv[0], "nand");
        strcpy(cmd_argv[1], "write");
        strcpy(cmd_argv[2], NAND_WRITE_ADDR_STR);
        strcpy(cmd_argv[3], "4000000");
        strcpy(cmd_argv[4], "100000");
        rc = do_nand(0, 0, 5, cmd_argv);
	    if(rc != 0) goto failed;


        /* NAND read */
        post_log("\n\nNAND data read, 1Mbyte\n");
        post_log("NAND read: from NAND offset=0x4000000 to ddr_addr=0x%x cnt=1MByte\n",NAND_READ_BUF_ADDR);
        strcpy(cmd_argv[0], "nand");
        strcpy(cmd_argv[1], "read");
        strcpy(cmd_argv[2], NAND_READ_ADDR_STR);
        strcpy(cmd_argv[3], "4000000");
        strcpy(cmd_argv[4], "100000");
        rc = do_nand(0, 0, 5, cmd_argv);
	    if(rc != 0) goto failed;
#endif  /* CONFIG_SABER2 */

        /* NAND Verify */
        for(i=0; i<NAND_DATA_BUF_SIZE/4; i++)
        {
            rd = *(u32 *)(NAND_READ_BUF_ADDR + i*4);
            wr = *(u32 *)(NAND_WRITE_BUF_ADDR + i*4);

		    if(rd != wr)
		    {
			    post_log("\n !! Compare failed at address <0x%08x>, write=0x%08x, read=0x%08x\n", 
						    NAND_READ_BUF_ADDR + i*4, wr, rd );
			    break;
		    }
	    }

        if(i==NAND_DATA_BUF_SIZE/4)
	    {
        	post_log("\n\nRead back and compare OK\n");
        	post_log("\n\nNAND test passed\n");
	        return 0;
	    }

failed:
	    post_log("\nNAND test failed");
	    return -1;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_NAND */
