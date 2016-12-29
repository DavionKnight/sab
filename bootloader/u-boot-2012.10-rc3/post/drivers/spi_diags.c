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
#include <spi_flash.h>
#include <malloc.h>

#if CONFIG_POST & CONFIG_SYS_POST_SPI

#include "asm/arch/socregs.h"
#include <pl022_spi.h>

#define ADC_TSCRegCtl2           0x180a6004
#define ADC_TS_Interrupt_Mask    0x180a600c
#define ADC_TS_Interrupt_Status  0x180a6010
#define ADC_TS_Analog_Control    0x180a601c
#define ADC_TS_Aux_Data          0x180a6020
#define ADC_TS_Interrupt_Status  0x180a6010

#define TSCRegCtl2_con_en        (1<<16)
#define TSCRegCtl2_ldo_pwrdn     (1<<5)
#define TSCRegCtl2_adc_pwrdn     (1<<4)
#define TSCRegCtl2_bgp_pwrdn     (1<<3)
#define TSCRegCtl2_ts_pwrdn      (1<<2)

#define auxin_data_available     (1<<3)

#define ADC_READ(x)  *((volatile unsigned int *)x)
#define ADC_WRITE(v,x) *((volatile unsigned int *)x) = v

int adc_aux_channel_read(int channel)
{
    unsigned int v;
    unsigned int cs;
    int ret_value = -1;
    unsigned int read_cnt = 0x100000;
    unsigned int wait_cnt = 100000;

    /*Enable CLK gating*/
    v = ADC_READ(0x180aa024);
    v |= 1<<9;
    ADC_WRITE(v, 0x180aa024);

    /*Reset ADC*/
    v = ADC_READ(0x180aa020);  
    v = v & ~(1 << 9);  
    ADC_WRITE(v, 0x180aa020);
    v = ADC_READ(0x180aa020);
    v = v | (1 << 9);  
    ADC_WRITE(v, 0x180aa020);
    while(--wait_cnt) ;

    /*Set channel*/
    cs = ADC_READ(ADC_TS_Analog_Control);
    cs &= ~(7<<11);
    cs |= channel<<11;
    ADC_WRITE(cs, ADC_TS_Analog_Control);  /* Set channel*/

    ADC_WRITE(0, ADC_TS_Interrupt_Mask);   /* Disable all interrupt */

    /*Clear interrupt status*/
    v = ADC_READ(ADC_TS_Interrupt_Status);
    ADC_WRITE(v, ADC_TS_Interrupt_Status); /*Write 1 to clear interrupt status*/
    v = ADC_READ(ADC_TS_Interrupt_Status);

    v = ADC_READ(ADC_TSCRegCtl2);
    v &= ~(TSCRegCtl2_ldo_pwrdn|TSCRegCtl2_adc_pwrdn|TSCRegCtl2_bgp_pwrdn); /* Power up LDO/ADC */
    v |= TSCRegCtl2_con_en;
    ADC_WRITE(v, ADC_TSCRegCtl2);

    v = ADC_READ(ADC_TS_Interrupt_Status);
    while(!(v&auxin_data_available)) /* At most check 0x10000 times, to check whether AUXIN data is coming */
    {
        v = ADC_READ(ADC_TS_Interrupt_Status);
        if(--read_cnt == 0)
            break;
    }

    if(!read_cnt)
        return ret_value;  /*IO error, read failed*/

    ret_value = ADC_READ(ADC_TS_Aux_Data); /*Read the sample data*/
    ret_value &= 0xFFFF;

    return ret_value;
}


/* Set Max5134 pin voltage, 16bits resolution */
void ssp_dac_write(struct spi_slave *slave, uint8_t pin, uint16_t val) {
    uint8_t dout[3];

    dout[0] = (0x3 << 4) + (1 << pin); /* command: write through */
    dout[1] = val >> 8; /* high byte */
    dout[2] = val & 0xff; /* low byte */

    pl022_spi_claim_bus(slave);
    pl022_spi_xfer(slave, 24, dout, NULL, SPI_XFER_BEGIN | SPI_XFER_END);
    pl022_spi_release_bus(slave);
}

/*
 * spi dac test
 * support Cygnus SVK spi0 & spi1 which connected to max5134
 */
int ssp_dac_post_test(uint8_t spi) {
    struct spi_slave *slave;
    uint32_t gpio_adc[4], voltage;
    int i;
    int ret = -1;

    /* configure spi port */
    /* setup 4 dac output to 0, 33%, 66%, 100% */
    /* read adc gpio, adc0 < adc1 < adc2, < adc3 */
    /* setup 4 dac output to 100%, 66%, 33%, 0% */
    /* read adc gpio, adc0 > adc1 > adc2 > adc3 */

    switch (spi) {
    case 0:
        post_log(
                "\n Test SPI0 -> DAC -> ADC 0-3. Please turn switch SW9002-5 to off \n");
        break;
    case 1:
        post_log(
                "\n Test SPI1 -> DAC -> ADC 4-7. Please turn switch SW9002-6,7,8 to off \n");
        break;
    default:
        post_log("\n invalid invocation to SPI_DAC test\n");
        return ret;
        break;
    }

    post_log(
            "\n INFO: MAX5134 DAC supports 30MHz max baudrate on its spi interface \n");
    post_log(
            "\n Configuring SPI for Motorola mode, 8-bit data, Master, SPO=0, SPH=1, and for 100Kbps baud \n");
    /* Max5134 requires 3 bytes a time, so use SPH=1 to keep CS continues low, mode 1 */
    slave = pl022_spi_setup_slave(CONFIG_PL022_SPI_BUS, spi, 5000, SPI_MODE_1);
    if (!slave) {
        post_log("\n PL022 SPI%d controller init failed\n", spi);
        return ret;
    }

    post_log("\n Set 4 DAC voltage \n");
    /* setup 4 dac output to 0, 33%, 66%, 100% */
    /* read adc gpio, adc0 < adc1 < adc2, < adc3 */
    for (i = 0; i <= 3; i++) {
        voltage = 0xb000 / 3 * i + 0x1000;
        ssp_dac_write(slave, i, voltage);
        post_log("\t DAC set pin %i = %d\n", i , voltage);
    }
    for (i = 0; i <= 3; i++) {
        gpio_adc[i] = adc_aux_channel_read(i + spi * 4);
        post_log("\t Voltage of pin %i = %d\n", i + spi * 4, gpio_adc[i]);
    }
    if ((gpio_adc[3] > gpio_adc[2]) && (gpio_adc[2] > gpio_adc[1])
            && (gpio_adc[1] > gpio_adc[0])) {
        post_log("\tPass\n");
    } else {
        post_log("\tFail\n");
        ret = -1;
        goto out;
    }

    post_log("\n Set 4 DAC voltage in reverse order\n");
    for (i = 0; i <= 3; i++) {
        voltage = 0xb000 / 3 * (3 - i) + 0x1000;
        ssp_dac_write(slave, i, voltage);
        post_log("\t DAC set pin %i = %d\n", i , voltage);
    }
    for (i = 0; i <= 3; i++) {
        gpio_adc[i] = adc_aux_channel_read(i + spi * 4);
        post_log("\t Voltage of pin %i = %d\n", i, gpio_adc[i]);
    }
    if ((gpio_adc[3] < gpio_adc[2]) && (gpio_adc[2] < gpio_adc[1])
            && (gpio_adc[1] < gpio_adc[0])) {
        post_log("\tPass\n");
    } else {
        post_log("\tFail\n");
        ret = -1;
        goto out;
    }
    ret = 0;
    post_log("\n\r SPI-%d Diag Passed\n", spi);
    out: pl022_spi_free_slave(slave);
    return ret;
}

/*
 * spi serial flash test
 * support Cygnus SVK spi2 & spi3 which connected to MX25L512E, 104MHz
 */
int ssp_flash_post_test(uint8_t spi) {
    int len = 4;
    struct spi_flash *flash;
    uint8_t buf[256];
    uint32_t j;
    int ret = -1;

    /* Configure IOMUX
     * detect spi
     * probe flash
     * for each page:
     *  erase block
     *  verify all 0
     *  write pattern
     *  read & verify
     */
	if (post_get_board_diags_type() != BCM958305K) {
	/*Skip this check for WA as all SPI are connected to MX25L512EMI*/
    	switch (spi) {
    	case 2:
        	post_log("\n Test SPI2 -> EEPROM. \n");
        	break;
    	case 3:
        	post_log(
                "\n Test SPI3 -> EEPROM. Please turn switch SW9017 3,4 to off(default) \n");
        	break;
    	default:
        	post_log("\n invalid invocation to SPI_DAC test\n");
        	return 1;
        	break;
    	}
	}

    post_log(
            "\n INFO: MX25L512EMI EEPROM supports 80MHz max baudrate on its spi interface \n");

    flash = spi_flash_probe(CONFIG_PL022_SPI_BUS, spi, 100000,
            CONFIG_SF_DEFAULT_MODE);
    if (!flash) {
        post_log("\n Failed to probe EEPROM \n");
        return -1;
    }

    post_log("erase...");
    ret = spi_flash_erase(flash, 0, flash->size);
    if (ret) {
        post_log("\n error erase \n");
        goto done;
    }

    post_log("read 1...");
    ret = spi_flash_read(flash, 0, len, buf);
    if (ret) {
        post_log("\n error read \n");
        goto done;
    }
    for (j = 0; j < len; j++) {
        post_log("%02x",buf[j]);
        if (buf[j] != 0xff)
            break;
        buf[j] = j;
    }
    if (j < len) {
        post_log("\n read error at offset %d\n", j);
        ret = -1;
        goto done;
    }
    post_log("  verified 1...");

    post_log("write 1...");
    ret = spi_flash_write(flash, 0, len, buf);
    if (ret) {
        post_log("\n error write %d\n");
        goto done;
    }

    post_log("read 2...");
    ret = spi_flash_read(flash, 0, len, buf);
    if (ret) {
        post_log("\n error read\n");
        goto done;
    }
    for (j = 0; j < len; j++) {
        post_log("%02x",buf[j]);
        if (buf[j] != j)
            break;
        buf[j] = j;
    }
    if (j < len) {
        post_log("\n error \n");
        ret = -1;
        goto done;
    }
    post_log("  verified 2...\n");

    done:
    post_log("\n\r SPI-%d Diag %s\n", spi,
            ret == 0 ? "Passed" : "Failed");
    spi_flash_free(flash);
    return ret;
}

int spi_post_test(int flags) {
    int ret = 0;
	if (post_get_board_diags_type() != BCM958305K) {
		ret |= ssp_dac_post_test(0);
    	ret |= ssp_dac_post_test(1);
    	ret |= ssp_flash_post_test(2);
    	ret |= ssp_flash_post_test(3);
	} else {
		/*For WA all the spi's are connected to EEPROM*/
		int i = 0;
		for (i = 0; i < 3; i++)
			ret |= ssp_flash_post_test(i);
	}
    return ret;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_MSR */
