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
#include <config.h>
#include <common.h>
#include <asm/arch/iproc_regs.h>
#include "asm/arch/socregs.h"
#include "asm/iproc/reg_utils.h"

#define mb() __asm__ __volatile__ ("" : : : "memory")

unsigned int armpll_clk_tab[] = {
	/*  mode,	ndiv_int,	ndiv_frac,	pdiv,	freqid */
		25,	64,			1,			1,		0,	
		100,	64,			1,			1,		2,	
		400,	64,			1,			1,		6,	
		448,	71,			713050,		1,		6,	
		500,	80,			1,			1,		6,	
		560,	89,			629145,		1,		6,	
		600,	96,			1,			1,		6,	
		800,	64,			1,			1,		7,	
		896,	71,			713050,		1,		7,	
		1000,	80,			1,			1,		7,	
		1100,	88,			1,			1,		7,	
		1120,	89,			629145,		1,		7,	
		1200,	96,			1,			1,		7,	
		0xffffffff
};

#if defined(CONFIG_IPROC_P7)

#define ARM_FREQ_1500 1500
#define ARM_FREQ_1250 1250
#define ARM_FREQ_1000 1000
#define ARM_FREQ_600  600
#define ARM_FREQ_400  400
#define ARM_FREQ_200  200

uint32_t iproc_config_armpll_v7(uint32_t mode)
{
    int i_loop  ;
    uint32_t pdiv = 1;
    
#if defined(CONFIG_GREYHOUND)
    if (reg32_read((volatile uint32_t *)IPROC_WRAP_TOP_STRAP_STATUS_1) & 0x01)  /* 50 MHz */
    	  pdiv = 2;
#endif

    // Before PLL locking change the Frequency ID to 2 'default'  

    mb();
    reg32_write((volatile uint32_t *)IHOST_PROC_CLK_WR_ACCESS, 0xA5A501);    // Write KPROC_CLK_MGR_REG_WR_ACCESS = 32'h00A5A501 to enable clk manager access. 
    reg32_write((volatile uint32_t *)IHOST_PROC_CLK_POLICY_FREQ, 0x82020202);   // Select the frequency ID =2 for all policies
    reg32_write((volatile uint32_t *)IHOST_PROC_CLK_POLICY_CTL , (1 << IHOST_PROC_CLK_POLICY_CTL__GO) | ( 1 << IHOST_PROC_CLK_POLICY_CTL__GO_AC)); 
    // Set the GO and GO_AC bit 
    while ((reg32_read((volatile uint32_t *)IHOST_PROC_CLK_POLICY_CTL) & (1 << IHOST_PROC_CLK_POLICY_CTL__GO) ) != 0);   // Wait for Go bit to get clear 

    if (mode ==  ARM_FREQ_1250) {
         // Reset the PLL and post-divider
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMA , 0x0006400 | (pdiv << 24));
         for (i_loop =0 ; i_loop < 5 ; i_loop++) {
         }
         // crystal_clk=25 MHz , MDIV=4 , H_MDIV=2 , PDIV=1 therefore pll_h_clk= (((crystal_clk/pdiv)* ndiv)/h_mdiv) = 1250
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMCTRL5 , (reg32_read((volatile uint32_t *)IHOST_PROC_CLK_PLLARMCTRL5) & 0xffffff00) | 0x2);  
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMC, (reg32_read((volatile uint32_t *)IHOST_PROC_CLK_PLLARMC) & 0xffffff00) | 0x4); 
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMA , 0x0006401 | (pdiv << 24));   
    } else if (mode == ARM_FREQ_1000) {
         // Reset the PLL and post-divider
//         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMA , 0x00005000 | (pdiv << 24));
         for (i_loop =0 ; i_loop < 5 ; i_loop++) ;  // Dummy loop for reste propagation
         // crystal_clk=25 MHz , MDIV=4 , H_MDIV=2 , PDIV=1 therefore pll_h_clk= (((crystal_clk/pdiv)* ndiv)/h_mdiv) = 1000
//         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMCTRL5 , (reg32_read((volatile uint32_t *)IHOST_PROC_CLK_PLLARMCTRL5) & 0xffffff00) | 0x2); 
//         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMC, (reg32_read((volatile uint32_t *)IHOST_PROC_CLK_PLLARMC) & 0xffffff00) | 0x4); 
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMA , 0x0005001 | (pdiv << 24));
    } else if (mode == ARM_FREQ_600) {
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMA , 0x0006000 | (pdiv << 24));
         for (i_loop=0; i_loop < 5; i_loop++) {
         }
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMCTRL5 , (reg32_read((volatile uint32_t *)IHOST_PROC_CLK_PLLARMCTRL5) & 0xffffff00) | 0x4);
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMC, (reg32_read((volatile uint32_t *)IHOST_PROC_CLK_PLLARMC) & 0xffffff00) | 0x4);
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMA , 0x0006001 | (pdiv << 24));
    } else if (mode == ARM_FREQ_400)  { // 400 MHz
         //VCO Frequency should be in between 1568.00/4080.00 MHz, set by vco_range = High
 
         // Reset the PLL and post-divider
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMA, 0x0002000 | (pdiv << 24));
         for (i_loop =0 ; i_loop < 5 ; i_loop++) ;  // Dummy loop for reste propagation
         // crystal_clk=25 MHz , MDIV=8 , H_MDIV=4 , PDIV=1 therefore pll_h_clk= (((crystal_clk/pdiv)* ndiv)/h_mdiv) = 400
         // SBL config is not right - ndiv = 32, pdiv =1, hmdiv needs to be 2!!!
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMCTRL5 , (reg32_read((volatile uint32_t *)IHOST_PROC_CLK_PLLARMCTRL5) & 0xffffff00) | 0x2);  
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMC, (reg32_read((volatile uint32_t *)IHOST_PROC_CLK_PLLARMC) & 0xffffff00) | 0x8);  
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMA , 0x0004001| (pdiv << 24));
    } else if (mode == ARM_FREQ_200)   {
          // 200 MHz
         //VCO Frequency should be in between 1568.00/4080.00 MHz, set by vco_range = High

         // Reset the PLL and post-divider
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMA , 0x0002000 | (pdiv << 24));
         for (i_loop =0 ; i_loop < 5 ; i_loop++) ;  // Dummy loop for reste propagation
         // crystal_clk=25 MHz , MDIV=8 , H_MDIV=4 , PDIV=1 therefore pll_h_clk= (((crystal_clk/pdiv)* ndiv)/h_mdiv) = 200
         // SBL config is not right - ndiv = 32, pdiv =1, hmdiv needs to be 4!!!
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMCTRL5 , (reg32_read((volatile uint32_t *)IHOST_PROC_CLK_PLLARMCTRL5) & 0xffffff00) | 0x4);  
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMC, (reg32_read((volatile uint32_t *)IHOST_PROC_CLK_PLLARMC) & 0xffffff00) | 0x10);  
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMA , 0x0004001 | (pdiv << 24));
    } else if (mode == ARM_FREQ_1500) {
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMA , 0x0007800 | (pdiv << 24));
         for (i_loop=0; i_loop < 5; i_loop++) {
         }
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMCTRL5 , (reg32_read((volatile uint32_t *)IHOST_PROC_CLK_PLLARMCTRL5) & 0xffffff00) | 0x2);
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMC, (reg32_read((volatile uint32_t *)IHOST_PROC_CLK_PLLARMC) & 0xffffff00) | 0x2);
         reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMA , 0x0007801 | (pdiv << 24));
    } else {
         printf("mode is not correct\n");
 
         return(-1);
    }

    while ( !(reg32_read((volatile uint32_t *)IHOST_PROC_CLK_PLLARMA) & (1 <<IHOST_PROC_CLK_PLLARMA__pllarm_lock)) ) {
           // Wait for PLL lock to be set 
           
    }
    reg32_set_bits((volatile uint32_t *)IHOST_PROC_CLK_PLLARMA, 1 << IHOST_PROC_CLK_PLLARMA__pllarm_soft_post_resetb);
    reg32_write((volatile uint32_t *)IHOST_PROC_CLK_POLICY_FREQ, 0x87070707);  // Switch to frequency ID 7 
    reg32_write((volatile uint32_t *)IHOST_PROC_CLK_POLICY_CTL , (1 << IHOST_PROC_CLK_POLICY_CTL__GO) | ( 1 << IHOST_PROC_CLK_POLICY_CTL__GO_AC));  
    // Set the GO and GO_AC bit 
    while ((reg32_read((volatile uint32_t *)IHOST_PROC_CLK_POLICY_CTL) & (1 << IHOST_PROC_CLK_POLICY_CTL__GO) ) != 0) {
          // Wait for Go bit to get clear 
    }
#ifdef CONFIG_SABER2
    /* Disable dynamic clock gating [Bit 1] in addition */
    reg32_write((volatile uint32_t *)IHOST_PROC_CLK_CORE0_CLKGATE ,         0x00000303);
    reg32_write((volatile uint32_t *)IHOST_PROC_CLK_CORE1_CLKGATE ,         0x00000303);
    reg32_write((volatile uint32_t *)IHOST_PROC_CLK_ARM_SWITCH_CLKGATE, 0x00000303);
    reg32_write((volatile uint32_t *)IHOST_PROC_CLK_ARM_PERIPH_CLKGATE, 0x00000303);
    reg32_write((volatile uint32_t *)IHOST_PROC_CLK_APB0_CLKGATE ,          0x00000303);
#else
    reg32_write((volatile uint32_t *)IHOST_PROC_CLK_CORE0_CLKGATE ,         0x00000301);
    reg32_write((volatile uint32_t *)IHOST_PROC_CLK_CORE1_CLKGATE ,         0x00000301);
    reg32_write((volatile uint32_t *)IHOST_PROC_CLK_ARM_SWITCH_CLKGATE, 0x00000301);
    reg32_write((volatile uint32_t *)IHOST_PROC_CLK_ARM_PERIPH_CLKGATE, 0x00000301);
    reg32_write((volatile uint32_t *)IHOST_PROC_CLK_APB0_CLKGATE ,          0x00000303);
#endif
    mb();
    return 0;
}

#endif /* CONFIG_IPROC_P7 */

/*
** 
** 
**
*/
uint32_t iproc_config_armpll(uint32_t clkmhz)
{
   uint32_t status = 0, freqid;
   int i = 0, j = 0;

#if defined(CONFIG_IPROC_P7)
	iproc_config_armpll_v7(clkmhz);
	return status;
#endif

	while(1) {
		if(armpll_clk_tab[i] == clkmhz)
			break;
		if(armpll_clk_tab[i] == 0xffffffff) {
			printf("Error: Requested clock configuration not supported\n");
			status = 1;
			goto armpll_config_done;
		}
		i += 5;
	}

	/* Enable write access */
	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_WR_ACCESS, 0xa5a501);

	if(clkmhz == 25)
		freqid = 0;
	else
		freqid = 2;

	/* Bypass ARM clock and run on sysclk */
		reg32_write((volatile uint32_t *)IHOST_PROC_CLK_POLICY_FREQ,
		1 << IHOST_PROC_CLK_POLICY_FREQ__priv_access_mode |
		freqid << IHOST_PROC_CLK_POLICY_FREQ__policy3_freq_R |
		freqid<< IHOST_PROC_CLK_POLICY_FREQ__policy2_freq_R |
		freqid << IHOST_PROC_CLK_POLICY_FREQ__policy1_freq_R |
		freqid << IHOST_PROC_CLK_POLICY_FREQ__policy0_freq_R);

	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_POLICY_CTL, 
		1 << IHOST_PROC_CLK_POLICY_CTL__GO |
		1 << IHOST_PROC_CLK_POLICY_CTL__GO_AC);

	for(j = 0; j < 0x100000; j++) {
		if(!(reg32_read((volatile uint32_t *)IHOST_PROC_CLK_POLICY_CTL) & (1 << IHOST_PROC_CLK_POLICY_CTL__GO)))
			break;
	}

	if(j == 0x100000) {
		printf("CCU polling timedout\n");
		status = 1;
		goto armpll_config_done;
	}

	if(clkmhz == 25 || clkmhz == 100) {
		status = 0;
		goto armpll_config_done;
	}

	/* Now it is safe to program the PLL */
	reg32_write_masked((volatile uint32_t *)IHOST_PROC_CLK_PLLARMB, 
		(1 << IHOST_PROC_CLK_PLLARMB__pllarm_ndiv_frac_WIDTH) -1, 
		armpll_clk_tab[i + 2] << IHOST_PROC_CLK_PLLARMB__pllarm_ndiv_frac_R);	

	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMA, 
			1 << IHOST_PROC_CLK_PLLARMA__pllarm_lock |
			armpll_clk_tab[i+1] << IHOST_PROC_CLK_PLLARMA__pllarm_ndiv_int_R |  
			armpll_clk_tab[i+3] << IHOST_PROC_CLK_PLLARMA__pllarm_pdiv_R |
			1 << IHOST_PROC_CLK_PLLARMA__pllarm_soft_resetb);

	for(j = 0; j < 0x100000; j++) {
		if(reg32_read((volatile uint32_t *)IHOST_PROC_CLK_PLLARMA) & (1 << IHOST_PROC_CLK_PLLARMA__pllarm_lock))
			break;
	}
	if(j == 0x100000) {
		printf("ARM PLL lock failed\n");
		status = 1;
		goto armpll_config_done;
	}

	reg32_set_bits((volatile uint32_t *)IHOST_PROC_CLK_PLLARMA, 1 << IHOST_PROC_CLK_PLLARMA__pllarm_soft_post_resetb);
	/* Set the policy */
	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_POLICY_FREQ,
		1 << IHOST_PROC_CLK_POLICY_FREQ__priv_access_mode |
		armpll_clk_tab[i+4] << IHOST_PROC_CLK_POLICY_FREQ__policy3_freq_R |
		armpll_clk_tab[i+4] << IHOST_PROC_CLK_POLICY_FREQ__policy2_freq_R |
		armpll_clk_tab[i+4] << IHOST_PROC_CLK_POLICY_FREQ__policy1_freq_R |
		armpll_clk_tab[i+4] << IHOST_PROC_CLK_POLICY_FREQ__policy0_freq_R);

	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_CORE0_CLKGATE, 0x00000303); 
	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_CORE1_CLKGATE, 0x00000303); 
	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_ARM_SWITCH_CLKGATE, 0x00000303); 
	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_ARM_PERIPH_CLKGATE, 0x00000303); 
	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_APB0_CLKGATE, 0x00000303); 

	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_POLICY_CTL, 
		1 << IHOST_PROC_CLK_POLICY_CTL__GO |
		1 << IHOST_PROC_CLK_POLICY_CTL__GO_AC);

	for(j = 0; j < 0x100000; j++) {
		if(!(reg32_read((volatile uint32_t *)IHOST_PROC_CLK_POLICY_CTL) & (1 << IHOST_PROC_CLK_POLICY_CTL__GO)))
			break;
	}

	if(j == 0x100000) {
		printf("CCU polling timedout\n");
		status = 1;
		goto armpll_config_done;
	}

	status = 0;
armpll_config_done:
	/* Disable access to PLL registers */
	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_WR_ACCESS, 0);

	return(status);
}
