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
#include <config.h>
#include <i2c.h>
#include <asm/io.h>

#include "iproc_i2c_regs.h"
#include "iproc_i2c_defs.h"
#include "iproc_i2c.h"
#include "errno.h"

#if defined(CONFIG_PDT_FACTORY_ENV) 

extern void fac_env_relocate_spec (void);
extern uchar fac_env_get_char_spec(int);
extern int fac_env_check(void);

#endif

/* local debug macro */
#undef IPROC_I2C_DBG

#undef debug
#ifdef IPROC_I2C_DBG
#define debug(fmt,args...)	printf (fmt ,##args)
#else
#define debug(fmt,args...)
#endif /* I2C_DEBUG */

static int  i2c_init_done = 0;

#if defined(CONFIG_HELIX4) 
static u32  SHADOW_CPY_BUFFER =     0x70000000;
static u32  IPROC_SMBUS_BASE_ADDR =   0x18038000;
#elif (defined(CONFIG_GREYHOUND) || defined(CONFIG_SABER2))
static u32 SHADOW_CPY_BUFFER =       0x70000000;
static u32 IPROC_SMBUS_BASE_ADDR  =  0x18008000;
#else 
#define SHADOW_CPY_BUFFER 0x89000000
#define IPROC_SMBUS_BASE_ADDR   (0x18009000)
#endif


#ifdef CONFIG_I2C_MULTI_BUS
static unsigned int current_bus;

/**
 * i2c_set_bus_num - change active I2C bus
 *	@bus: bus index, zero based
 *	@returns: 0 on success, non-0 on failure
 */
int i2c_set_bus_num(unsigned int bus)
{
	if ((bus < 0) || (bus >= CONFIG_SYS_MAX_I2C_BUS)) {
		printf("Bad bus: %d\n", bus);
		return -1;
	}

	switch (bus) {
        case 0:
#if defined(CONFIG_HELIX4)
                IPROC_SMBUS_BASE_ADDR = 0x18038000;
#else
                IPROC_SMBUS_BASE_ADDR = 0x18008000;
#endif
                SHADOW_CPY_BUFFER = 0x70000000;
                break;
        case 1:
#if defined(CONFIG_HELIX4)
                IPROC_SMBUS_BASE_ADDR = 0x1803B000;
#else
                IPROC_SMBUS_BASE_ADDR = 0x1800B000;
#endif
                SHADOW_CPY_BUFFER  = 0x70100000;
                break;
#if CONFIG_SYS_MAX_I2C_BUS > 2
        case 2:
                /* CMICD I2C base */
		        IPROC_SMBUS_BASE_ADDR = 0x3200000;
                SHADOW_CPY_BUFFER  = 0x70100000;
                break;
#endif
	default:
		return -1;
	}
	current_bus = bus;
	return 0;
}
/**
 * i2c_get_bus_num - returns index of active I2C bus
 */
unsigned int i2c_get_bus_num(void)
{
	return current_bus;
}
#endif



/* Function to read a value from specified register. */
static unsigned int iproc_i2c_reg_read(unsigned long reg_addr)
{
    unsigned int val;

    val = __raw_readl((void *)(IPROC_SMBUS_BASE_ADDR + reg_addr));
    return( cpu_to_le32(val) );
}

/* Function to write a value ('val') in to a specified register. */
static int iproc_i2c_reg_write(unsigned long reg_addr, unsigned int val)
{
    val = cpu_to_le32(val);
    __raw_writel(val, (void *)(IPROC_SMBUS_BASE_ADDR + reg_addr));
    return (0);
}
#ifdef IPROC_I2C_DBG
static int iproc_dump_i2c_regs(void)
{
    unsigned int regval;

    debug("\n----------------------------------------------\n");
    debug("%s: Dumping SMBus registers... \n", __func__);

    regval = iproc_i2c_reg_read(CCB_SMB_CFG_REG);
    debug("CCB_SMB_CFG_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_TIMGCFG_REG);
    debug("CCB_SMB_TIMGCFG_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_ADDR_REG);
    debug("CCB_SMB_ADDR_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_MSTRFIFOCTL_REG);
    debug("CCB_SMB_MSTRFIFOCTL_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_SLVFIFOCTL_REG);
    debug("CCB_SMB_SLVFIFOCTL_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_BITBANGCTL_REG);
    debug("CCB_SMB_BITBANGCTL_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);
    debug("CCB_SMB_MSTRCMD_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_SLVCMD_REG);
    debug("CCB_SMB_SLVCMD_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_EVTEN_REG);
    debug("CCB_SMB_EVTEN_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_EVTSTS_REG);
    debug("CCB_SMB_EVTSTS_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_MSTRDATAWR_REG);
    debug("CCB_SMB_MSTRDATAWR_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_MSTRDATARD_REG);
    debug("CCB_SMB_MSTRDATARD_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_SLVDATAWR_REG);
    debug("CCB_SMB_SLVDATAWR_REG=0x%08X\n", regval);

    regval = iproc_i2c_reg_read(CCB_SMB_SLVDATARD_REG);
    debug("CCB_SMB_SLVDATARD_REG=0x%08X\n", regval);

    debug("----------------------------------------------\n\n");
    return(0);
}
#endif
/*
 * Function to ensure that the previous transaction was completed before
 * initiating a new transaction. It can also be used in polling mode to
 * check status of completion of a command
 */
static int iproc_i2c_startbusy_wait(void)
{
    unsigned int regval;

    regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);

    /* Check if an operation is in progress. During probe it won't be.
     * But when shutdown/remove was called we want to make sure that
     * the transaction in progress completed
     */
    if (regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK) {
        unsigned int i = 0;

        do {
               udelay(10000); /* Wait for 1 msec */
               i++;
               regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);

          /* If start-busy bit cleared, exit the loop */
        } while ((regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK) &&
                 (i < IPROC_SMB_MAX_RETRIES));

        if (i >= IPROC_SMB_MAX_RETRIES) {
            printf("%s: START_BUSY bit didn't clear, exiting\n",
                   __func__);;
            return -ETIMEDOUT;
        }
    }
    return 0;
}

/*
 * This function copies data to SMBus's Tx FIFO. Valid for write transactions
 * only
 *
 * base_addr: Mapped address of this SMBus instance
 * dev_addr:  SMBus (I2C) device address. We are assuming 7-bit addresses
 *            initially
 * info:   Data to copy in to Tx FIFO. For read commands, the size should be
 *         set to zero by the caller
 *
 */
static void iproc_i2c_write_trans_data(unsigned short dev_addr,
                                       struct iproc_xact_info *info)
{
    unsigned int regval;
    unsigned int i;
    unsigned int num_data_bytes = 0;

#ifdef IPROC_I2C_DBG
    debug("\n%s: dev_addr=0x%X, cmd_valid=%d, cmd=0x%02x, size=%u proto=%d\n", 
                __func__, dev_addr, info->cmd_valid, info->command, info->size, info->smb_proto);
#endif /* IPROC_I2C_DBG */

    /* Write SMBus device address first */
    /* Note, we are assuming 7-bit addresses for now. For 10-bit addresses,
     * we may have one more write to send the upper 3 bits of 10-bit addr
     */
    iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG, dev_addr);

    /* If the protocol needs command code, copy it */
    if (info->cmd_valid) {
        iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG, info->command);
    }

    /* Depending on the SMBus protocol, we need to write additional transaction
     * data in to Tx FIFO. Refer to section 5.5 of SMBus spec for sequence for a
     * transaction
     */
    switch (info->smb_proto) {

        case SMBUS_PROT_RECV_BYTE:
            /* No additional data to be written */
            num_data_bytes = 0;
        break;

        case SMBUS_PROT_SEND_BYTE:
            num_data_bytes = info->size;
        break;

        case SMBUS_PROT_RD_BYTE:
        case SMBUS_PROT_RD_WORD:
        case SMBUS_PROT_BLK_RD:
            /* Write slave address with R/W~ set (bit #0) */
            iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG, dev_addr | 0x1);
            num_data_bytes = 0;
        break;
        case SMBUS_PROT_WR_BYTE:
        case SMBUS_PROT_WR_WORD:
            /* No additional bytes to be written. Data portion is written in the
             * 'for' loop below
             */
            num_data_bytes = info->size;
        break;

        case SMBUS_PROT_BLK_WR:
            /* 3rd byte is byte count */
            iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG, info->size);
            num_data_bytes = info->size;
        break;

        default:
        break;

    }

    /* Copy actual data from caller, next. In general, for reads, no data is
     * copied
     */
    for (i = 0; num_data_bytes; --num_data_bytes, i++) {
        /* For the last byte, set MASTER_WR_STATUS bit */
        regval = (num_data_bytes == 1) ? 
                                       info->data[i] | CCB_SMB_MSTRWRSTS_MASK :
                                       info->data[i];

        iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG, regval);
    }

    return;
}

static int iproc_i2c_data_send(unsigned short addr,
                               struct iproc_xact_info *info)
{
    int rc, retry=3;
    unsigned int regval;

    /* Make sure the previous transaction completed */
    rc = iproc_i2c_startbusy_wait();

    if (rc < 0) {
        printf("%s: Send: bus is busy, exiting\n", __func__);;
        return rc;
    }

    /* Write transaction bytes to Tx FIFO */
    iproc_i2c_write_trans_data(addr, info);

    /* Program master command register (0x30) with protocol type and set
     * start_busy_command bit to initiate the write transaction
     */
    regval = (info->smb_proto << CCB_SMB_MSTRSMBUSPROTO_SHIFT) |
              CCB_SMB_MSTRSTARTBUSYCMD_MASK;

    iproc_i2c_reg_write(CCB_SMB_MSTRCMD_REG, regval);

    /* Check for Master status */
    regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);
    while(regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK)
    {
        udelay(10000);
        if(retry-- <=0) break;
        regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);
    }        

    /* If start_busy bit cleared, check if there are any errors */
    if (!(regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK)) {

        /* start_busy bit cleared, check master_status field now */
        regval &= CCB_SMB_MSTRSTS_MASK;
        regval >>= CCB_SMB_MSTRSTS_SHIFT;

        if (regval != MSTR_STS_XACT_SUCCESS) {
            /* Error We can flush Tx FIFO here */
            printf("%s: ERROR: Send: Error in transaction %u, exiting",
                   __func__, regval);
           return -EREMOTEIO;
        }
    }

    return(0);
}

static int iproc_i2c_data_recv(unsigned short addr,
                               struct iproc_xact_info *info,
                               unsigned int *num_bytes_read)
{
    int rc, retry=3;
    unsigned int regval;

    /* Make sure the previous transaction completed */
    rc = iproc_i2c_startbusy_wait();

    if (rc < 0) {
        printf("%s: Receive: Bus is busy, exiting\n", __func__);
        return rc;
    }

    /* Program all transaction bytes into master Tx FIFO */
    iproc_i2c_write_trans_data(addr, info);

    /* Program master command register (0x30) with protocol type and set
     * start_busy_command bit to initiate the write transaction
     */
    regval = (info->smb_proto << CCB_SMB_MSTRSMBUSPROTO_SHIFT) |
              CCB_SMB_MSTRSTARTBUSYCMD_MASK | info->size;

    iproc_i2c_reg_write(CCB_SMB_MSTRCMD_REG, regval);

    /* Check for Master status */
    regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);
    while(regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK)
    {
        udelay(1000);
        if(retry-- <=0) break;
        regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);
    } 

    /* If start_busy bit cleared, check if there are any errors */
    if (!(regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK)) {

        /* start_busy bit cleared, check master_status field now */
        regval &= CCB_SMB_MSTRSTS_MASK;
        regval >>= CCB_SMB_MSTRSTS_SHIFT;

        if (regval != MSTR_STS_XACT_SUCCESS) {
            /* We can flush Tx FIFO here */
            printf("%s: Error in transaction %d, exiting\n",
                   __func__, regval);
           return -EREMOTEIO;
        }
    }

    /* Read received byte(s), after TX out address etc */
    regval = iproc_i2c_reg_read(CCB_SMB_MSTRDATARD_REG);

    /* For block read, protocol (hw) returns byte count, as the first byte */
    if (info->smb_proto == SMBUS_PROT_BLK_RD) {

        int i;

        *num_bytes_read = regval & CCB_SMB_MSTRRDDATA_MASK;

        /* Limit to reading a max of 32 bytes only; just a safeguard. If
         * # bytes read is a number > 32, check transaction set up, and contact
         * hw engg. Assumption: PEC is disabled 
         */
        for (i = 0; (i < *num_bytes_read) && (i < I2C_SMBUS_BLOCK_MAX); i++) {

            /* Read Rx FIFO for data bytes */
            regval = iproc_i2c_reg_read(CCB_SMB_MSTRDATARD_REG);
            info->data[i] = regval & CCB_SMB_MSTRRDDATA_MASK;
        }
    } else if (info->size == 2) {
        /* 2 Bytes data */
        info->data[0] = regval & CCB_SMB_MSTRRDDATA_MASK;
        regval = iproc_i2c_reg_read(CCB_SMB_MSTRDATARD_REG);
        info->data[1] = regval & CCB_SMB_MSTRRDDATA_MASK;
        *num_bytes_read = 2;
    }
    else {
        /* 1 Byte data */
        *(info->data) = regval & CCB_SMB_MSTRRDDATA_MASK;
        *num_bytes_read = 1;
    }

    return(0);
}


/*
 * This function set clock frequency for SMBus block. As per hardware
 * engineering, the clock frequency can be changed dynamically.
 */
static int iproc_i2c_set_clk_freq(smb_clk_freq_t freq)
{
    unsigned int regval;
    unsigned int val;

    switch (freq) {

        case I2C_SPEED_100KHz:
            val = 0;
            break;

        case I2C_SPEED_400KHz:
            val = 1;
            break;

        default:
            return -EINVAL;
            break;
    }
    
    regval = iproc_i2c_reg_read(CCB_SMB_TIMGCFG_REG);

    SETREGFLDVAL(regval, val, CCB_SMB_TIMGCFG_MODE400_MASK, 
                 CCB_SMB_TIMGCFG_MODE400_SHIFT);

    iproc_i2c_reg_write(CCB_SMB_TIMGCFG_REG, regval);
    return(0);
}

static void iproc_i2c_init (int speed, int slaveadd)
{
    unsigned int regval;

#ifdef IPROC_I2C_DBG
    debug("\n%s: Entering i2c_init\n", __func__);
#endif /* IPROC_I2C_DBG */

    /* Flush Tx, Rx FIFOs. Note we are setting the Rx FIFO threshold to 0.
     * May be OK since we are setting RX_EVENT and RX_FIFO_FULL interrupts
     */
    regval = CCB_SMB_MSTRRXFIFOFLSH_MASK | CCB_SMB_MSTRTXFIFOFLSH_MASK;
    iproc_i2c_reg_write(CCB_SMB_MSTRFIFOCTL_REG, regval);

    /* Enable SMbus block. Note, we are setting MASTER_RETRY_COUNT to zero
     * since there will be only one master
     */

    regval = iproc_i2c_reg_read(CCB_SMB_CFG_REG);
    regval |= CCB_SMB_CFG_SMBEN_MASK;
    iproc_i2c_reg_write(CCB_SMB_CFG_REG, regval);
    /* Wait a minimum of 50 Usec, as per SMB hw doc. But we wait longer */
    udelay(10000);

    /* Set default clock frequency */
    iproc_i2c_set_clk_freq(I2C_SPEED_100KHz);

    /* Disable intrs */
    regval = 0x0;
    iproc_i2c_reg_write(CCB_SMB_EVTEN_REG, regval);

    /* Clear intrs (W1TC) */
    regval = iproc_i2c_reg_read(CCB_SMB_EVTSTS_REG);    
    iproc_i2c_reg_write(CCB_SMB_EVTSTS_REG, regval);

    i2c_init_done = 1;

#ifdef IPROC_I2C_DBG
    iproc_dump_i2c_regs();

    debug("%s: Init successful\n", __func__);
#endif /* IPROC_I2C_DBG */


}

extern void iproc_i2c_iomux(int op);
extern int fac_env_init(void);

void i2c_init (int speed, int slaveadd)
{
#ifdef IPROC_I2C_DBG
    debug("\n%s: Entering i2c_init\n", __func__);
#endif /* IPROC_I2C_DBG */
#ifdef CONFIG_I2C_MULTI_BUS
    unsigned int old_bus;
    old_bus = i2c_get_bus_num();
    i2c_set_bus_num(0);               
    iproc_i2c_init(speed, slaveadd);
    i2c_set_bus_num(1);
    iproc_i2c_init(speed, slaveadd);
#if CONFIG_SYS_MAX_I2C_BUS > 2
    i2c_set_bus_num(2);
    iproc_i2c_init(speed, slaveadd);
#endif
    i2c_set_bus_num(old_bus); /* tianzhy */
#else
    iproc_i2c_init(speed, slaveadd);
#endif
#if defined(CONFIG_PDT_FACTORY_ENV) 
	fac_env_init();
	fac_env_check();
#endif
}

int i2c_probe (uchar chip)
{
    u32 slave_addr = chip, regval;
#ifdef IPROC_I2C_DBG
    debug("\n%s: Entering probe\n", __func__);
#endif /* IPROC_I2C_DBG */

    /* Init internal regs, disable intrs (and then clear intrs), set fifo
     * thresholds, etc.
     */
    if(! i2c_init_done)
    {
#ifdef CONFIG_I2C_MULTI_BUS
        unsigned int old_bus;
        old_bus = i2c_get_bus_num();
        i2c_set_bus_num(0);               
        iproc_i2c_init(0, 0);
        i2c_set_bus_num(1);
        iproc_i2c_init(0, 0);
#if CONFIG_SYS_MAX_I2C_BUS > 2
        i2c_set_bus_num(2);
        iproc_i2c_init(0, 0);
#endif
        i2c_set_bus_num(old_bus);
#else
        iproc_i2c_init(0, 0);
#endif
    }

    regval = (slave_addr << 1);
    iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG, regval);
    regval = ((0 << 9) | (1 << 31));
    iproc_i2c_reg_write(CCB_SMB_MSTRCMD_REG, regval);
    do {
        udelay(1000);
        regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);
        regval &= CCB_SMB_MSTRSTARTBUSYCMD_MASK;
    }  while (regval);
    regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);
    if (((regval >> 25) & 7) == 0)
             //printf("i2c slave address:%x\n", slave_addr);
      ;
    else 
       return -1;

#ifdef IPROC_I2C_DBG
    iproc_dump_i2c_regs();

    debug("%s: probe successful\n", __func__);
#endif /* IPROC_I2C_DBG */

    return 0;
}

static int i2c_read_byte (u8 devaddr, u8 regoffset, u8 * value)
{
    int rc;
    struct iproc_xact_info info;
    unsigned int num_bytes_read = 0;

    devaddr <<= 1;

    info.cmd_valid = 1;
    info.command = (unsigned char)regoffset;
    info.data = value;
    info.size = 1;
    info.flags = 0;
    info.smb_proto = SMBUS_PROT_RD_BYTE;
    /* Refer to i2c_smbus_read_byte for params passed. */
    rc = iproc_i2c_data_recv(devaddr, &info, &num_bytes_read);

    if (rc < 0) {
        printf("%s: %s error accessing device 0x%X\n", 
                    __func__, "Read", devaddr);
        return -EREMOTEIO;
    }

    return (0);
}

int i2c_read (uchar chip, uint addr, int alen, uchar * buffer, int len)
{
	int i;

	if (alen > 1) {
		printf ("I2C read: addr len %d not supported\n", alen);
		return 1;
	}

	if (addr + len > 256) {
		printf ("I2C read: address out of range\n");
		return 1;
	}

	for (i = 0; i < len; i++) {
		if (i2c_read_byte (chip, addr + i, &buffer[i])) {
			printf ("I2C read: I/O error\n");
			i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
			return 1;
		}
	}

    memcpy((uchar*)SHADOW_CPY_BUFFER, buffer, len);

	return 0;
}

static int i2c_write_byte (u8 devaddr, u8 regoffset, u8 value)
{
    int rc;
    struct iproc_xact_info info;


    devaddr <<= 1;

    info.cmd_valid = 1;
    info.command = (unsigned char)regoffset;
    info.data = &value; 
    info.size = 1;
    info.flags = 0;
    info.smb_proto = SMBUS_PROT_WR_BYTE;
    /* Refer to i2c_smbus_write_byte params passed. */
    rc = iproc_i2c_data_send(devaddr, &info);

    if (rc < 0) {
        printf("%s: %s error accessing device 0x%X\n", 
                    __func__, "Write", devaddr);
        return -EREMOTEIO;
    }

    return (0);
}

int i2c_write (uchar chip, uint addr, int alen, uchar * buffer, int len)
{
	int i;

	if (alen > 1) {
		printf ("I2C read: addr len %d not supported\n", alen);
		return 1;
	}

	if (addr + len > 256) {
		printf ("I2C read: address out of range\n");
		return 1;
	}

	for (i = 0; i < len; i++) {
		if (i2c_write_byte (chip, addr + i, buffer[i])) {
			printf ("I2C read: I/O error\n");
			i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
			return 1;
		}
	}

	return 0;
}
int i2c_write_word (uchar devaddr, uint addr, u16 value)
{
    int rc;
    struct iproc_xact_info info;
    u8 data[2];

    devaddr <<= 1;
    data[0] = value & 0xff;
    data[1] = (value & 0xff00) >> 8;

    info.cmd_valid = 1;
    info.command = (unsigned char)addr;
    info.data = data; 
    info.size = 2;
    info.flags = 0;
    info.smb_proto = SMBUS_PROT_WR_WORD;

    /* Refer to i2c_smbus_write_byte params passed. */
    rc = iproc_i2c_data_send(devaddr, &info);

    if (rc < 0) {
        printf("%s: %s error accessing device 0x%X\n", 
                    __func__, "Write", devaddr);
        return -EREMOTEIO;
    }

    return (0);
}

int i2c_read_word (uchar devaddr, uint addr , u16* value)
{
    int rc;
    struct iproc_xact_info info;
    u8 data[2];
    unsigned int num_bytes_read;

    devaddr <<= 1;

    info.cmd_valid = 1;
    info.command = (unsigned char)addr;
    info.data = data; 
    info.size = 2;
    info.flags = 0;
    info.smb_proto = SMBUS_PROT_RD_WORD;

    rc = iproc_i2c_data_recv(devaddr, &info, &num_bytes_read);

    if (rc < 0) {
        printf("%s: %s error accessing device 0x%X\n", 
                    __func__, "Read", devaddr);
        return -EREMOTEIO;
    }

    *value = (u16) (info.data[0] & 0xff);
    *value |= (u16)(((u16)info.data[1]) << 8);	    

    return (0);
}
int i2c_block_read(uchar chip, uint addr, u8* buffer, int* len)
{
    int i, rc;
    struct iproc_xact_info info;
    unsigned int num_bytes_read;

    chip <<= 1;

    info.cmd_valid = 1;
    info.command = (unsigned char)addr;
    info.data = buffer; 
    info.size = 0;
    info.flags = 0;
    info.smb_proto = SMBUS_PROT_BLK_RD;

    rc = iproc_i2c_data_recv(chip, &info, &num_bytes_read);

    if (rc < 0) {
        printf("%s: %s error accessing device 0x%X\n", 
                    __func__, "Read", chip);
        return -EREMOTEIO;
    }

    for( i = 0; i < num_bytes_read; i++) {
        buffer[i] = info.data[i];
    }
    *len = num_bytes_read;

    return (0);
}
int i2c_send_byte (uchar chip, u8 addr)
{
    int rc;
    struct iproc_xact_info info;
    u8 data[2];

    chip <<= 1;
    data[0] = addr;

    info.cmd_valid = 0;
    info.data = data; 
    info.size = 1;
    info.flags = 0;
    info.smb_proto = SMBUS_PROT_SEND_BYTE;

    /* Refer to i2c_smbus_write_byte params passed. */
    rc = iproc_i2c_data_send(chip, &info);

    if (rc < 0) {
        printf("%s: %s error accessing device 0x%X\n", 
                    __func__, "Write", chip);
        return -EREMOTEIO;
    }

    return (0);
}
/**
 * i2c_set_bus_speed - set i2c bus speed
 *      @speed: bus speed (in HZ)
 * This function returns invalid or 0
 */

int i2c_set_bus_speed(unsigned int speed)
{

    switch (speed) {

        case 100000:
	    iproc_i2c_set_clk_freq(I2C_SPEED_100KHz);
            break;

        case 400000:
            iproc_i2c_set_clk_freq(I2C_SPEED_400KHz);
            break;

        default:
            return -EINVAL;
            break;
    }
    return 0;
}

/**
 * i2c_get_bus_speed - get i2c bus speed
 *     
 * This function returns the speed of operation in Hz
 */
unsigned int i2c_get_bus_speed(void)
{
     unsigned int regval;
     unsigned int val;

     regval = iproc_i2c_reg_read(CCB_SMB_TIMGCFG_REG);
     val = GETREGFLDVAL(regval, CCB_SMB_TIMGCFG_MODE400_MASK,
                                 CCB_SMB_TIMGCFG_MODE400_SHIFT);
     switch (val) {
        case I2C_SPEED_100KHz:
            return 100000;
            break;

        case I2C_SPEED_400KHz:
            return 400000;
            break;

        default:
            return 0;
            break;
     }
     return 0;
}
