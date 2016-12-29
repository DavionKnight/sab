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
 * $Id$
 */
#include <stdint.h>

#include "shmoo_sig.h"

uint8_t shmoo_mem[] = {0x53,0x48,0x4D,0x4F,0x14,0xDC,0x00,0x00,0xB9,0x0b,0x00,0x00};


void shmoo_mem2sig(uint8_t *mem, struct shmoo_signature *sig)
{
    memcpy(sig->magic,mem,4);
    sig->dev_id = mem[4] | ((uint16_t)mem[5] << 8);
    sig->sku_id = mem[6] | ((uint16_t)mem[7] << 8);
    sig->ddr_type = mem[8] & 0x03;
    sig->ddr_clock = (mem[8] & 0xFC | ((uint16_t)mem[9] << 8)) >> 2;
}

void shmoo_sig2mem(struct shmoo_signature *sig, uint8_t *mem)
{
    memset(mem,0,10);
    memcpy(mem,sig->magic,4);
    mem[4] = sig->dev_id & 0xff;
    mem[5] = sig->dev_id >> 8;
    mem[6] = sig->sku_id & 0xff;
    mem[7] = sig->sku_id >> 8;
    mem[8] = sig->ddr_type;
    mem[8] |= (sig->ddr_clock << 2) & 0xFF;
    mem[9] |= sig->ddr_clock >> 6;
}

int shmoo_sigmemcmp(struct shmoo_signature *sig, uint8_t *mem)
{
    uint8_t tmp[10];
    shmoo_sig2mem(sig,tmp);
    return(memcmp(tmp,mem,sizeof(tmp)));
}


void shmoo_dump_sig(struct shmoo_signature *sig)
{
    printf("shmoo sig: magic:%c%c%c%c dev_id:%x sku_id:%x ddr_type:%x ddr_clock:%x\n",
        sig->magic[0], sig->magic[1], sig->magic[2], sig->magic[3],
        sig->dev_id,sig->sku_id,sig->ddr_type,sig->ddr_clock);
}
