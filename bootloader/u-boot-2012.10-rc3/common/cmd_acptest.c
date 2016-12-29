/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * To debug NS ACP hang issue, An experiment test was written to perform following sequence. 
 * 1.Lock 
 * 2.Read, modify and write to Memory location-1 (cached memory access).
 * 3.unlock
 * 4.lock
 * 5.Read, modify and write to Memory location-2 (cached memory access).
 * 6.unlock
 * 7.lock
 * 8.Read from Memory location  3 (cached memory access).
 * 9.unlock
 * 10.lock
 * 11.I/O (PCIe) Read/Write.
 * 12.unlock
 * 13.Write character to Uart (preferably with cpu number)
 * 14.Go to step 1.
*/

#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/mach-types.h>
#include <configs/iproc_board.h>
#include <asm/arch/iproc.h>
#include <asm/arch/socregs.h>
#include <asm/arch/reg_utils.h>
#include <asm/system.h>

DECLARE_GLOBAL_DATA_PTR;

int do_acptest (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
  int *memaddr;
  int rd_data;
  int id;

  ulong loop, lp_cnt;

  if (argv[1] == '\0') {
    return CMD_RET_USAGE;
  } 

  loop = simple_strtoul(argv[1], NULL, 16);
  lp_cnt = loop;

  puts ("ACP Test \n");

  /* Wakeup Second CPU */
  asm("SEV");

  id = get_cpuid();

  do 
   {
    memaddr = IPROC_DDR_MEM_BASE2; 

    l2c_lockdown_by_line();
    rd_data = *memaddr;
    printf("CPU ID - %d : Value Read at addr: 0x%08x is 0x%08x\n", id, memaddr, rd_data);

    *memaddr+=0x1;
    rd_data = *memaddr;
    printf("CPU ID - %d : Value Read afer RMW at addr: 0x%08x is 0x%08x\n", id, memaddr, rd_data);

    l2cc_unlock();
    l2cc_invalidate();

    memaddr = IPROC_DDR_MEM_BASE2 + 0x1000; 

    l2c_lockdown_by_line();
    rd_data = *memaddr;
    printf("CPU ID - %d : Value Read at addr: 0x%08x is 0x%08x\n", id, memaddr, rd_data);

    *memaddr+=0x1;
    rd_data = *memaddr;
    printf("CPU ID - %d : Value Read afer RMW at addr: 0x%08x is 0x%08x\n", id, memaddr, rd_data);

    l2cc_unlock();
    l2cc_invalidate();

    memaddr = IPROC_DDR_MEM_BASE2 + 0x10000;
    l2c_lockdown_by_line();
    rd_data = *memaddr;
    printf("CPU ID - %d : Value Read at addr: 0x%08x is 0x%08x\n", id, memaddr, rd_data);

    l2cc_unlock();
    l2cc_invalidate();


    l2c_lockdown_by_line();

    loop--;
    } while (loop != 0);
/*
    printf("Argc value is %d \n", argc);
    printf("Argv value is %s \n", argv[1]);
*/
  printf("Executed the loop on core %d %d times \n", id, lp_cnt);
  return 0;
}

void l2c_lockdown_by_line (void);


void l2c_lockdown_by_line () {

 int *l2c_ldbl;

    /* Enable lockdown by line */
  l2c_ldbl = IHOST_L2C_CACHE_ID + 0x950;

 *l2c_ldbl = 0x1;
 return 0; 
}

/***************************************************/

U_BOOT_CMD(
	acptest,	2,	1,	do_acptest,
	"acptest loop",
	"acptest loop \n"
	"  - Perform RMW at MEM1, MEM2, Read at MEM3 and RW at I/O address for loop times\n"
);
