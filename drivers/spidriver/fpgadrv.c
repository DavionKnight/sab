/*
*  COPYRIGHT NOTICE
*  Copyright (C) 2016 HuaHuan Electronics Corporation, Inc. All rights reserved
*
*  Author               :kevin_fzs
*  File Name            :fpga_flash_drv.c
*  Create Date          :2017/02/23
*  Last Modified        :
*  Description      :
*/
#include "gpiodrv.h"
#include "fpgadrv.h"
#include "spidrv.h"
#include "spi.h"

extern int spidrv_semid;
extern struct sembuf spidrv_sembufLock, spidrv_sembufUnlock;
int gpio_direction_output ( unsigned gpio, unsigned char value );

#if defined(H20RN181)
#define GPIO_RECONFIG 	14
#endif

#if defined(H20RN2000) || defined(H20RN1000)

#define GPIO_RECONFIG 	4

#define FPGA_RT_CLAU_ADDR               (0x2020)
#define FPGA_RT_BUFF_ADDR               (0x2200)
#define FPGA_RT_CMD_BUFF_ADDR   (0x0400)
#define FPGA_RT_CLAU_UNIT_SIZE  (0x0020)
#define FPGA_RT_RD_OVER_FLGA    (0x2002)

#define FPGA_RT_WR_ADDR                 (0x2010)

#define FPGA_CR_CLAU_ADDR               (0x2100)
#define FPGA_CR_BUFF_ADDR               (0x2400)
#define FPGA_CR_CMD_BUFF_ADDR   (0x0800)
#define FPGA_CR_CLAU_UNIT_SIZE  (0x0020)
#define FPGA_CR_EN_ADDR                 (0x2040)

#define FPGA_RT_CLAU                    32
#define FPGA_CR_CLAU                    128
#define FPGA_CLAUSE_NUM                 168

/* buffer assigned
---------------------------------------0x2200
|
|       32 short * 32 clauses    rt
|--------------------------------------0x2400
|
|
|       32 short * 128 clauses   cir
|
|
|--------------------------------------0x2C00
|
|
|--------------------------------------0x2FFF
 */

static unsigned char rtClause = 0;

#pragma pack(1)
typedef struct s_fpga_rm_argv
{
    unsigned char       slot;    // 0/1/2/3/11
    unsigned int        addr;    // 0x0000~0x0FFF
    unsigned int        size;    // max FPGA_COPRO_MAX_SIZE
    unsigned short      *pbuf;  //
    unsigned char       used;   //
}       s_FPGA_RM_ARGV;
#pragma pack()

s_FPGA_RM_ARGV clausCrMap[FPGA_CR_CLAU] = {0};

#endif

static int fpga_local_write ( unsigned int addr, unsigned char *data, size_t count )
{
    int ret, i;
    unsigned int address = 0;
    unsigned char buf[MULTI_REG_LEN_MAX + 2] = {0};

    if ( !data || count > MULTI_REG_LEN_MAX )
    {
        return -1;
    }

    if ( 1 == count )
    {
        buf[0] = SPI_FPGA_WR_SINGLE;
    }
    else
    {
        buf[0] = SPI_FPGA_WR_BURST;
    }
    address = addr;
    buf[1] = ( unsigned char ) ( ( address >> 8 ) & 0xff );
    buf[2] = ( unsigned char ) ( ( address ) & 0xff );

    memcpy ( &buf[3], data, count );

    ret = spidrv_write ( buf, count + 3, 0, CHIPSELECT_FPGA );
    if ( 0 != ret )
    {
        printf ( "spidrv_write error!!!\n" );
    }

}
int fpga_write ( unsigned int addr, unsigned char *data, size_t count, unsigned char slot )
{
    int ret, i;
    unsigned int address = 0;
    unsigned char buf[MULTI_REG_LEN_MAX + 2] = {0};

    semop ( spidrv_semid, &spidrv_sembufLock, 1 );

#if defined(H20RN181) || defined(H20RN181_ARM)
    fpga_local_write ( addr, data, count );
#endif

#if defined(H20RN2000) || defined(H20RN1000)
    if ( ( 0xe == slot ) || ( 0xf == slot ) )
    {
        fpga_local_write ( addr, data, count );
    }
    else
    {
        unsigned char wdata[8] = {0};
        unsigned short *ptr = ( unsigned short * ) data;
        unsigned int wrdata = 0;

        for ( ptr; ptr < ( unsigned short * ) ( data + count ); ptr++ )
        {
            wdata[0] = ( ( slot & 0x0f ) << 4 ) | ( ( addr & 0xf00 ) >> 8 );
            wdata[1] = addr & 0xff;
            wdata[2] = ( *ptr & 0xff00 ) >> 8;
            wdata[3] = *ptr & 0xff;
            memcpy ( &wrdata, wdata, sizeof ( wrdata ) );
            fpga_local_write ( FPGA_RT_WR_ADDR, ( unsigned char * ) &wrdata, sizeof ( wrdata ) );
            usleep ( 10 );
            addr ++;
        }
    }
#endif

    semop ( spidrv_semid, &spidrv_sembufUnlock, 1 );

    return ret;
}

static int fpga_local_read ( unsigned int addr, unsigned char *data, size_t count )
{
    int ret;
    unsigned char tx_buf[MULTI_REG_LEN_MAX + 4] = {0};
    unsigned char rx_buf[MULTI_REG_LEN_MAX + 4] = {0};

    if ( 1 == count )
    {
        tx_buf[0] = SPI_FPGA_RD_SINGLE;
    }
    else
    {
        tx_buf[0] = SPI_FPGA_RD_BURST;
    }
    tx_buf[1] = ( unsigned char ) ( ( addr >> 8 ) & 0xff );
    tx_buf[2] = ( unsigned char ) ( ( addr ) & 0xff );

    ret = spidrv_read ( tx_buf, 3, rx_buf, count, CHIPSELECT_FPGA );

    memcpy ( data, &rx_buf[0], count );

    return ret;

}


int fpga_read ( unsigned int addr, unsigned char *data, size_t count, unsigned char slot )
{
    int ret;
    unsigned int address = 0;

    if ( !data || count > MULTI_REG_LEN_MAX )
    {
        return -1;
    }

    semop ( spidrv_semid, &spidrv_sembufLock, 1 );

#if defined(H20RN181) || defined(H20RN181_ARM)
    fpga_local_read ( addr, data, count );
#endif
#if defined(H20RN2000) || defined(H20RN1000)
    if ( ( 0xe == slot ) || ( 0xf == slot ) )
    {
        fpga_local_read ( addr, data, count );
    }
    else /*unit board read*/
    {
        unsigned int rdata[FPGA_RT_CLAU_UNIT_SIZE / 2] = {0};
        unsigned int reg_addr;
        unsigned char data_set[4] = {0};
        unsigned int data_rw = 0, bufaddr = 0, clause = 0;
        unsigned int value_en;
        unsigned short read_reg;
        unsigned int delay_count = 1000;
        unsigned short *pbuf = ( unsigned short * ) data;
        unsigned int i = 0;

        clause = rtClause++;
        if ( rtClause >= FPGA_RT_CLAU )
        {
            rtClause = 0;
        }

//              printf("clause = %d,unitboard slot = 0x%x, addr = 0x%02x\n", clause, slot,addr);
        bufaddr = FPGA_RT_CMD_BUFF_ADDR + clause * FPGA_RT_CLAU_UNIT_SIZE;
        read_reg = FPGA_RT_CLAU_ADDR + clause;

        data_set[0] = ( ( slot & 0x0f ) << 4 ) | ( ( addr & 0xf00 ) >> 8 );
        data_set[1] = addr & 0xff;
        data_set[2] = ( 0x5 << 5 ) | ( ( bufaddr & 0x1f00 ) >> 8 );
        data_set[3] = ( ( bufaddr & 0xe0 ) | ( 0x1f ) );

        memcpy ( &data_rw, data_set, sizeof ( data_rw ) );

        fpga_local_write ( ( unsigned short ) read_reg, ( unsigned char * ) &data_rw, sizeof ( data_rw ) );

        do
        {
            fpga_local_read ( ( unsigned short ) FPGA_RT_RD_OVER_FLGA, ( unsigned char * ) &value_en, sizeof ( value_en ) );
            if ( ( value_en == 0 ) || ( delay_count < 1 ) )
            {
                break;
            }
        }
        while ( delay_count-- );

        reg_addr = FPGA_RT_BUFF_ADDR + clause * FPGA_RT_CLAU_UNIT_SIZE / 2;

        fpga_local_read ( ( unsigned short ) reg_addr, ( unsigned char * ) rdata, sizeof ( rdata ) );
        for ( i = 0; i < count / 2; i++ )
        {
            pbuf[i] = i % 2 ? ( ( rdata[i / 2] >> 16 ) & 0xffff ) : ( rdata[i / 2] & 0xffff );
        }
    }
#endif


    semop ( spidrv_semid, &spidrv_sembufUnlock, 1 );

    return ret;
}

void fpga_flash_gpio_set ( unsigned char en )
{
#if defined(H20RN2000)||defined(H20RN1000)
    if ( en > 1 )
    {
        return;
    }
    gpio_direction_output ( 12, en );
#endif
#if defined(H20RN181)
    if(en > 1)
	return;
    gpio_direction_output (11 ,en);
#endif
#if 0
    /*gpioout:0x18000064---gpioouten:0x18000068*/
    unsigned int *virt_addr_out, *virt_addr_outen;

    virt_addr_out    = mapbase + 0x64;
    virt_addr_outen  = mapbase + 0x68;
    if ( !en )
    {
        * ( ( unsigned int * ) virt_addr_out )   &= ~ ( 0x800 );
        * ( ( unsigned int * ) virt_addr_outen ) |= 0x800;
    }
    else
    {
        * ( ( unsigned int * ) virt_addr_out )   |= 0x800;
        * ( ( unsigned int * ) virt_addr_outen ) |= 0x800;
    }
#endif
}

/******************************w25p16.h start***************************/
#define FLASH_PAGESIZE      256

/* Flash opcodes. */
#define OPCODE_WREN         0x06    /* Write enable */
#define OPCODE_RDSR         0x05    /* Read status register */
#define OPCODE_WRSR         0x01    /* Write status register 1 byte */
#define OPCODE_NORM_READ    0x03    /* Read data bytes (low frequency) */
#define OPCODE_FAST_READ    0x0b    /* Read data bytes (high frequency) */
#define OPCODE_PP           0x02    /* Page program (up to 256 bytes) */
#define OPCODE_BE_4K        0x20    /* Erase 4KiB block */
#define OPCODE_BE_32K       0x52    /* Erase 32KiB block */
#define OPCODE_CHIP_ERASE   0xc7    /* Erase whole flash chip */
#define OPCODE_SE           0xd8    /* Sector erase (usually 64KiB) */
#define OPCODE_RDID         0x9f    /* Read JEDEC ID */

#define OPCODE_READ         0x03
void fpga_flash_reconfig ( void )
{
    int i;
//    gpio_direction_output ( GPIO_RECONFIG, 0 );
    usleep(100000);
 //   gpio_direction_output ( GPIO_RECONFIG, 1 );
}

static int fpga_flash_status_get()
{
    unsigned char code = OPCODE_RDSR;
    int ret;

    unsigned char buf[14] = {0};
    unsigned char rx_buf[14] = {0};

    buf[0] = code;

    ret = spidrv_read ( buf, 1, rx_buf, 1, CHIPSELECT_FPGA_FLASH );
    if ( 0 != ret )
    {
        printf ( "spidrv_read error!!!\n" );
    }

    return rx_buf[0];
}


/*
 * Set write enable latch with Write Enable command.
 * Returns negative if error occurred.
 */
static int fpga_flash_write_enable()
{
    unsigned char code = OPCODE_WREN;
    int ret;

    ret = spidrv_write ( &code, 1, 0, CHIPSELECT_FPGA_FLASH );
    if ( 0 != ret )
    {
        printf ( "spidrv_write error!!!\n" );
    }

    return ret;
}

/*
 * Service routine to read status register until ready, or timeout occurs.
 * Returns non-zero if error.
 */
#define MAX_READY_WAIT_COUNT    100000
/* Status Register bits. */
#define SR_WIP          1   /* Write in progress */
#define SR_WEL          2   /* Write enable latch */
#define MTD_SIZE    0x7fffff
#define CMD_SIZE        4

static int fpga_flash_wait_till_ready()
{
    int count;
    int sr;


    for ( count = 0; count < MAX_READY_WAIT_COUNT; count++ )
    {
        if ( ( sr = fpga_flash_status_get() ) < 0 )
        {
            break;
        }
        else if ( ! ( sr & SR_WIP ) )
        {
            return 0;
        }
        usleep ( 10 );
    }

    return -1;
}

int fpga_flash_erase ( void )
{

    int ret, i;
    unsigned char buf = 0;

    semop ( spidrv_semid, &spidrv_sembufLock, 1 );

    fpga_flash_gpio_set ( 0 );

    /* Wait until finished previous write command. */

    if ( fpga_flash_wait_till_ready() )
    {
        printf ( "flash is not ready!\n" );
        fpga_flash_gpio_set ( 1 );
        semop ( spidrv_semid, &spidrv_sembufUnlock, 1 );
        return -1;
    }

    /* Send write enable, then erase commands. */
    fpga_flash_write_enable();

    /* Set up command buffer. */
    buf = OPCODE_CHIP_ERASE;

    ret = spidrv_write ( &buf, 1, 0, CHIPSELECT_FPGA_FLASH );
    if ( 0 != ret )
    {
        printf ( "spidrv_write error!!!\n" );
    }

    fpga_flash_gpio_set ( 1 );
    semop ( spidrv_semid, &spidrv_sembufUnlock, 1 );
    for ( i = 0; i < 10000; i++ ); /*yu20160711*/

    return 0;
}

struct spi_transfer_t
{
    /* it's ok if tx_buf == rx_buf (right?)
     * for MicroWire, one buffer must be null
     * buffers must work with dma_*map_single() calls, unless
     *   spi_message.is_dma_mapped reports a pre-existing mapping
     */
    const void  *tx_buf;
    void        *rx_buf;
    unsigned    len;
    unsigned char       bits_per_word;
    unsigned short      delay_usecs;
    unsigned int        speed_hz;

};
int fpga_flash_write ( long to, const unsigned char *buf, unsigned int len )
{
    unsigned int page_offset, page_size;
    unsigned char chip_select = 2;
    struct spi_transfer_t t[2];
    int ret, i;
    unsigned char tx_buf[256] = {0};

    semop ( spidrv_semid, &spidrv_sembufLock, 1 );

    fpga_flash_gpio_set ( 0 );
    t[0].tx_buf  = tx_buf;

    /* sanity checks */
    if ( !len )
    {
        return ( 0 );
    }

    if ( to + len > MTD_SIZE )
    {
        return -EINVAL;
    }

    t[0].tx_buf = tx_buf;
    t[0].len = CMD_SIZE;

#if 1
    /* Wait until finished previous write command. */
    if ( fpga_flash_wait_till_ready() )
    {
        printf ( "flash no ready!\n" );
        fpga_flash_gpio_set ( 1 );
        semop ( spidrv_semid, &spidrv_sembufUnlock, 1 );
        return 1;
    }
#endif

    fpga_flash_write_enable();

    /* Set up the opcode in the write buffer. */
    tx_buf [0] = OPCODE_PP;
    tx_buf [1] = to >> 16;
    tx_buf [2] = to >> 8;
    tx_buf [3] = to;
    memcpy ( tx_buf + 4 , buf, len );

    /* what page do we start with? */
    page_offset = to % FLASH_PAGESIZE;

    /* do all the bytes fit onto one page? */
    if ( page_offset + len <= FLASH_PAGESIZE )
    {

        t[0].len = CMD_SIZE + len ;

        ret = spidrv_write ( tx_buf, t[0].len, 0, CHIPSELECT_FPGA_FLASH );
        if ( 0 != ret )
        {
            printf ( "spidrv_write error!!!\n" );
        }

    }
    else
    {
        unsigned int i;

        /* the size of data remaining on the first page */
        page_size = FLASH_PAGESIZE - page_offset;

        t[1].len = page_size;

        ret = spidrv_write ( tx_buf, page_size + CMD_SIZE, 0, CHIPSELECT_FPGA_FLASH );
        if ( 0 != ret )
        {
            printf ( "spidrv_write error!!!\n" );
        }

        for ( i = page_size; i < len; i += page_size )
        {
            page_size = len - i;
            if ( page_size > FLASH_PAGESIZE )
            {
                page_size = FLASH_PAGESIZE;
            }

            /* write the next page to flash */
            tx_buf[1] = ( to + i ) >> 16;
            tx_buf[2] = ( to + i ) >> 8;
            tx_buf[3] = ( to + i );

            t[1].tx_buf = tx_buf + i;
            t[1].len = page_size;

            fpga_flash_wait_till_ready();
            fpga_flash_write_enable();

            ret = spidrv_write ( ( unsigned char * ) t[1].tx_buf, page_size, 0, CHIPSELECT_FPGA_FLASH ); /*yu20160705*/
            if ( 0 != ret )
            {
                printf ( "spidrv_write error!!!\n" );
            }

        }
    }

    for ( i = 0; i < 3000; i++ );                                       /*yu20160711*/
    fpga_flash_gpio_set ( 1 );
    semop ( spidrv_semid, &spidrv_sembufUnlock, 1 );

    return 0;
}

int fpga_flash_read ( long from, unsigned char *buf, unsigned int len )
{
    struct spi_transfer_t t[2];
    int ret, i;
    unsigned char chip_select = 2;

    semop ( spidrv_semid, &spidrv_sembufLock, 1 );
    fpga_flash_gpio_set ( 0 );

    unsigned char rx_buf[256] = {0};
    unsigned char tx_buf[256] = {0};

    /* sanity checks */
    if ( !len )
    {
        return 0;
    }

    if ( from + len > MTD_SIZE )
    {
        return -1;
    }

    memset ( t, 0, ( sizeof t ) );

    /* NOTE:
     * OPCODE_FAST_READ (if available) is faster.
     * Should add 1 byte DUMMY_BYTE.
     */
    tx_buf[0] = OPCODE_READ;
    tx_buf[1] = from >> 16;
    tx_buf[2] = from >> 8;
    tx_buf[3] = from;
    t[0].tx_buf = tx_buf;
    t[0].len = len;
    t[0].rx_buf = rx_buf;

    /* Wait till previous write/erase is done. */
    if ( fpga_flash_wait_till_ready() )
    {
        /* REVISIT status return?? */
        fpga_flash_gpio_set ( 1 );
        semop ( spidrv_semid, &spidrv_sembufUnlock, 1 );
        return 1;
    }

    ret = spidrv_read ( tx_buf, 4, rx_buf, len, CHIPSELECT_FPGA_FLASH );
    if ( 0 != ret )
    {
        printf ( "spidrv_read error!!!\n" );
    }

    memcpy ( buf, rx_buf, len );

    for ( i = 0; i < 3000; i++ );                                   /*yu20160711*/

    fpga_flash_gpio_set ( 1 );

    semop ( spidrv_semid, &spidrv_sembufUnlock, 1 );

    return 0;
}


#if defined(H20RN2000) || defined(H20RN1000)

int fpga_circle_read_get ( int *clause )
{
    int i = 0;

    for ( i = 0; i < FPGA_CR_CLAU; i++ )
    {
        if ( !clausCrMap[i].used )
        {
            break;
        }
    }
    if ( FPGA_CR_CLAU == i )
    {
        return -1;
    }
    *clause = i;
    return 0;
}

int fpga_circle_read_set ( int clause, unsigned char slot, unsigned int addr, unsigned int size )
{
    unsigned char data[4] = {0};
    unsigned short read_reg;
    unsigned int data_rw = 0, bufaddr = 0, count;

    if ( ( clause < 0 ) || ( clause >= FPGA_CR_CLAU ) )
    {
        return -1;
    }

    if ( ( size > FPGA_CR_CLAU_UNIT_SIZE ) )
    {
        printf ( "Read length should be less than 32, size = %d\n", size );
        return -1;
    }

    clausCrMap[clause].slot = slot;
    clausCrMap[clause].addr = addr;
    clausCrMap[clause].size = size;
    clausCrMap[clause].used = 1;

    bufaddr = FPGA_CR_CMD_BUFF_ADDR + clause * FPGA_CR_CLAU_UNIT_SIZE;
    read_reg = FPGA_CR_CLAU_ADDR + clause;

    data[0] = ( ( slot & 0x0f ) << 4 ) | ( ( addr & 0xf00 ) >> 8 );
    data[1] = addr & 0xff;
    data[2] = ( 0x5 << 5 ) | ( ( bufaddr & 0x1f00 ) >> 8 );
    data[3] = ( ( bufaddr & 0xe0 ) | ( 0x1f ) );

    memcpy ( &data_rw, data, sizeof ( data_rw ) );
    //      printf("data_rw=0x%x data3=0x%x size=0x%x\n",data_rw,data[3],size);
    return fpga_write ( read_reg, ( unsigned char * ) &data_rw, sizeof ( data ), 0xe );

}

int fpga_circle_read_get_config ( int clause, unsigned char *slot, unsigned int *addr, unsigned int *size )
{
    if ( ( clause < 0 ) || ( clause >= FPGA_CR_CLAU ) )
    {
        return -1;
    }

    *slot = clausCrMap[clause].slot;
    *addr = clausCrMap[clause].addr;
    *size = clausCrMap[clause].size;

    return 0;
}
int fpga_circle_read_enable ( int clause )
{
    unsigned int en_addr, en_bit;
    unsigned int data, delay_count = 1000;

    if ( clause > FPGA_CR_CLAU )
    {
        return -1;
    }
    en_addr = FPGA_CR_EN_ADDR + clause / 16;
    en_bit  = clause % 16;

    do
    {
        fpga_read ( en_addr, ( unsigned char * ) &data, sizeof ( data ), 0xe );
        if ( ( ( data & 0xffff ) == 0 ) || ( delay_count < 1 ) )
        {
            break;
        }
    }
    while ( delay_count-- );

    data = 1 << en_bit;
    //      printf("en_addr=0x%x,en data=0x%x\n",en_addr,data);
    return fpga_write ( en_addr, ( unsigned char * ) &data, sizeof ( data ), 0xe );
}
/*
 *set circle read enable by enbuf
 *size: 0 ~ 7
 *
 */
int fpga_circle_read_block_enable ( unsigned short *enbuf, unsigned int size )
{
    unsigned int en_addr, i = 0;
    unsigned int data, delay_count = 1000;

    if ( ( NULL == enbuf ) || ( size >= 8 ) )
    {
        return -1;
    }
    for ( i = 0; i < size; i++ )
    {
        en_addr = FPGA_CR_EN_ADDR + i;
        do
        {
            fpga_read ( en_addr, ( unsigned char * ) &data, sizeof ( data ), 0xe );
            if ( ( ( data & 0xffff ) == 0 ) || ( delay_count < 1 ) )
            {
                break;
            }
        }
        while ( delay_count-- );

        data = 0;
        data |= enbuf[i];
        fpga_write ( en_addr, ( unsigned char * ) &data, sizeof ( data ), 0xe );
    }
    return 0;
}
/*
 * cir rd data
 *   mode: 0, check status, default
 *         1, do not check
 */
int fpga_circle_read ( int clause, unsigned char slot, unsigned int addr, unsigned short *pbuf, unsigned int size )
{
    unsigned int data[FPGA_CR_CLAU_UNIT_SIZE / 2] = {0}, mode;
    int i = 0;
    unsigned int reg_addr;
    unsigned int en_addr, en_bit;
    unsigned int delay_count = 1000;
    unsigned int value_en;

    if ( ( clause < 0 ) || ( clause >= FPGA_CR_CLAU ) )
    {
        return -1;
    }
    if ( !pbuf )
    {
        return -1;
    }
    if ( ( clausCrMap[clause].slot != slot ) || ( clausCrMap[clause].addr != addr ) || ( clausCrMap[clause].size != size ) )
    {
        printf ( "fpga_rm_cir_read para is not confirm with configure\n" );
        return -1;
    }

    en_addr = FPGA_CR_EN_ADDR + clause / 16;
    en_bit  = clause % 16;

    do
    {
        fpga_read ( en_addr, ( unsigned char * ) &value_en, sizeof ( value_en ), 0xe );
        if ( ( ( value_en & ( 1 << en_bit ) ) == 0 ) || ( delay_count < 1 ) )
        {
            break;
        }
    }
    while ( delay_count-- );
    reg_addr = FPGA_CR_BUFF_ADDR + clause * FPGA_CR_CLAU_UNIT_SIZE / 2;

    fpga_read ( reg_addr, ( unsigned char * ) data, sizeof ( data ), 0xe );
    for ( i = 0; i < size; i++ )
    {
        pbuf[i] = i % 2 ? ( ( data[i / 2] >> 16 ) & 0xffff ) : ( data[i / 2] & 0xffff );
    }

    return 0;

}

#endif

