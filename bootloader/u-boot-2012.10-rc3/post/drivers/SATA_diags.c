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
#include <ahci.h>
#include <scsi.h>
#include <iproc_ahci.h>

#define true 1
#define false 0

#if CONFIG_POST & CONFIG_SYS_POST_SATA

#define NUM_OF_PORTS  2
#define NUM_OF_BLOCKS  1000
#define START_LBA     1000

/* Size in byte */
#define SATA_DATA_SECT_COUNT 256
#define SATA_DATA_BUF_SIZE   (SATA_DATA_SECT_COUNT * 512)
#define SATA_DATA_BUF_ADDR   0x70000000
#define SATA_DATA_TRANSFER_COUNT 16384*8

static int run_pcie_sata(void);

int sata_post_test (int flags)
{

        int j,port=0;

    //22HR has a SATA sitting on the PCI bus
    //it is different from the native sata
    if ( post_get_board_diags_type() == BCM958622HR )
    {
        return (run_pcie_sata());
    }

    //Config IDM clock control, AHCI endianess
    config_AHCI();
    //Config PHY
    configure_SATA_PHY ();


    if ( post_get_board_diags_type() & (BCM953025K|BCM953025K_REV2))
    {

                if ( (flags & POST_AUTO) !=  POST_AUTO )
                {
                        post_log("\nPlease connect the SATA drive to the port to be tested,make sure it is powered on(0/1)");
                        post_log("\n---Please power cycle the hard drive after switching ports---");
                        post_getConfirmation("\nReady? (Y/N)");
                }
    }
    //Auto test still defaults to port 0
    //Manual test - user selectable
    if ( (flags & POST_AUTO) !=  POST_AUTO )
    {
        // 23HR has only one port SATA 0
        if ( post_get_board_diags_type() != BCM958623HR )
        {
                        post_log("\n\nSelect SATA port to test:0 - iSATA ; 1 - eSATA \n");
                        port = post_getUserInput("port #? (0/1) : ");

                        if ( (port != 0) && ( port != 1) )
                        {
                                printf("\nWrong port !!!");
                                return -1;
                        }
        }
    }


    if ( -1 == ahci_init(port) )
    {
        printf("\nPort init failed!!");
        return -1;
    }

    for ( j = 0; j <=NUM_OF_BLOCKS; j+=4 )
    {
        if ( 0 == iproc_SATA_DMA_RD_WR_verify(port, 0, 4, START_LBA))
        {
                printf(" . ");
        }
        else
        {
                printf("\nFailed!");
                return -1;
        }
    }
    printf("\nPassed\n");
    return 0;
}

/*
 *  Parse a string into an unsigned int.
 *
 *  Skips leading whitespace.  Skips underlines, commas, periods (they are not
 *  considered in the resulting value).  Anything else that is not a digit
 *  terminates (including further whitespace).
 *
 *  Digits are taken from this pool (in this order), according to the base:
 *    0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@
 *  If the base is less than 36, upper and lower case alpha are equivalent.
 *  Note that even things from this pool that are not valid for the working
 *  base are not considered digits and will cause the parsing to terminate.
 *  Whatever value has been obtained at termination is returned.
 *
 *  The default base is provided by the caller, but if an invalid or
 *  unsupported base is provided by the caller, decimal is assumed. If
 *  encountered at the beginning of the number, these override the base:
 *    0x = hexadecimal
 *    0 = octal (only if next character is not 'x')
 *    % = binary
 *    $ = hexadecimal
 *    #n# = base n, where n is a decimal number 2..64
 *  No, Microsoft-style base-postfix form is not supported.  Don't ask for it.
 *
 *  Returns zero if all went well.  Otherwise returns non-zero.  The value is
 *  returned by writing it to the address indicated by the value argument.  If
 *  the parsed number is too big, ~0 is returned instead.  Value will not be
 *  updated on non-zero returns.
 */
static int
strtouint(const char *buffer, const char **next, unsigned int base, unsigned int *value)
{
    unsigned int index = 0;
    unsigned int offset = 0;
    unsigned int working = 0;
    unsigned int previous;
    unsigned int digit;
    char current;
    int skip;
    int result = 0;

    if ((2 > base) || (64 < base)) {
        /* provided base is not valid; assume decimal */
        base = 10;
    }
    if ((!value) || (!buffer)) {
        return 1; /* invalid parameters */
    }
    /* skip leading whitespace */
    while (('\t' == buffer[offset]) ||
           ('\n' == buffer[offset]) ||
           (' ' == buffer[offset])) {
        offset++;
    }
    /* scan to end of valid number (might be immediate) */
    do {
        previous = working;
        current = buffer[offset + index];
        if (0 == index) {
            /* first non-blank character can be a special case */
            skip = 0;
            switch (current) {
            case '%':
                /* old style binary */
                base = 2;
                skip = 1;
                break;
            case '$':
                /* old-style hexadecimal */
                base = 16;
                skip = 1;
                break;
            case '0':
                /* C-style octal (or maybe hexadecimal)   */
                if (('x' == buffer[offset + index + 1]) ||
                    ('X' == buffer[offset + index + 1])) {
                    /* C-style hexadecimal */
                    base = 16;
                    skip = 2;
                } else {
                    /* C-style octal */
                    base = 8;
                    skip = 1;
                }
                break;
            case '#':
                /* more flexible base, up to 64 */
                index++;
                base = 0;
                while (('0' <= buffer[offset + index]) &&
                       ('9' >= buffer[offset + index])) {
                    base *= 10;
                    base += (buffer[offset + index] - '0');
                    index++;
                }
                if ('#' != buffer[offset + index]) {
                    result = 2; /* invalid format */
                    goto abort;
                }
                if ((2 > base) ||
                    (64 < base)) {
                    result = 3; /* unsupported base */
                    goto abort;
                }
                skip = 1;
            }
            if (skip) {
                index += skip;
                continue;
            }
        } /* if (0 == index) */
        if (('_' == current) ||
            ('.' == current) ||
            (',' == current)) {
            /* just skip these */
            index++;
            continue;
        } else if (('0' <= current) && ('9' >= current)) {
            /* decimal digit */
            digit = current - '0';
        } else if (('A' <= current) && ('Z' >= current)) {
            /* upper-case alpha */
            digit = current - 'A' + 10;
        } else if (('a' <= current) && ('z' >= current)) {
            /* lower-case alpha */
            digit = current - 'a' + 10;
            if (base > 36) {
                /* lower-case and upper-case are different for large bases */
                digit += 26;
            }
        } else if ('!' == current) {
            digit = 62;
        } else if ('@' == current) {
            digit = 63;
        }
        if (digit >= base) {
            /* this digit is not valid for this base, so end of number */
            break;
        }
        working *= base;
        working += digit;
        if (working < previous) {
            /* overflow, so saturate instead of wrapping */
            working = ~0;
        }
        index++;
    } while (0 != buffer[offset + index]);
abort:
    if (next) {
        /* caller wants to know the next character in the buffer */
        *next = &(buffer[offset + index]);
    }
    if (0 == result) {
        *value = working;
    }
    return result;
}

/* standard AHCI tests use up to 16MiB at 0x71000000, so it should be 'safe' */
#define SATA_POST_TEST_PATT_BUFFER 0x62000000

#define AHCI_GHC_HOST_IRQ_STAT 0x18041008


/* at 128KiB per segment, this gives 2MiB of data */
static inline u16 valueatindex(unsigned int index) {
    unsigned int mode = (index >> 16) & 0xF;
    register u16 value = index & 0xFFFF;
    switch (mode) {
    case 0:
        /* basic counting */
        /* requires no modifications */
        break;
    case 1:
        /* bitwise inversion of basic counting */
        value = ~value;
        break;
    case 2:
        /* backwards counting */
        value = -value;
        break;
    case 3:
        /* bitwise inversion of backwards counting */
        value = ~(-value);
        break;
    case 4:
        /* odd sectors are bitwise inversion of mode 0 */
        if (index & 0x200) {
            value = ~value;
        }
        break;
    case 5:
        /* even sectors are bitwise inversion of mode 0 */
        if (0 == (index & 0x200)) {
            value = ~value;
        }
        break;
    case 6:
        /* odd sectors are bitwise inversion of mode 2 */
        value = -value;
        if (index & 0x200) {
            value = ~value;
        }
        break;
    case 7:
        /* even sectors are bitwise inversion of mode 2 */
        value = -value;
        if (0 == (index & 0x200)) {
            value = ~value;
        }
        break;
    case 8:
        /* even sectors are mode 0, odd sectors are mode 2 */
        if (index & 0x200) {
            value = -value;
        }
        break;
    case 9:
        /* even sectors are mode 2, odd sectors are mode 0 */
        if (0 == (index & 0x200)) {
            value = -value;
        }
        break;
    case 10:
        /* even sectors are mode 1, odd sectors are mode 3 */
        if (index & 0x200) {
            value = ~(-value);
        } else {
            value = ~value;
        }
        break;
    case 11:
        /* even sectors are mode 3, odd sectors are mode 1 */
        if (index & 0x200) {
            value = ~value;
        } else {
            value = ~(-value);
        }
        break;
    case 12:
        /* odd sector pairs inverted, odd sectors backwards */
        if (index & 0x200) {
            value = -value;
        }
        if (index & 0x400) {
            value = ~value;
        }
        break;
    case 13:
        /* odd sector pairs inverted, even sectors backwards */
        if (0 == (index & 0x200)) {
            value = -value;
        }
        if (index & 0x400) {
            value = ~value;
        }
        break;
    case 14:
        /* even sector pairs inverted, odd sectors backwards */
        if (index & 0x200) {
            value = -value;
        }
        if (0 == (index & 0x400)) {
            value = ~value;
        }
        break;
    case 15:
        /* even sector pairs inverted, even sectors backwards */
        if (0 == (index & 0x200)) {
            value = -value;
        }
        if (0 == (index & 0x400)) {
            value = ~value;
        }
        break;
    }
    return value;
}
/* this must be less than or equal to the patterns in the above function */
#define BLOCK_PATTERN_COUNT 16

static int
sata_post_test_sectors_write(unsigned int drive,
                             u16* value,
                             unsigned int initBlock,
                             unsigned int blocksPerAccess,
                             unsigned int maxBytesPerTransfer)
{
    unsigned int index;
    unsigned int count;
    unsigned int offset;
    unsigned int blocks;
    index = 0;
    count = 0;
    offset = initBlock;
    blocks = blocksPerAccess;
    int result = 0;
    do {
        printf("Writing pattern to drive at sector %u..%u (%u sectors)\r",
               offset,
               offset + blocks,
               blocks);
        AhciFillCmdSlot(drive,
                        0 /* tag */,
                        true /* write */,
                        true /* DMA */,
                        blocks /* sectors */,
                        false /* NCQ */,
                        offset /* first sector */,
                        false /* reset */);
        result = AhciFilSg(drive,
                           0 /* tag */,
                           (u32)&(value[index << 8]),
                           blocks << 9,
                           maxBytesPerTransfer,
                           false /* blank */);
        if (result) {
            printf("error preparing SG list for write\n");
            goto error;
        }
        AhciIssueCmd(drive, 0 /* tag */);
        result = wait_command_done(drive, 0 /* tag */, true /* DMA */, blocks);
        if (result) {
            printf("error waiting for command to complete\n");
            goto error;
        }
        index += blocks;
        offset += blocks;
        count++;
        if ((index + blocks) > (BLOCK_PATTERN_COUNT << 8)) {
            blocks = (BLOCK_PATTERN_COUNT << 8) - index;
        }
    } while (index < (BLOCK_PATTERN_COUNT << 8));
error:
    if (result) {
        printf("\nUnable to write pattern to drive %d\n", drive);
    } else {
        printf("\rWritten pattern to drive %d at sector %u..%u, %u writes     \n",
               drive,
               initBlock,
               offset - 1,
               count);
    }
    return result;
}

static unsigned int
sata_post_test_sectors_check(unsigned int drive,
                             u16* value,
                             unsigned int *firstErr)
{
    unsigned int index;
    u16 expected;
    int result = 0;

    /* ensure no speculative (or residual) data still cached */
    invalidate_dcache_range((unsigned long)&(value[0]),
                            (unsigned long)&(value[BLOCK_PATTERN_COUNT << 16]));
    for (index = 0; index < (BLOCK_PATTERN_COUNT << 16); index++) {
        expected = valueatindex(index);
        if (expected != value[index]) {
            if (!result) {
                *firstErr = index;
            }
            if (result < 200) {
                printf("\rMiscompare at %08X: expected %04X, got %04X\n",
                       index << 1,
                       expected,
                       value[index]);
            } else if (200 == result) {
                printf("\rToo many miscompares; skipping the rest\n");
            }
            result++;
        }
    } /* for (index = 0; index < 16*64*1024; index++) */
    return result;
}

static unsigned int
sata_post_test_sectors_qcheck(unsigned int drive,
                              u16* pattern,
                              u16* value,
                              unsigned int *firstErr)
{
    register unsigned int index;
    register u64* orig;
    register u64* data;
    int result = 0;

    /* ensure no speculative (or residual) data still cached */
    invalidate_dcache_range((unsigned long)&(value[0]),
                            (unsigned long)&(value[BLOCK_PATTERN_COUNT << 16]));
    for (index = 0, orig = (u64*)pattern, data = (u64*)value;
         index < (BLOCK_PATTERN_COUNT << 14);
         index++, orig++, data++) {
        if ((*orig) != (*data)) {
            result = -1;
            break;
        }
    }
    if (result) {
        /* buffers are not the same; scan for the differences */
        printf("\rChecking mismatch ");
        result = 0;
        for (index = 0; index < (BLOCK_PATTERN_COUNT << 16); index++) {
            if (pattern[index] != value[index]) {
                if (!result) {
                    *firstErr = index;
                }
                if (result < 200) {
                    printf("\rMiscompare at %08X: expected %04X, got %04X\n",
                           index << 1,
                           pattern[index],
                           value[index]);
                } else if (200 == result) {
                    printf("\rToo many miscompares; skipping the rest\n");
                }
                result++;
            }
        } /* for (index = 0; index < 16*64*1024; index++) */
    }
    return result;
}

static void
sata_post_test_sectors_qdump(unsigned int drive,
                             u16* pattern,
                             u16* value,
                             unsigned int initBlock,
                             unsigned int blocksPerAccess,
                             unsigned int firstErr)
{
    unsigned int index;

    index = (firstErr >> 8) / blocksPerAccess;
    printf("Drive %u miscompare at %u, in read %u\n"
           "Sector %u into read of %u sectors\n",
           drive,
           firstErr << 1,
           index,
           (firstErr >> 8) - (index * blocksPerAccess),
           blocksPerAccess);
    printf("Read sector numbers %u..%u (%u..%u relative)\n"
           "Failed sector %u (%u relative)\n",
           (index * blocksPerAccess) + initBlock,
           (((index + 1) * blocksPerAccess) - 1) + initBlock,
           index * blocksPerAccess,
           ((index + 1) * blocksPerAccess) - 1,
           (firstErr >> 8) + initBlock,
           firstErr >> 8);
    printf("Expected sector data:");
    for (index = firstErr & (~0xFF);
         index < ((firstErr & (~0xFF)) + 0x200);
         index++) {
        if (0 == (index & 0xFF)) {
            printf("\n  [sector boundary]");
        }
        if (0 == (index & 0x7)) {
            printf("\n    %08X: ", index << 1);
        }
        printf(" %04X", pattern[index]);
        if ((7 == (index & 0x7)) &&
             (((index & (~7)) == (firstErr & (~7))))) {
            printf("  << correct values");
        }
    }
    printf("\n  [sector boundary]\n"
           "Actual sector data:");
    for (index = firstErr & (~0xFF);
         index < ((firstErr & (~0xFF)) + 0x200);
         index++) {
        if (0 == (index & 0xFF)) {
            printf("\n  [sector boundary]");
        }
        if (0 == (index & 0x7)) {
            printf("\n    %08X: ", index << 1);
        }
        printf(" %04X", value[index]);
        if ((7 == (index & 0x7)) &&
            (((index & (~7)) == (firstErr & (~7))))) {
            printf("  << mismatch values");
        }
    }
    printf("\n  [sector boundary]\n");
}

static void
sata_post_test_sectors_dump(unsigned int drive,
                            u16* value,
                            unsigned int initBlock,
                            unsigned int blocksPerAccess,
                            unsigned int firstErr)
{
    unsigned int index;

    index = (firstErr >> 8) / blocksPerAccess;
    printf("Drive %u miscompare at %u, in read %u\n"
           "Sector %u into read of %u sectors\n",
           drive,
           firstErr << 1,
           index,
           (firstErr >> 8) - (index * blocksPerAccess),
           blocksPerAccess);
    printf("Read sector numbers %u..%u (%u..%u relative)\n"
           "Failed sector %u (%u relative)\n",
           (index * blocksPerAccess) + initBlock,
           (((index + 1) * blocksPerAccess) - 1) + initBlock,
           index * blocksPerAccess,
           ((index + 1) * blocksPerAccess) - 1,
           (firstErr >> 8) + initBlock,
           firstErr >> 8);
    printf("Expected sector data:");
    for (index = firstErr & (~0xFF);
         index < ((firstErr & (~0xFF)) + 0x200);
         index++) {
        if (0 == (index & 0xFF)) {
            printf("\n  [sector boundary]");
        }
        if (0 == (index & 0x7)) {
            printf("\n    %08X: ", index << 1);
        }
        printf(" %04X", valueatindex(index));
        if ((7 == (index & 0x7)) &&
             (((index & (~7)) == (firstErr & (~7))))) {
            printf("  << correct values");
        }
    }
    printf("\n  [sector boundary]\n"
           "Actual sector data:");
    for (index = firstErr & (~0xFF);
         index < ((firstErr & (~0xFF)) + 0x200);
         index++) {
        if (0 == (index & 0xFF)) {
            printf("\n  [sector boundary]");
        }
        if (0 == (index & 0x7)) {
            printf("\n    %08X: ", index << 1);
        }
        printf(" %04X", value[index]);
        if ((7 == (index & 0x7)) &&
            (((index & (~7)) == (firstErr & (~7))))) {
            printf("  << mismatch values");
        }
    }
    printf("\n  [sector boundary]\n");
}

static char readbuffer[CONFIG_SYS_CBSIZE];

int
sata_post_test_pattern_multi(int flags)
{
    u64 bytesSoFar;
    u64 timeSoFar;
    u32 timeBias;
    u32 timeCheck;
    volatile u32 *pTmp32;
    static unsigned int initBlock = 2048;
    static unsigned int blocksPerAccess = 256;
    static unsigned int maxIterations = 50000;
    static unsigned int maxBytesPerTransfer = 4194304;
    unsigned int maxPorts = 2;
    unsigned int port;
    unsigned int temp;
    unsigned int index[2];
    unsigned int offset[2];
    unsigned int length[2];
    unsigned int iter;
    unsigned int firstErr;
    unsigned int timeout;
    u16 *patt[2];
    u16 *value[2];
    u8 status[2];
    int result;

    //22HR has a SATA sitting on the PCI bus
    //it is different from the native sata
    if (post_get_board_diags_type() == BCM958622HR) {
        printf("\nThis diag intended for NS+ only\n");
        return 2;
    }

    //Config IDM clock control, AHCI endianess
    config_AHCI();
    //Config PHY
    configure_SATA_PHY ();

    if (post_get_board_diags_type() & (BCM953025K|BCM953025K_REV2)) {
        if ((flags & POST_AUTO) !=  POST_AUTO) {
            post_log("\nPlease connect the SATA drive to the port to be tested,make sure it is powered on(0/1)");
            post_log("\n---Please power cycle the hard drive after switching ports---");
            post_getConfirmation("\nReady? (Y/N)");
        }
    }
    if (BCM958623HR == post_get_board_diags_type()) {
        printf("unable to run on this hardware\n");
        return -1;
    }

    //Manual test - user selectable
    if (POST_AUTO != (flags & POST_AUTO)) {
        snprintf(readbuffer, CONFIG_SYS_CBSIZE, "%u", initBlock);
        if (readline_into_buffer("Starting sector:", readbuffer, 0)) {
            result = strtouint(readbuffer, NULL, 10, &temp);
            if (0 == result) {
                initBlock = temp;
            }
        }
        snprintf(readbuffer, CONFIG_SYS_CBSIZE, "%u", blocksPerAccess);
        if (readline_into_buffer("Sectors per command:", readbuffer, 0)) {
            result = strtouint(readbuffer, NULL, 10, &temp);
            if (0 == result) {
                blocksPerAccess = temp;
                if (blocksPerAccess > 16384) {
                    post_log("Sectors per command must be < 32768; assuming 256\n");
                    blocksPerAccess = 256;
                }
                if (blocksPerAccess < 1) {
                    post_log("Must access at lest one sector per command; assuming 256\n");
                    blocksPerAccess = 256;
                }
            }
        }
        snprintf(readbuffer, CONFIG_SYS_CBSIZE, "%u", maxBytesPerTransfer);
        if (readline_into_buffer("Maximum bytes per AHCI memory access:", readbuffer, 0)) {
            result = strtouint(readbuffer, NULL, 10, &temp);
            if (0 == result) {
                maxBytesPerTransfer = temp;
            }
        }
        snprintf(readbuffer, CONFIG_SYS_CBSIZE, "%u", maxIterations);
        if (readline_into_buffer("Maximum iteration count:", readbuffer, 0)) {
            result = strtouint(readbuffer, NULL, 10, &temp);
            if (0 == result) {
                maxIterations = temp;
            }
        }
    } /* if (POST_AUTO != (flags & POST_AUTO)) */

    ahci_reset_controller();
    ahci_init_controller();
    for (port = 0; port < maxPorts; port++) {
        if (SpinUpAndWait(port, 0 /* no speed limit */)) {
            return -1;
        }
        ahci_port_clear_all_serr(port);
        ahci_port_clear_all_int(port);
    }
    ahci_clear_global_int_stat();

    /* set up original pattern buffers  */
    patt[0] = (u16*)SATA_POST_TEST_PATT_BUFFER;
    for (port = 1; port < maxPorts; port++) {
        patt[port] = &(patt[port - 1][BLOCK_PATTERN_COUNT << 17]);
    }
    printf("Building the pattern...");
    for (port = 1; port < maxPorts; port++) {
        /* pattern rotates one bit left for each port */
        unsigned int invPort = 16 - port;
        u16 lm = 0xFFFF << port;
        u16 rm = ~lm;
        for (temp = 0; temp < (BLOCK_PATTERN_COUNT << 16); temp++) {
            register u16 v;
            v = valueatindex(temp);
            patt[port][temp] = (((v << port) & lm) |
                                ((v >> invPort) & rm));
        }
    }
    printf(" %u doublebytes per drive\n", temp);

    printf("Flushing core and L2 cache to memory\n");
    for (port = 1; port < maxPorts; port++) {
        flush_dcache_range((unsigned long)&(patt[port][0]),
                           (unsigned long)&(patt[port][temp]));
    }
    /* write the pattern to the drives */
    for (port = 0; port < maxPorts; port++) {
        result = sata_post_test_sectors_write(port,
                                              patt[port],
                                              initBlock,
                                              blocksPerAccess,
                                              maxBytesPerTransfer);
        if (result) {
            goto error;
        }
    }

    /* set up read buffers */
    value[0] = &(patt[maxPorts - 1][BLOCK_PATTERN_COUNT << 17]);
    for (port = 1; port < maxPorts; port++) {
        value[port] = &(value[port - 1][BLOCK_PATTERN_COUNT << 17]);
    }

    bytesSoFar = 0;
    timeSoFar = 0;
    timeBias = get_timer(0);
    for (iter = 1; iter <= maxIterations; iter++) {
        printf("\rIteration %d ", iter);
        for (port = 0; port < maxPorts; port++) {
            register u64* blank;
            register unsigned int bidx;
            /* poison the buffer so it has no residual data */
            blank = (u64*)(value[port]);
            for (bidx = 0;
                 bidx < (BLOCK_PATTERN_COUNT << 15);
                 bidx++, blank++) {
                *blank = 0xFFFFFFFFFFFFFFFFull;
            }
            /* make sure all writes are written to DRAM */
            flush_dcache_range((unsigned long)&(value[port][0]),
                               (unsigned long)&(value[port][BLOCK_PATTERN_COUNT << 16]));
            status[port] = 0; /* mark port as ready */
            offset[port] = initBlock; /* start at initial block number */
            length[port] = blocksPerAccess; /* start with max access size */
            index[port] = 0 ; /* start at relative sector 0 */
        }
        timeout = 60 * 1000000; /* will be more than one minute */
        do /* while (0 == result) */ {
            if (timeout < 100000) {
                /* display status if nearing timeout */
                printf("\r");
            }
            for (port = 0; port < maxPorts; port++) {
                if (timeout < 100000) {
                    /* display status if nearing timeout */
                    printf("p%1u s%02X i%08X c%08X r%04X ",
                           port,
                           status[port],
                           index[port],
                           offset[port],
                           length[port]);
                }
                switch (status[port]) {
                case 0:
                    /* port is ready */
                    AhciFillCmdSlot(port,
                                    0 /* tag */,
                                    false /* write */,
                                    true /* DMA */,
                                    length[port] /* sectors */,
                                    false /* NCQ */,
                                    offset[port] /* first sector */,
                                    false /* reset */);
                    result = AhciFilSg(port,
                                       0 /* tag */,
                                       (u32)&(value[port][index[port] << 8]),
                                       length[port] << 9,
                                       maxBytesPerTransfer,
                                       false /* blank */);
                    if (result) {
                        printf("error preparing SG list for drive %d"
                               " sectors %d..%d\n",
                               port,
                               offset[port],
                               offset[port] + length[port] - 1);
                        goto error;
                    }
                    AhciIssueCmd(port, 0 /* tag */);
                    status[port] = 1; /* mark port as busy */
                    break;
                case 1:
                    /* port is running a command */
                    if (check_command_done(port,
                                           0 /* tag */,
                                           true /* DMA */,
                                           length[port])) {
                        /* check results from the command */
                        result = check_command_result(port,
                                                      0 /* tag */,
                                                      true /* DMA */,
                                                      length[port]);
                        /* clear port interrupts */
                        ahci_port_clear_all_int(port);
                        pTmp32 = (u32 *)AHCI_GHC_HOST_IRQ_STAT;
                        *pTmp32 = 1 << port;
                        status[port] = 2; /* mark as clearing interrupts */
                        /* handle error after signalling interrupt clear */
                        if (result) {
                            printf("error during read command for drive %d"
                                   " sectors %d..%d\n",
                                   port,
                                   offset[port],
                                   offset[port] + length[port] - 1);
                            goto error;
                        }
                    }
                    break;
                case 2:
                    /* port is clearing interrupts */
                    pTmp32 = (u32 *)AHCI_GHC_HOST_IRQ_STAT;
                    if (0 == ((*pTmp32) & (1 << port))) {
                        /* prepare for next read */
                        offset[port] += length[port];
                        index[port] += length[port];
                        length[port] = blocksPerAccess;
                        if ((index[port] + length[port]) >
                            (BLOCK_PATTERN_COUNT << 8)) {
                            length[port] = (BLOCK_PATTERN_COUNT << 8) - index[port];
                        }
                        if (index[port] < (BLOCK_PATTERN_COUNT << 8)) {
                            status[port] = 0; /* mark as ready */
                        } else {
                            status[port] = 0xFF; /* mark as done */
                        }
                    }
                    break;
                } /* switch (status[port]) */
            } /* for (port = 0; port < maxPorts; port++) */
            for (port = 0, result = 1; port < maxPorts; port++) {
                if (0xFF != status[port]) {
                    /* this port is not finished yet */
                    result = 0;
                }
            }
            udelay(1);
            timeout--;
            if (!timeout) {
                printf("\n"); /* ensure last status stays displayed */
                /* took too long to finish the reads */
                result = -1;
                /* abort the loop */
                goto error;
            }
        } while (0 == result);
        timeCheck = get_timer(0);
        if (timeBias <= timeCheck) {
            timeSoFar += (timeCheck - timeBias);
        } else {
            timeSoFar += ((timeCheck - timeBias) + (u64)0x100000000ull);
        }
        timeBias = timeCheck;
        bytesSoFar += (maxPorts * (BLOCK_PATTERN_COUNT << 17));
        for (result = 0, port = 0; port < maxPorts; port++) {
            temp = sata_post_test_sectors_qcheck(port,
                                                 patt[port],
                                                 value[port],
                                                 &firstErr);
            if (temp) {
                printf("\rIteration %u, failed: %d miscompares\n", iter, temp);
                sata_post_test_sectors_qdump(port,
                                             patt[port],
                                             value[port],
                                             initBlock,
                                             blocksPerAccess,
                                             firstErr);
                /* check it again out of paranoia */
                sata_post_test_sectors_qcheck(port,
                                              patt[port],
                                              value[port],
                                              &firstErr);
                result = -1;
            }
        }
        if (result) {
            break;
        }
    } /* for (count = 1; count <= maxIterations; count++) */
    printf("\n%llu bytes in %llums; %llu bytes per second\n",
           bytesSoFar,
           timeSoFar,
           (bytesSoFar * 1000) / timeSoFar);
error:
    if (result) {
        result = -1;
    }
    return result;
}

int
sata_post_test_pattern(int flags)
{
    u64 timeSoFar;
    u32 timeBias;
    u32 timeCheck;
    int port=0;
    static unsigned int initBlock = 2048;
    static unsigned int blocksPerAccess = 256;
    static unsigned int maxIterations = 50000;
    static unsigned int maxBytesPerTransfer = 4194304;
    unsigned int temp;
    unsigned int index;
    unsigned int offset;
    unsigned int count;
    unsigned int firstErr;
    u16 *value = (u16*)(SATA_POST_TEST_PATT_BUFFER);
    u64 bytesSoFar;
    int result;

    //22HR has a SATA sitting on the PCI bus
    //it is different from the native sata
    if (post_get_board_diags_type() == BCM958622HR) {
        printf("\nThis diag intended for NS+ only\n");
        return 2;
    }

    //Config IDM clock control, AHCI endianess
    config_AHCI();
    //Config PHY
    configure_SATA_PHY ();

    if (post_get_board_diags_type() & (BCM953025K|BCM953025K_REV2)) {
        if ((flags & POST_AUTO) !=  POST_AUTO) {
            post_log("\nPlease connect the SATA drive to the port to be tested,make sure it is powered on(0/1)");
            post_log("\n---Please power cycle the hard drive after switching ports---");
            post_getConfirmation("\nReady? (Y/N)");
        }
    }
    //Auto test still defaults to port 0
    //Manual test - user selectable
    if ((flags & POST_AUTO) !=  POST_AUTO) {
        // 23HR has only one port SATA 0
        if (post_get_board_diags_type() != BCM958623HR ) {
            post_log("\n\nSelect SATA port to test:0 - iSATA ; 1 - eSATA \n");
            port = post_getUserInput("port #? (0/1) : ");
            if ( (port != 0) && ( port != 1) ) {
                printf("\nWrong port !!!");
                return -1;
            }
        }
    }

    if (POST_AUTO != (flags & POST_AUTO)) {
        snprintf(readbuffer, CONFIG_SYS_CBSIZE, "%u", initBlock);
        printf("\n");
        if (readline_into_buffer("Starting sector:", readbuffer, 0)) {
            result = strtouint(readbuffer, NULL, 10, &temp);
            if (0 == result) {
                initBlock = temp;
            }
        }
        snprintf(readbuffer, CONFIG_SYS_CBSIZE, "%u", blocksPerAccess);
        if (readline_into_buffer("Sectors per command:", readbuffer, 0)) {
            result = strtouint(readbuffer, NULL, 10, &temp);
            if (0 == result) {
                blocksPerAccess = temp;
                if (blocksPerAccess > 16384) {
                    post_log("Sectors per command must be < 32768; assuming 256\n");
                    blocksPerAccess = 256;
                }
                if (blocksPerAccess < 1) {
                    post_log("Must access at lest one sector per command; assuming 256\n");
                    blocksPerAccess = 256;
                }
            }
        }
        snprintf(readbuffer, CONFIG_SYS_CBSIZE, "%u", maxBytesPerTransfer);
        if (readline_into_buffer("Maximum bytes per AHCI memory access:", readbuffer, 0)) {
            result = strtouint(readbuffer, NULL, 10, &temp);
            if (0 == result) {
                maxBytesPerTransfer = temp;
            }
        }
        snprintf(readbuffer, CONFIG_SYS_CBSIZE, "%u", maxIterations);
        if (readline_into_buffer("Maximum iteration count:", readbuffer, 0)) {
            result = strtouint(readbuffer, NULL, 10, &temp);
            if (0 == result) {
                maxIterations = temp;
            }
        }
    } /* if (POST_AUTO != (flags & POST_AUTO)) */

    if (-1 == ahci_init(port)) {
        printf("\nPort init failed!!");
        return -1;
    }

    printf("Building the pattern...");
    for (index = 0; index < (BLOCK_PATTERN_COUNT << 16); index++) {
        value[index] = valueatindex(index);
    }
    printf(" %u doublebytes\n", index);

    printf("Flushing core and L2 cache to memory\n");
    flush_dcache_range((unsigned long)value, (unsigned long)&(value[index]));

    result = sata_post_test_sectors_write(port,
                                          value,
                                          initBlock,
                                          blocksPerAccess,
                                          maxBytesPerTransfer);
    if (result) {
        goto error;
    }

    bytesSoFar = 0;
    timeSoFar = 0;
    timeBias = get_timer(0);
    for (count = 1; count <= maxIterations; count++) {
        printf("\rIteration %d ", count);
        /* poison the memory so it's not correct values already */
        memset(value, 0xFF, (2 * BLOCK_PATTERN_COUNT << 16));
        /* ensure no writes pending to the buffer region */
        flush_dcache_range((unsigned long)&(value[0]),
                           (unsigned long)&(value[BLOCK_PATTERN_COUNT << 16]));
        index = 0;
        offset = initBlock;
        temp = blocksPerAccess;
        firstErr = 0;
        do {
            AhciFillCmdSlot(port,
                            0 /* tag */,
                            false /* write */,
                            true /* DMA */,
                            temp /* sectors */,
                            false /* NCQ */,
                            offset /* first sector */,
                            false /* reset */);
            result = AhciFilSg(port,
                               0 /* tag */,
                               (u32)&(value[index << 8]),
                               temp << 9,
                               maxBytesPerTransfer,
                               false /* blank */);
            if (result) {
                printf("error preparing SG list for read\n");
                goto error;
            }
            AhciIssueCmd(port, 0 /* tag */);
            result = wait_command_done(port,
                                       0 /* tag */,
                                       true /* DMA */,
                                       blocksPerAccess);
            if (result) {
                printf("error waiting for command to complete\n");
                goto error;
            }
            /* check results from the command */
            result = check_command_result(port,
                                          0 /* tag */,
                                          true /* DMA */,
                                          temp);
            if (result) {
                printf("error during read command for sectors %d..%d\n",
                       offset,
                       offset + temp - 1);
                goto error;
            }
            index += temp;
            offset += temp;
            if ((index + temp) > (BLOCK_PATTERN_COUNT << 8)) {
                temp = (BLOCK_PATTERN_COUNT << 8) - index;
            }
        } while (index < (BLOCK_PATTERN_COUNT << 8));
        timeCheck = get_timer(0);
        if (timeBias <= timeCheck) {
            timeSoFar += (timeCheck - timeBias);
        } else {
            timeSoFar += ((timeCheck - timeBias) + (u64)0x100000000ull);
        }
        timeBias = timeCheck;
        bytesSoFar += (BLOCK_PATTERN_COUNT << 17);
        result = sata_post_test_sectors_check(port, value, &firstErr);
        if (result) {
            printf("\rIteration %u, failed: %d miscompares\n", count, result);
            sata_post_test_sectors_dump(port,
                                        value,
                                        initBlock,
                                        blocksPerAccess,
                                        firstErr);
            /* check it again out of paranoia */
            sata_post_test_sectors_check(port, value, &firstErr);
            break;
        }
    } /* for (count = 1; count <= maxIterations; count++) */
    printf("\n%llu bytes in %llums; %llu bytes per second\n",
           bytesSoFar,
           timeSoFar,
           (bytesSoFar * 1000) / timeSoFar);
error:
    if (result) {
        result = -1;
    }
    return result;
}

int
sata_post_test_read(int flags)
{
    int port=0;
    int lba;
    int count;
    u64 bytesSoFar;
    u64 timeSoFar;
    u32 timeBias;
    u32 timeCheck;

    //22HR has a SATA sitting on the PCI bus
    //it is different from the native sata
    if (post_get_board_diags_type() == BCM958622HR) {
        printf("\nThis diag intended for NS+ only\n");
        return 2;
    }

    //Config IDM clock control, AHCI endianess
    config_AHCI();
    //Config PHY
    configure_SATA_PHY ();

    if (post_get_board_diags_type() & (BCM953025K|BCM953025K_REV2)) {
        if ((flags & POST_AUTO) !=  POST_AUTO) {
            post_log("\nPlease connect the SATA drive to the port to be tested,make sure it is powered on(0/1)");
            post_log("\n---Please power cycle the hard drive after switching ports---");
            post_getConfirmation("\nReady? (Y/N)");
        }
    }
    //Auto test still defaults to port 0
    //Manual test - user selectable
    if ((flags & POST_AUTO) !=  POST_AUTO) {
        // 23HR has only one port SATA 0
        if (post_get_board_diags_type() != BCM958623HR ) {
            post_log("\n\nSelect SATA port to test:0 - iSATA ; 1 - eSATA \n");
            port = post_getUserInput("port #? (0/1) : ");
            if ( (port != 0) && ( port != 1) ) {
                printf("\nWrong port !!!");
                return -1;
            }
        }
    }

    if (-1 == ahci_init(port)) {
        printf("\nPort init failed!!");
        return -1;
    }

    post_log("test: |");
    for (count = 0; count < SATA_DATA_TRANSFER_COUNT; count += 0x800) {
        post_log(" ");
    }
    post_log("|\rtest: |");
    timeSoFar = 0;
    bytesSoFar = 0;
    for (count = 0, lba = START_LBA;
         count < SATA_DATA_TRANSFER_COUNT;
         count++, lba += SATA_DATA_SECT_COUNT) {
        timeBias = get_timer(0);
        AHCI_DMA_read(port, 0, SATA_DATA_SECT_COUNT, lba, false);
        timeCheck = get_timer(0);
        if (timeBias <= timeCheck) {
            timeSoFar += (timeCheck - timeBias);
        } else {
            timeSoFar += ((timeCheck - timeBias) + (u64)0x100000000ull);
        }
        bytesSoFar += (SATA_DATA_SECT_COUNT * 512);
        if (0 == (count & 0x7FF)) {
            post_log(".");
        }
    }

    printf("\n%llu bytes in %llums; %llu bytes per second\n",
           bytesSoFar,
           timeSoFar,
           (bytesSoFar * 1000) / (timeSoFar));
    return 0;
}

int
sata_post_test_write(int flags)
{
    int port=0;
    int lba;
    int count;
    u64 bytesSoFar;
    u64 timeSoFar;
    u32 timeBias;
    u32 timeCheck;

    //22HR has a SATA sitting on the PCI bus
    //it is different from the native sata
    if (post_get_board_diags_type() == BCM958622HR) {
        printf("\nThis diag intended for NS+ only\n");
        return 2;
    }

    //Config IDM clock control, AHCI endianess
    config_AHCI();
    //Config PHY
    configure_SATA_PHY ();

    if (post_get_board_diags_type() & (BCM953025K|BCM953025K_REV2)) {
        if ((flags & POST_AUTO) !=  POST_AUTO) {
            post_log("\nPlease connect the SATA drive to the port to be tested,make sure it is powered on(0/1)");
            post_log("\n---Please power cycle the hard drive after switching ports---");
            post_getConfirmation("\nReady? (Y/N)");
        }
    }
    //Auto test still defaults to port 0
    //Manual test - user selectable
    if ((flags & POST_AUTO) !=  POST_AUTO) {
        // 23HR has only one port SATA 0
        if (post_get_board_diags_type() != BCM958623HR ) {
            post_log("\n\nSelect SATA port to test:0 - iSATA ; 1 - eSATA \n");
            port = post_getUserInput("port #? (0/1) : ");
            if ( (port != 0) && ( port != 1) ) {
                printf("\nWrong port !!!");
                return -1;
            }
        }
    }

    if (-1 == ahci_init(port)) {
        printf("\nPort init failed!!");
        return -1;
    }

    post_log("test: |");
    for (count = 0; count < SATA_DATA_TRANSFER_COUNT; count += 0x800) {
        post_log(" ");
    }
    post_log("|\rtest: |");
    timeSoFar = 0;
    bytesSoFar = 0;
    for (count = 0, lba = START_LBA;
         count < SATA_DATA_TRANSFER_COUNT;
         count++, lba += SATA_DATA_SECT_COUNT) {
        timeBias = get_timer(0);
        AHCI_DMA_Write(port, 0, SATA_DATA_SECT_COUNT, lba);
        timeCheck = get_timer(0);
        if (timeBias <= timeCheck) {
            timeSoFar += (timeCheck - timeBias);
        } else {
            timeSoFar += ((timeCheck - timeBias) + (u64)0x100000000ull);
        }
        bytesSoFar += (SATA_DATA_SECT_COUNT * 512);
        if (0 == (count & 0x7FF)) {
            post_log(".");
        }
    }

    printf("\n%llu bytes in %llums; %llu bytes per second\n",
           bytesSoFar,
           timeSoFar,
           (bytesSoFar * 1000) / (timeSoFar));
    return 0;
}

extern void pci_unregister_hoses(void);
extern void pci_iproc_set_port(int port);
static int run_pcie_sata(void)
{
    int j;
    struct pci_controller * hose;
    unsigned long  PORT_IN_USE = 0;

    //pci_unregister_hoses();
    //pci_iproc_init_board (PORT_IN_USE);
    pci_iproc_set_port(PORT_IN_USE);
    dumpSATAinfo (PORT_IN_USE);

    if ( -1 == pcie_ahci_init(0) )
    {
        printf("\nDevice Spinup failed!!");
        return -1;
    }
    printf("\nPassed\n");
    return 0;
}
#endif /* CONFIG_POST & CONFIG_SYS_POST_RGMII */
