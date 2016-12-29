#
# (C) Copyright 2000-2015
# Broadcom Corporation
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

#include <common.h>
#include <exports.h>
#include <asm/io.h>

#include <asm/arch/socregs.h>

//#include "qspi_apis.h"
//#include "cyg_ccg_spi2.h"

#define test_status_t int
#define TEST_PASS 0
#define TEST_FAIL 0
#define print_log printf
#define rd(a) readl(a)
#define wr(a,b) writel(b,a)



#define SPI_CMD_READ_ID                 0x9F
#define SPI_CMD_RESET_ENABLE            0x66
#define SPI_CMD_RESET_MEMORY            0x99
#define SPI_CMD_WRITE_ENABLE            0x06
#define SPI_CMD_READ_STATUS_REGISTER    0x05
#define SPI_CMD_SECTOR_ERASE            0xD8
#define SPI_CMD_PAGE_PROGRAM            0x02
#define SPI_CMD_READ                    0x03

test_status_t wait_for_spi_done(void) {
    uint32_t data;
    do {
        data = rd(ChipcommonG_SPI2_SSPSR);
        data = (data >> ChipcommonG_SPI2_SSPSR__BSY) & 1; 
    } while (data);
}

test_status_t ccg_spi_transfer(uint8_t wr_len, uint8_t *wr_ptr, uint8_t rd_len, uint8_t *rd_ptr) {
    uint32_t i;
    uint32_t cmd_ram_addr;
    uint32_t cmd_ram_data;
    uint32_t endqp;
    uint32_t data;
    
    wait_for_spi_done();

    for(i=0; i<wr_len; i++) {
        wr(ChipcommonG_SPI2_SSPDR, *(wr_ptr+i));
    }
    for(i=0; i<rd_len; i++) {
        wr(ChipcommonG_SPI2_SSPDR, 0x0);
    }
    
    wr(ChipcommonG_SPI2_SSPCR1, (1 << ChipcommonG_SPI2_SSPCR1__SSE));
    wait_for_spi_done();
    wr(ChipcommonG_SPI2_SSPCR1, 0);

    i=0;
    for(i=0; i<wr_len; i++) {
        data = rd(ChipcommonG_SPI2_SSPDR);
        //print_log("Dummy READ_DATA[%d] = 0x%08X\n", i++, data);
    } 
    
    i=0;
    while (rd(ChipcommonG_SPI2_SSPSR) & (1 << ChipcommonG_SPI2_SSPSR__RNE)) {
        data = rd(ChipcommonG_SPI2_SSPDR);
        //print_log("READ_DATA[%d] = 0x%08X\n", i, data);
        *(rd_ptr+i) = data;
        i++;
    } 
}

test_status_t ccg_spi_read_devid() {
    uint8_t tx_data[16], rx_data[16];
    uint8_t spi_device_id[3];
    
    spi_device_id[0] = 0;
    spi_device_id[1] = 0;
    spi_device_id[2] = 0;
    tx_data[0] = SPI_CMD_READ_ID;
    ccg_spi_transfer(1, tx_data, 3, rx_data);
    spi_device_id[0] = rx_data[0];
    spi_device_id[1] = rx_data[1];
    spi_device_id[2] = rx_data[2];

    if((spi_device_id[0] == 0) || (spi_device_id[0] == 0xFF)) {
        print_log("No device found on SPI\n");
    } else {
        print_log("Device ID found on SPI bus - 0x%02X 0x%02X 0x%02X \n", spi_device_id[0], spi_device_id[1], spi_device_id[2]);
    }
}

test_status_t ccg_spi_read_data(uint32_t address, uint32_t *data) {
    uint8_t tx_data[16], rx_data[16];
    tx_data[0] = SPI_CMD_READ;
    tx_data[1] = ((address >> 0)  & 0xFF);
    tx_data[2] = ((address >> 8)  & 0xFF);
    tx_data[3] = ((address >> 16) & 0xFF);
    ccg_spi_transfer(4, tx_data, 4, rx_data);
    *data = *(uint32_t *) rx_data;
}

test_status_t cyg_ccg_spi2(uint32_t argc, uint32_t *argv)
{
    test_status_t overall_status = TEST_PASS;
    uint8_t message_arr[256],read_back_arr[256];
    int32_t i; 
    uint32_t DestAddr;
    uint32_t Offset;
    uint32_t rdata32;
    uint32_t error=0; 
    uint32_t pgm_data_len = 256;
    uint8_t tx_data[16], rx_data[16];
    uint32_t cpol, cpha, data;
    uint8_t spi_device_id[3];
    uint32_t device_status_reg;
    uint32_t sector_address, write_addr, read_addr;
    uint32_t addr;
    uint32_t rd_data, rd_addr;

    //-----------------------------------------------------------------------------

    wr(CRMU_CHIP_IO_PAD_CONTROL, 0);

    data = 0;
    data |= (1 << ChipcommonG_SPI2_SSPCR0__SCR_R);
    data |= (1 << ChipcommonG_SPI2_SSPCR0__SPH);
    data |= (1 << ChipcommonG_SPI2_SSPCR0__SPO);
    data |= (0 << ChipcommonG_SPI2_SSPCR0__FRF_R);
    data |= (7 << ChipcommonG_SPI2_SSPCR0__DSS_R);
    wr(ChipcommonG_SPI2_SSPCR0, data);
  
    ccg_spi_read_devid();

    //-----------------------------------------------------------------------------
    rd_addr = 0;
    ccg_spi_read_data(rd_addr, &rd_data);
    print_log("Read address = 0x%08X, Read data = 0x%08X\n", rd_addr, rd_data);
    
    //-----------------------------------------------------------------------------


    if(error) overall_status = TEST_FAIL;
    return overall_status;
}
