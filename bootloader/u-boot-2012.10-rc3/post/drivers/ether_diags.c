/*
 * (C) Copyright 2002
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

#include <common.h>

/*
 * Ethernet test
 *
 * The Serial Communication Controllers (SCC) listed in ctlr_list array below
 * are tested in the loopback ethernet mode.
 * The controllers are configured accordingly and several packets
 * are transmitted. The configurable test parameters are:
 *   MIN_PACKET_LENGTH - minimum size of packet to transmit
 *   MAX_PACKET_LENGTH - maximum size of packet to transmit
 *   TEST_NUM - number of tests
 */

#include <post.h>

#if CONFIG_POST & CONFIG_SYS_POST_ETHER

#include <common.h>
#include <net.h>
#include <asm/arch/ethHw.h>

#undef BCMIPROC_ETH_DEBUG

#ifdef BCMIPROC_ETH_DEBUG

#ifdef CONFIG_RANDOM_MACADDR
#define PKT_MIN_LEN             ((rand() % 1455) + 60) /* 60 ~ 1514 */
#else
#define PKT_MIN_LEN             60
#endif /* CONFIG_RANDOM_MACADDR */
#define PKT_MAX_LEN             PKT_MIN_LEN

#else

#define PKT_MIN_LEN             60
#define PKT_MAX_LEN             1514

#endif /* BCMIPROC_ETH_DEBUG */


extern uint32_t crc32(uint32_t, const unsigned char *, unsigned int);


static int
packet_fill(uint8_t *pkt, int len)
{
    int idx;
    uint32_t crc;
    
    for (idx = 0; idx < 6; idx++) {
        pkt[idx] = 0xff;
    }
	eth_getenv_enetaddr("ethaddr", &pkt[idx]);
    pkt[12] = 0x08;
    pkt[13] = 0x00;
    pkt[14] = 0x00;
    pkt[15] = 0x00;
    pkt[16] = 0x00;
    pkt[17] = 0x00;

    for (idx = 18; idx < len; idx++) {
#ifdef CONFIG_RANDOM_MACADDR
        pkt[idx] = (uint8_t)rand();
#else
        pkt[idx] = (uint8_t)(idx - 17);
#endif /* CONFIG_RANDOM_MACADDR */
    }
    
    /* Fill the CRC value into byte 14, 15, 16 and 17 */
    crc = crc32(0, pkt, len);
    pkt[14] = (uint8_t )(crc >> 24);
    pkt[15] = (uint8_t )(crc >> 16);
    pkt[16] = (uint8_t )(crc >> 8);
    pkt[17] = (uint8_t )(crc >> 0);
    
    return 0;
}

static int 
packet_check(uint8_t *pkt, int len)
{
    uint32_t crc;
    
    crc = ((pkt[14] << 24) | 
           (pkt[15] << 16) | 
           (pkt[16] << 8) | 
           (pkt[17] << 0));
    
    pkt[14] = 0;
    pkt[15] = 0;
    pkt[16] = 0;
    pkt[17] = 0;

    if (crc != crc32(0, pkt, len)) {
        return -1;
    }
    
    return 0;
}

static int
loopback_test(struct eth_device *dev, int length, int no_crc)
{
    uint8_t txPkt[PKT_MAX_LEN + 4];
    uint8_t *rxPkt = NULL;
    int txLen = length;
    int rxLen;
    int idx = dev->index;
#ifdef BCMIPROC_ETH_DEBUG
    int bdx;
#else
    int repeat;
#endif /* BCMIPROC_ETH_DEBUG */
   
    /* Fill content of the test packet */
    memset(txPkt, 0, sizeof(uint8_t)*(PKT_MAX_LEN + 4));
    packet_fill(txPkt, txLen);

    if (dev->init(dev, NULL)) {
        post_log("ERR: open the Ethernet device %d failed.\n", idx);
        return -1;
    }
   
#ifdef BCMIPROC_ETH_DEBUG
    ethHw_dump(idx, DUMP_REGS);
    /* ethHw_dump(idx, DUMP_TX_MIB); */
    /* ethHw_dump(idx, DUMP_RX_MIB); */
       
    printf("Send packet, length:%d\n", txLen);
    for(bdx = 0 ; bdx < txLen; bdx++) {
        printf("%.2x ", txPkt[bdx]);
        if ((bdx % 8) == 7) {
            printf("\n");
        }
    }
    printf("\n");
#endif /* BCMIPROC_ETH_DEBUG */
        
    /* Load buf and start transmit */
    if (ethHw_dmaTx(idx, txPkt, txLen)) {
        post_log("ERR: send a packet (len:%d) via Ethernet device %d failed.\n", txLen, idx);
        return -1;
    }
    
    if (ethHw_dmaTxWait(idx)) {
        post_log("ERR: send a packet (len:%d) via Ethernet device %d failed.\n", txLen, idx);
        return -1;
    }
    
#ifdef BCMIPROC_ETH_DEBUG
    /* Delay 1 seconds for packet receive */
    udelay(1000000);
    if (ethHw_dmaRx(idx, rxPkt, (size_t *)&rxLen)) {
        /* ethHw_dump(idx, DUMP_REGS); */
        ethHw_dump(idx, DUMP_TX_MIB);
        ethHw_dump(idx, DUMP_RX_MIB);
        post_log("ERR: receive the packet from Ethernet device %d failed.\n", idx);
        return -1;
    }
    
    /* ethHw_dump(idx, DUMP_REGS); */
    ethHw_dump(idx, DUMP_TX_MIB);
    ethHw_dump(idx, DUMP_RX_MIB);
    
    printf("Rx packet, length: %d\n", rxLen);
    for(bdx = 0 ; bdx < rxLen; bdx++) {
        printf("%.2x ", rxPkt[bdx]);
        if ((bdx % 8) == 7) {
            printf("\n");
        }
    }
    printf("\n");
#else
    /* Start receive the loopback packet */
    repeat = 10;
    do {
        if (ethHw_dmaRx(idx, rxPkt, (size_t *)&rxLen)) {
            udelay(100);
            repeat--;
        } else {
            break;
        }
    } while (repeat);
    
    if (!repeat) {
        post_log("ERR: receive the packet from Ethernet device %d failed.\n", idx);
        return -1;
    }
#endif /* BCMIPROC_ETH_DEBUG */

    if (!no_crc) {
        rxLen -= 4;
    }
    
    if (rxLen != txLen) {
        post_log("ERR: Receive packet length is not correct (rxLen:%d, txLen:%d)\n", (!no_crc) ? (rxLen + 4): rxLen, txLen);
        return -1;
    }
            
    /* Verify the receive packet */    
    if (packet_check(rxPkt, rxLen)) {
        post_log("ERR: check the receive packet crc failed.\n");
        return -1;
    }
            
    post_log("Ethernet loopback test in length %d successful\n", txLen);

    dev->halt(dev);
    return 0;
}

int 
ether_post_test(int flags)
{
    struct eth_device *dev = NULL;
    int length, idx;
    int rc = 0;

#ifdef CONFIG_RANDOM_MACADDR
    srand(get_timer(0));
#endif /* CONFIG_RANDOM_MACADDR */
   
    for (idx = 0; idx < CONFIG_GMAC_NUM; idx++) {
        dev = eth_get_dev_by_index(idx);
        if (!dev) {
            post_log("ERR: retrieval Ethernet device %d failed.\n", idx);
            return -1;
        }
    
        dev->halt(dev);
            
        /****************************************/
        /* Start to test the DMA loopback       */
        /****************************************/
        /* Enable the DMA loopback test */
        ethHw_dmaLoopbackSet(idx, 1);
        post_log("\nStart the DMA loopback test on eth%d device.\n", idx);
        length = PKT_MIN_LEN;
        do {
            if (loopback_test(dev, length, 1)) {
                dev->halt(dev);
                rc = -1;
                break;
            }
#ifdef BCMIPROC_ETH_DEBUG
            break;
#endif /* BCMIPROC_ETH_DEBUG */
        } while (++length <= PKT_MAX_LEN);

        /* Disable the DMA loopback test */
        ethHw_dmaLoopbackSet(idx, 0);

        /****************************************/
        /* Start to test the MAC loopback       */
        /****************************************/
        /* Enable the MAC loopback test */
        post_log("\nStart the DMA loopback test on eth%d device.\n", idx);
        ethHw_macLoopbackSet(idx, 1);

        length = PKT_MIN_LEN;
        do {
            if (loopback_test(dev, length, 0)) {
                dev->halt(dev);
                rc = -1;
                break;
            }
#ifdef BCMIPROC_ETH_DEBUG
            break;
#endif /* BCMIPROC_ETH_DEBUG */
        } while (++length <= PKT_MAX_LEN);

        /* Disable the MAC loopback test */
        ethHw_macLoopbackSet(idx, 0);
    }

    return rc;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_ETHER */
