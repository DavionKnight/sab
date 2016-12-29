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
// endian agnostic wrapper of the shmoo signature which used to
// be a bitfield
struct shmoo_signature {
    char            magic[4];
    uint16_t        dev_id;
    uint16_t        sku_id;
    uint8_t         ddr_type;
    uint16_t        ddr_clock;
};

void shmoo_mem2sig(uint8_t *mem, struct shmoo_signature *sig);
void shmoo_sig2mem(struct shmoo_signature *sig, uint8_t *mem);
int shmoo_sigmemcmp(struct shmoo_signature *sig, uint8_t *mem);
void shmoo_dump_sig(struct shmoo_signature *sig);
