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
 *
 * synopsys.c
 *
 *  Created on: 09-Apr-2013
 *      Author: Chandra Mohan C 
 *      Email : chandra@broadcom.com
 */


#include <config.h>
#include <watchdog.h>
#include <linux/types.h>
#include <asm/io.h>

#include "synopsysUart.h"

#define serial_out(x, y)	writel(x, y)
#define serial_in(y)		readl(y)

#ifndef CONFIG_SYS_SYNOPSYS_UART_IER
#define CONFIG_SYS_SYNOPSYS_UART_IER  0x00
#endif /* CONFIG_SYS_SysnopsysUart_IER */

#define UART_LCRVAL UART_LCR_8N1		/* 8 data, 1 stop, no parity */
#define UART_MCRVAL (UART_MCR_DTR | \
		     UART_MCR_RTS)		/* RTS/DTR */
#define UART_FCRVAL (UART_FCR_FIFO_EN |	\
		     UART_FCR_RXSR |	\
		     UART_FCR_TXSR)		/* Clear & enable FIFOs */

void synopsysUart_init(SynopsysUart_t com_port, int baud_divisor)
{
	serial_out(CONFIG_SYS_SYNOPSYS_UART_IER, &com_port->cd2.ier);
	serial_out(UART_LCR_BKSE | UART_LCRVAL, (ulong)&com_port->lcr);
	serial_out(0, &com_port->cd1.dll);
	serial_out(0, &com_port->cd2.dlm);
	serial_out(UART_LCRVAL, &com_port->lcr);
	serial_out(UART_MCRVAL, &com_port->mcr);
	serial_out(UART_FCRVAL, &com_port->cd3.fcr);
	serial_out(UART_LCR_BKSE | UART_LCRVAL, &com_port->lcr);
	serial_out(baud_divisor & 0xff, &com_port->cd1.dll);
	serial_out((baud_divisor >> 8) & 0xff, &com_port->cd2.dlm);
	serial_out(UART_LCRVAL, &com_port->lcr);
}

#ifndef CONFIG_SYNOPSYS_UART_MIN_FUNCTIONS
void synopsysUart_reinit(SynopsysUart_t com_port, int baud_divisor)
{
	serial_out(CONFIG_SYS_SYNOPSYS_UART_IER, &com_port->cd2.ier);
	serial_out(UART_LCR_BKSE | UART_LCRVAL, &com_port->lcr);
	serial_out(0, &com_port->cd1.dll);
	serial_out(0, &com_port->cd2.dlm);
	serial_out(UART_LCRVAL, &com_port->lcr);
	serial_out(UART_MCRVAL, &com_port->mcr);
	serial_out(UART_FCRVAL, &com_port->cd3.fcr);
	serial_out(UART_LCR_BKSE, &com_port->lcr);
	serial_out(baud_divisor & 0xff, &com_port->cd1.dll);
	serial_out((baud_divisor >> 8) & 0xff, &com_port->cd2.dlm);
	serial_out(UART_LCRVAL, &com_port->lcr);
}
#endif /* CONFIG_SYNOPSYS_UART_MIN_FUNCTIONS */

void synopsysUart_putc(const char c, SynopsysUart_t com_port)
{
    int i = 0;
    int loop = 0;
    if  (serial_in(&com_port->mcr) & UART_MCR_LOOPBACK )
    {
        loop = 1;
    }
	while ((serial_in(&com_port->lsr) & UART_LSR_THRE) == 0)
    {
            i++;
            if (i > 1000)
            {
                synopsysUart_reinit(com_port, CONFIG_SYS_SYNOPSYS_CLK_DIV);
                if (loop)
                   synopsysUart_enterLoopback(com_port);
                break;
            }            
    }

	serial_out(c, &com_port->cd1.thr);

	/*
	 * Call watchdog_reset() upon newline. This is done here in putc
	 * since the environment code uses a single puts() to print the complete
	 * environment upon "printenv". So we can't put this watchdog call
	 * in puts().
	 */
	if (c == '\n')
		WATCHDOG_RESET();
}

#ifndef CONFIG_SYNOPSYS_UART_MIN_FUNCTIONS
char synopsysUart_getc(SynopsysUart_t com_port)
{
	while ((serial_in(&com_port->lsr) & UART_LSR_DR) == 0);

	return serial_in(&com_port->cd1.rbr);
}

int synopsysUart_tstc(SynopsysUart_t com_port)
{
	return (serial_in(&com_port->lsr) & UART_LSR_DR) != 0;
}

void synopsysUart_enterLoopback(SynopsysUart_t com_port)
{
	//serial_out((serial_in(&com_port->mcr) |UART_MCR_LOOPBACK ), &com_port->mcr);
    int  baud_divisor;

    baud_divisor = CONFIG_SYS_SYNOPSYS_CLK_DIV;
	serial_out(CONFIG_SYS_SYNOPSYS_UART_IER, &com_port->cd2.ier);
	serial_out(UART_LCR_BKSE | UART_LCRVAL, &com_port->lcr);
	serial_out(0, &com_port->cd1.dll);
	serial_out(0, &com_port->cd2.dlm);
	serial_out(UART_LCRVAL, &com_port->lcr);
	serial_out(UART_MCRVAL | UART_MCR_LOOPBACK, &com_port->mcr);
	serial_out(UART_FCRVAL, &com_port->cd3.fcr);
	serial_out(UART_LCR_BKSE, &com_port->lcr);
	serial_out(baud_divisor & 0xff, &com_port->cd1.dll);
	serial_out((baud_divisor >> 8) & 0xff, &com_port->cd2.dlm);
	serial_out(UART_LCRVAL, &com_port->lcr);
}

void synopsysUart_exitLoopback(SynopsysUart_t com_port)
{
	//serial_out((serial_in(&com_port->mcr) & (~UART_MCR_LOOPBACK)) ,&com_port->mcr);
	//serial_out(UART_FCRVAL, &com_port->cd3.fcr);
    synopsysUart_reinit(com_port, CONFIG_SYS_SYNOPSYS_CLK_DIV);
}

#endif /* CONFIG_SYNOPSYS_UART_MIN_FUNCTIONS */
