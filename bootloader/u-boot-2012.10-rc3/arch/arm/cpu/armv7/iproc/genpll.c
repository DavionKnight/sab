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


#if defined(CONFIG_HURRICANE2)
unsigned int genpll_clk_tab[] = {
	/*  mode,	ndiv,	MDIV
						0	1	2	3	4	5 */

		0,		80,		10,	0,	0,	5,	8,	0,		/* 400 MHz AXI */
		2,		80,		10,	0,	0,	7,	8,	0,		/* 285 MHz AXI */
		3,		80,		10,	0,	0,	8,	8,	0,		/* 250 MHz AXI */	
		4,		80,		10,	0,	0,	10,	8,	0,		/* 200 MHz AXI */	
		5,		80,		10,	0,	0,	20,	8,	0,		/* 100 MHz AXI */	
		0xffffffff
};
#endif
#if defined(CONFIG_HELIX4)
unsigned int genpll_clk_tab[] = {
	/*  mode,	ndiv,	MDIV
						0	1	2	3	4	5 */
		0,		60,		12,	0,	50,	3,	6,	0,		/* 500 MHz AXI(Ch3), ch0:125, ch2:30, ch4:250 */
		1,		80,		16,	0,	50,	5,	8,	0,		/* 400 MHz AXI */
		2,		60,		12,	0,	50,	5,	6,	0,		/* 300 MHz AXI */
		3,		60,		12,	0,	50,	6,	6,	0,		/* 250 MHz AXI */	
		4,		80,		16,	0,	50,	10,	8,	0,		/* 200 MHz AXI */	
		5,		60,		12,	0,	50,	15,	6,	0,		/* 100 MHz AXI */	
		0xffffffff
};
#endif
#if (defined(CONFIG_GREYHOUND) || defined(CONFIG_HURRICANE3))
unsigned int genpll_clk_tab[] = {
	/*  mode,	ndiv,	MDIV
						0	1	2	3	4	5 */
		0,		96,		12,	0,	80,	6,	5,	40,		/* 400 MHz AXI */
		1,		96,		12,	0,	80,	12,	5,	40,		/* 200 MHz AXI */
		0xffffffff
};
#endif

#if (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2) \ 
    || defined(CONFIG_GREYHOUND) || defined(CONFIG_HURRICANE3))

uint32_t iproc_config_genpll(uint32_t mode)
{
   volatile uint32_t addr;
   volatile uint32_t rdata;
   volatile uint32_t wdata;
   volatile uint32_t lock;
   int i = 0;

	while(1) {
		if(genpll_clk_tab[i] == mode)
			break;
		if(genpll_clk_tab[i] == 0xffffffff) {
			return(1);
		}
		i += 8;
	}

   // Clear Load_en Channel3 & Channel4
   rdata = reg32_read((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL3);
   rdata &= 0xffc0ffff;
   reg32_write((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL3,rdata);

	// Write fast_lock =1
   rdata = reg32_read((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL0);
   rdata |= (1<<IPROC_WRAP_GEN_PLL_CTRL0__FAST_LOCK);
   reg32_write(IPROC_WRAP_GEN_PLL_CTRL0,rdata);

   // Write NDIV
   rdata = reg32_read((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL1);
   rdata &= 0xfffffc00;
   rdata |= (genpll_clk_tab[i+1] << IPROC_WRAP_GEN_PLL_CTRL1__NDIV_INT_R);
   reg32_write((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL1,rdata);

   // Write MDIV
   rdata = reg32_read((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL2);
   rdata &= 0xff0000ff;
   rdata |= ((genpll_clk_tab[i+5] <<IPROC_WRAP_GEN_PLL_CTRL2__CH3_MDIV_R)|
            (genpll_clk_tab[i+6]<<IPROC_WRAP_GEN_PLL_CTRL2__CH4_MDIV_R));
   reg32_write((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL2,rdata);

   // Write PLL_LOAD
   rdata = reg32_read((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL3);
   rdata |= (1<<IPROC_WRAP_GEN_PLL_CTRL3__SW_TO_GEN_PLL_LOAD);
   reg32_write((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL3,rdata);

   // Load Channel3 & Channel4
   rdata &= 0xffc0ffff;
   rdata |= (0x18<<IPROC_WRAP_GEN_PLL_CTRL3__LOAD_EN_CH_R);
   reg32_write((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL3,rdata);

   // Wait for IPROC_WWRAP GENPLL lock
   do{
     rdata = reg32_read((volatile uint32_t *)IPROC_WRAP_GEN_PLL_STATUS);
     lock = ((rdata>>IPROC_WRAP_GEN_PLL_STATUS__GEN_PLL_LOCK)&1);
   }while(!lock);

	return(0);
}

uint32_t iproc_get_axi_clk(uint32_t refclk)
{
#if defined(CONFIG_HURRICANE2_EMULATION) || defined(CONFIG_IPROC_EMULATION)
	return(IPROC_AXI_CLK); /* return the emulator clock defined in configuration file */
#else
   uint32_t ndiv, mdiv, pdiv;

   ndiv = (reg32_read((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL1) >> IPROC_WRAP_GEN_PLL_CTRL1__NDIV_INT_R) &
			((1 << IPROC_WRAP_GEN_PLL_CTRL1__NDIV_INT_WIDTH) -1);
   if(ndiv == 0)
	   ndiv = 1 << IPROC_WRAP_GEN_PLL_CTRL1__NDIV_INT_WIDTH;

   pdiv = (reg32_read((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL1) >> IPROC_WRAP_GEN_PLL_CTRL1__PDIV_R) &
			((1 << IPROC_WRAP_GEN_PLL_CTRL1__PDIV_WIDTH) -1);
   if(pdiv == 0)
	   pdiv = 1 << IPROC_WRAP_GEN_PLL_CTRL1__PDIV_WIDTH;

   mdiv = (reg32_read((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL2) >> IPROC_WRAP_GEN_PLL_CTRL2__CH3_MDIV_R) &
			((1 << IPROC_WRAP_GEN_PLL_CTRL2__CH3_MDIV_WIDTH) -1);
   if(mdiv == 0)
	   mdiv = 1 << IPROC_WRAP_GEN_PLL_CTRL2__CH3_MDIV_WIDTH;

	return refclk * ndiv / pdiv / mdiv;
#endif
}
#endif

#if defined(CONFIG_KATANA2)
uint32_t iproc_get_axi_clk(uint32_t refclk)
{
#if defined(CONFIG_KATANA2_EMULATION)
	return(IPROC_AXI_CLK); /* return the emulator clock defined in configuration file */
#else
	return(495000000);
#endif
}
#endif

#if defined(CONFIG_SABER2)
uint32_t iproc_get_axi_clk(uint32_t refclk)
{
#if defined(CONFIG_IPROC_EMULATION)
    return (IPROC_AXI_CLK); /* return clock defined in configuration file */
#else
    uint32_t ndiv, mdiv, pdiv;

    ndiv = (reg32_read((volatile uint32_t *)IPROC_WRAP_IPROC_PLL_CTRL_3) >> IPROC_WRAP_IPROC_PLL_CTRL_3__NDIV_INT_R) &
			((1 << IPROC_WRAP_IPROC_PLL_CTRL_3__NDIV_INT_WIDTH) -1);

    mdiv = (reg32_read((volatile uint32_t *)IPROC_WRAP_IPROC_PLL_CTRL_5) >> IPROC_WRAP_IPROC_PLL_CTRL_5__CH1_MDIV_R) &
			((1 << IPROC_WRAP_IPROC_PLL_CTRL_5__CH1_MDIV_WIDTH) -1);

    pdiv = (reg32_read((volatile uint32_t *)IPROC_WRAP_IPROC_PLL_CTRL_1) >> IPROC_WRAP_IPROC_PLL_CTRL_1__PDIV_R) &
			((1 << IPROC_WRAP_IPROC_PLL_CTRL_1__PDIV_WIDTH) -1);

    return refclk * ndiv / pdiv / mdiv;
#endif
}
#endif

uint32_t iproc_get_uart_clk(uint32_t uart)
{
	uint32_t uartclk, uartclkovr, uartclksel; 
	
#if defined(CONFIG_SABER2)
    uartclk = iproc_get_axi_clk(CONFIG_IPROC_PLL_REF_CLK) / 4; /* APB clock */
#else
	uartclk = iproc_get_axi_clk(CONFIG_SYS_REF_CLK) / 4; /* APB clock */
#endif

#if (!defined(CONFIG_GREYHOUND) && !defined(CONFIG_SABER2) && \
	!defined(CONFIG_HURRICANE3))
	if (uart < 2) {
		/* CCA UART */
		uartclkovr = (reg32_read((volatile uint32_t *)ChipcommonA_CoreCtrl) >> ChipcommonA_CoreCtrl__UARTClkOvr) & 0x01;
		uartclksel = (reg32_read((volatile uint32_t *)APBW_IDM_IDM_IO_CONTROL_DIRECT) >> APBW_IDM_IDM_IO_CONTROL_DIRECT__UARTClkSel) & 0x01;
		if(!uartclkovr) {
			if(uartclksel) {
				uartclk /= ((reg32_read((volatile uint32_t *)ChipcommonA_ClkDiv) >> ChipcommonA_ClkDiv__UartClkDiv_R) & 
					        ((1 << ChipcommonA_ClkDiv__UartClkDiv_WIDTH) - 1));
			}
			else{
				uartclk = CONFIG_SYS_REF_CLK; /* Reference clock */
			}
		}
	}
#endif /* !CONFIG_GREYHOUND && !CONFIG_SABER2 */
    
	return(uartclk);
}
