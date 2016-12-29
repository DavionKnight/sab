/*
 * serial_synopsys.c
 *
 *  Created on: 09-Apr-2013
 *      Author: Chandra Mohan C 
 *      Email : chandra@broadcom.com
 */

/*
 * (C) Copyright 2000
 * Rob Taylor, Flying Pig Systems. robt@flyingpig.com.
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

#include <common.h>
#include <linux/compiler.h>

#include "synopsysUart.h"
#include <asm/arch/iproc_regs.h>
#include <asm/arch/iproc.h>

#if defined (CONFIG_SERIAL_MULTI)
#include <serial.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

/* See comment above early_access_env_vars() about the value */
#define EARLY_ACC_BUF_SIZE 512

#if !defined(CONFIG_CONS_INDEX)
	#error	"No console port specified."
#elif (CONFIG_CONS_INDEX < 0) || (CONFIG_CONS_INDEX > 4)
	#error	"Invalid console port value."
#endif

#if CONFIG_CONS_INDEX == 0 && !defined(CONFIG_SYS_SYNOPSYS_COM0)
#error	"Console port 0 defined but not configured."
#elif CONFIG_CONS_INDEX == 1 && !defined(CONFIG_SYS_SYNOPSYS_COM1)
#error	"Console port 1 defined but not configured."
#elif CONFIG_CONS_INDEX == 2 && !defined(CONFIG_SYS_SYNOPSYS_COM2)
#error	"Console port 2 defined but not configured."
#elif CONFIG_CONS_INDEX == 3 && !defined(CONFIG_SYS_SYNOPSYS_COM3)
#error	"Console port 3 defined but not configured."
#elif CONFIG_CONS_INDEX == 4 && !defined(CONFIG_SYS_SYNOPSYS_COM4)
#error	"Console port 4 defined but not configured."
#endif

static SynopsysUart_t serial_ports[5] = {
#ifdef CONFIG_SYS_SYNOPSYS_COM0
		(SynopsysUart_t)CONFIG_SYS_SYNOPSYS_COM0,
#else
		NULL,
#endif
#ifdef CONFIG_SYS_SYNOPSYS_COM1
		(SynopsysUart_t)CONFIG_SYS_SYNOPSYS_COM1,
#else
		NULL,
#endif
#ifdef CONFIG_SYS_SYNOPSYS_COM2
		(SynopsysUart_t)CONFIG_SYS_SYNOPSYS_COM2,
#else
		NULL,
#endif
#ifdef CONFIG_SYS_SYNOPSYS_COM3
		(SynopsysUart_t)CONFIG_SYS_SYNOPSYS_COM3,
#else
		NULL,
#endif
#ifdef CONFIG_SYS_SYNOPSYS_COM4
		(SynopsysUart_t)CONFIG_SYS_SYNOPSYS_COM4,
#else
		NULL,
#endif
		};

#if defined(CONFIG_SERIAL_MULTI)

/* Multi serial device functions */
#define DECLARE_ESERIAL_FUNCTIONS(port) \
static int  eserial##port##_init (void) {\
    int clock_divisor; \
    clock_divisor = calc_divisor(); \
    synopsysUart_init(serial_ports[port], clock_divisor); \
    return(0);}\
static void eserial##port##_setbrg (void) {\
    _serial_setbrg(port);}\
static int  eserial##port##_getc (void) {\
    return _serial_getc(port);}\
static int  eserial##port##_tstc (void) {\
    return _serial_tstc(port);}\
static void eserial##port##_putc (const char c) {\
    _serial_putc(c, port);}\
static void eserial##port##_puts (const char *s) {\
    _serial_puts(s, port);} \
static void eserial##port##_loop(int en) { \
    en ? serial_enter_loopback(port):serial_exit_loopback(port);} 

/* Serial device descriptor */
#if (CONFIG_POST & CONFIG_SYS_POST_UART)
#define LOOP(x) x
#else
#define  LOOP(x)
#endif

#define INIT_ESERIAL_STRUCTURE(port) {\
	"serial"#port,\
	eserial##port##_init,\
	NULL,\
	eserial##port##_setbrg,\
	eserial##port##_getc,\
	eserial##port##_tstc,\
	eserial##port##_putc,\
	eserial##port##_puts,\
    LOOP(eserial##port##_loop) \
	}

//#endif
#endif /* CONFIG_SERIAL_MULTI */

static int calc_divisor(void) {
#ifdef CONFIG_SYS_SYNOPSYS_CLK_DIV
    return CONFIG_SYS_SYNOPSYS_CLK_DIV;
#else
	return (CONFIG_SYS_UART_CLK / (gd->baudrate * 16));
#endif
}

#if !defined(CONFIG_SERIAL_MULTI)
extern void iproc_uart_iomux(int op);
int serial_init(void) {
	int clock_divisor;
 
    iproc_uart_iomux(1);
	clock_divisor = calc_divisor();

#ifdef CONFIG_SYS_SYNOPSYS_COM0
	synopsysUart_init(serial_ports[0], clock_divisor);
#endif

#ifdef CONFIG_SYS_SYNOPSYS_COM1
	synopsysUart_init(serial_ports[1], clock_divisor);
#endif

#ifdef CONFIG_SYS_SYNOPSYS_COM2
	synopsysUart_init(serial_ports[2], clock_divisor);
#endif

#ifdef CONFIG_SYS_SYNOPSYS_COM3
	synopsysUart_init(serial_ports[3], clock_divisor);
#endif
#ifdef CONFIG_SYS_SYNOPSYS_COM4
	synopsysUart_init(serial_ports[4], clock_divisor);
#endif

	serial_printf("\nINFO-Cygnus: uart-%d with clk_div @ 0x%x = %d", CONFIG_CONS_INDEX,
	        CONFIG_PHYS_SDRAM_1, *((unsigned short*)CONFIG_PHYS_SDRAM_1)& 0xffff);

	return (0);
}
#endif

void
_serial_putc(const char c, const int port)
{
	if (c == '\n')
	synopsysUart_putc('\r', serial_ports[port]);

	synopsysUart_putc(c, serial_ports[port]);
}

#if !defined(CONFIG_SERIAL_MULTI)
void serial_putc(const char c) {
	if (c == '\n')
		synopsysUart_putc('\r', serial_ports[CONFIG_CONS_INDEX]);

	synopsysUart_putc(c, serial_ports[CONFIG_CONS_INDEX]);
}
#endif

void
_serial_putc_raw(const char c, const int port)
{
	synopsysUart_putc(c, serial_ports[port]);
}

#if !defined(CONFIG_SERIAL_MULTI)
void serial_putc_raw(const char c) {
	_serial_putc_raw(c, CONFIG_CONS_INDEX);
}
#endif
void
_serial_puts(const char *s, const int port)
{
	while (*s)
	{
		_serial_putc(*s++, port);
	}
}
#if !defined(CONFIG_SERIAL_MULTI)
void serial_puts(const char *s) {
	while (*s) {
		_serial_putc(*s++, CONFIG_CONS_INDEX);
	}
}
#endif
int
_serial_getc(const int port)
{
	return synopsysUart_getc(serial_ports[port]);
}

#if !defined(CONFIG_SERIAL_MULTI)
int serial_getc(void) {
	return _serial_getc(CONFIG_CONS_INDEX);
}
#endif
int
_serial_tstc(const int port)
{
	return synopsysUart_tstc(serial_ports[port]);
}

#if !defined(CONFIG_SERIAL_MULTI)
int serial_tstc(void) {
	return _serial_tstc(CONFIG_CONS_INDEX);
}
#endif
void
_serial_setbrg(const int port)
{
	int clock_divisor;

	clock_divisor = calc_divisor();
	synopsysUart_reinit(serial_ports[port], clock_divisor);
}

#if !defined(CONFIG_SERIAL_MULTI)
void serial_setbrg(void) {
	int clock_divisor;

	clock_divisor = calc_divisor();
	synopsysUart_reinit(serial_ports[CONFIG_CONS_INDEX], clock_divisor);
}
#endif

void serial_enter_loopback(const int port) {

	return synopsysUart_enterLoopback((SynopsysUart_t) serial_ports[port]);

}

void serial_exit_loopback(const int port) {

	return synopsysUart_exitLoopback((SynopsysUart_t) serial_ports[port]);
}

#ifdef IPROC_BOARD_DIAGS
void
_serial_enter_loopback( const int port )
{
      return serial_enter_loopback(port);
}
void
_serial_exit_loopback( const int port )
{
      return serial_exit_loopback(port);
}
#endif

/* Find sub string 't' in string 's'. If found, return index within 's' pointing
 * to the last matching character. If not found, return -1
 */
static int find_str_indx(char *s, size_t len, char *t)
{
    int i, j;
    
    i = j = 0;

    do {

        j = 0;

        while (s[i] == t[j]) {
            i++;
            j++;
        }

        if (t[j] == '\0') {
            /* var found */
            return(i - 1);
        } 

        i++;
    } while (i < len); /* (s[i] != '\0'); */

    return -1;
}

/*
 * Note about EARLY_ACC_BUF_SIZE used in this function. Since this function
 * is called very early in the init sequence, the amount of stack is limited,
 * so we limit the amount of data read from flash to EARLY_ACC_BUF_SIZE, that
 * is, we do not read the entire sector from flash (on helix4 its 64K, as per
 * CONFIG_ENV_SIZE). Since this function is written to get baudrate, which is
 * always towards beginning of uboot env list, reading 1st 1K (or even less
 * amount) is sufficient. If this function needs to be used to read some other
 * env var, the 'env_ptr' can be adjusted by doing some approximation, depending
 * on the name of env var (uboot stores env vars in sorted order), so that
 * a large buffer size need not be allocated simply to find a env var
 */
int early_access_env_vars(void)
{
#ifdef CONFIG_ENV_IS_NOWHERE
    return -1;
#else /* CONFIG_ENV_IS_NOWHERE */
    volatile uint8_t *env_ptr;
    int i;
    char buf[EARLY_ACC_BUF_SIZE];
    unsigned long baud;
    unsigned long bauds[] = CONFIG_SYS_BAUDRATE_TABLE;
    int baud_arr_size = sizeof(bauds) / sizeof(bauds[0]);


    /* Note CONFIG_ENV_OFFSET is different on nsp(0xa0000) and hx4(0xc0000) */
#if defined(CONFIG_ENV_IS_IN_NAND) && CONFIG_ENV_IS_IN_NAND
    env_ptr = (volatile uint8_t *)(IPROC_NAND_MEM_BASE + CONFIG_ENV_OFFSET);
#elif defined (CONFIG_ENV_IS_IN_FLASH) 
    env_ptr = (volatile uint8_t *)(IPROC_NOR_MEM_BASE + CONFIG_ENV_OFFSET);
#else
    env_ptr = (volatile uint8_t *)(IPROC_QSPI_MEM_BASE + CONFIG_ENV_OFFSET);
#endif

    for (i = 0; i < EARLY_ACC_BUF_SIZE; i++) {
        buf[i] = *(env_ptr + i);
    }

    baud = 0;
    i = find_str_indx(buf, sizeof(buf),  "baudrate=");
    if (i != -1) {
        /* Convert to integer value */
        while ((buf[i + 1] >= '0') && (buf[i + 1] <= '9')) {

            baud = baud * 10 + (buf[i + 1] - '0');
            i++;
        }

        /* Verify that the baudrate is a valid and a supported value */
        for (i = 0; i < baud_arr_size; i++) {
            if (baud == bauds[i]) {
                break;
            }
        }

        if (i < baud_arr_size) {
            /* Baud rate value is valid */
            gd->baudrate = baud;
        } else {
            return -1;
        }

    } else {

        return -1;
    }

        return 0;
#endif /* CONFIG_ENV_IS_NOWHERE */
}

#ifdef IPROC_BOARD_DIAGS
//Initialize the Console 3, UART 2 to
//Communicate to the console for the 8051
//which controls the strap pin setting
int serial_init_com8051(void)
{
    //UART clk is 125Mhz APB Clk
    // 19200 = 125Mhz / ( 16 * clock_divisor )
    // to do - for CYGNUS
	return 0;
}
#endif


#if defined(CONFIG_SERIAL_MULTI)
DECLARE_ESERIAL_FUNCTIONS(0);
struct serial_device synopsys_serial0_device =
	INIT_ESERIAL_STRUCTURE(0);
DECLARE_ESERIAL_FUNCTIONS(1);
struct serial_device synopsys_serial1_device =
	INIT_ESERIAL_STRUCTURE(1);
DECLARE_ESERIAL_FUNCTIONS(2);
struct serial_device synopsys_serial2_device =
	INIT_ESERIAL_STRUCTURE(2);
DECLARE_ESERIAL_FUNCTIONS(3);
struct serial_device synopsys_serial3_device =
	INIT_ESERIAL_STRUCTURE(3);
DECLARE_ESERIAL_FUNCTIONS(4);
struct serial_device synopsys_serial4_device =
	INIT_ESERIAL_STRUCTURE(4);

__weak struct serial_device *default_serial_console(void)
{
#if CONFIG_CONS_INDEX == 0
	return &synopsys_serial0_device;
#elif CONFIG_CONS_INDEX == 1
	return &synopsys_serial1_device;
#elif CONFIG_CONS_INDEX == 2
	return &synopsys_serial2_device;
#elif CONFIG_CONS_INDEX == 3
	return &synopsys_serial3_device;
#elif CONFIG_CONS_INDEX == 4
	return &synopsys_serial4_device;
#else
#error "Bad CONFIG_CONS_INDEX."
#endif
}
#endif /* CONFIG_SERIAL_MULTI */


