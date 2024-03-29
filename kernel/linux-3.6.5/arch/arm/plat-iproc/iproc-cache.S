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
#include <linux/linkage.h>
#include <linux/init.h>

	__INIT

/*
 *   The secondary kernel init calls v7_flush_dcache_all before it enables
 *   the L1; however, the L1 comes out of reset in an undefined state, so
 *   the clean + invalidate performed by v7_flush_dcache_all causes a bunch
 *   of cache lines with uninitialized data and uninitialized tags to get
 *   written out to memory, which does really unpleasant things to the main
 *   processor.  We fix this by performing an invalidate, rather than a
 *   clean + invalidate, before jumping into the kernel.
 */

ENTRY(v7_invalidate_l1)
	mov     r0, #0
	mcr     p15, 2, r0, c0, c0, 0
	mrc     p15, 1, r0, c0, c0, 0

	ldr     r1, =0x7fff
	and     r2, r1, r0, lsr #13

	ldr     r1, =0x3ff

	and     r3, r1, r0, lsr #3	@ NumWays - 1
	add     r2, r2, #1		@ NumSets

	and     r0, r0, #0x7
	add     r0, r0, #4		@ SetShift

	clz     r1, r3			@ WayShift
	add     r4, r3, #1		@ NumWays
1:	sub     r2, r2, #1		@ NumSets--
	mov     r3, r4			@ Temp = NumWays
2:	subs    r3, r3, #1		@ Temp--
	mov     r5, r3, lsl r1
	mov     r6, r2, lsl r0
	orr     r5, r5, r6		@ Reg = (Temp<<WayShift)|(NumSets<<SetShift)
	mcr     p15, 0, r5, c7, c6, 2
	bgt     2b
	cmp     r2, #0
	bgt     1b
	dsb
	isb
	mov     pc, lr
ENDPROC(v7_invalidate_l1)

/*
 * Invalidate the cache without flushing the contents of all
 * cache levels. This code is copied from v7_flush_dcache_all
 * from cache-v7.S
 */
ENTRY(__v7_invalidate_dcache_all)
	dmb					@ ensure ordering with previous memory accesses
	mov	r8, r1				@ Save machine ID
	mov	r13, r2				@ Save atags pointer
	mrc     p15, 1, r0, c0, c0, 1		@ read clidr
	ands    r3, r0, #0x7000000		@ extract loc from clidr
	mov     r3, r3, lsr #23			@ left align loc bit field
	beq     finished			@ if loc is 0, then no need to clean
	mov     r10, #0				@ start clean at cache level 0
	loop1:
	add     r2, r10, r10, lsr #1		@ work out 3x current cache level
	mov     r1, r0, lsr r2			@ extract cache type bits from clidr
	and     r1, r1, #7			@ mask of the bits for current cache only
	cmp     r1, #2				@ see what cache we have at this level
	blt     skip				@ skip if no cache, or just i-cache
	mcr     p15, 2, r10, c0, c0, 0		@ select current cache level in cssr
	isb					@ isb to sych the new cssr&csidr
	mrc     p15, 1, r1, c0, c0, 0		@ read the new csidr
	and     r2, r1, #7			@ extract the length of the cache lines
	add     r2, r2, #4			@ add 4 (line length offset)
	ldr     r4, =0x3ff
	ands    r4, r4, r1, lsr #3		@ find maximum number on the way size
	clz     r5, r4				@ find bit position of way size increment
	ldr     r7, =0x7fff
	ands    r7, r7, r1, lsr #13		@ extract max number of the index size
loop2:
	mov     r9, r4				@ create working copy of max way size
loop3:
 ARM(   orr     r11, r10, r9, lsl r5    )	@ factor way and cache number into r11
 THUMB( lsl     r6, r9, r5              )
 THUMB( orr     r11, r10, r6            )	@ factor way and cache number into r11
 ARM(   orr     r11, r11, r7, lsl r2    )	@ factor index number into r11
 THUMB( lsl     r6, r7, r2              )
 THUMB( orr     r11, r11, r6            )	@ factor index number into r11
	mcr     p15, 0, r11, c7, c6, 2		@ invalidate line
	subs    r9, r9, #1			@ decrement the way
	bge     loop3
	subs    r7, r7, #1			@ decrement the index
	bge     loop2
skip:
	add     r10, r10, #2			@ increment cache number
	cmp     r3, r10
	bgt     loop1
finished:
	mov     r10, #0				@ swith back to cache level 0
	mcr     p15, 2, r10, c0, c0, 0		@ select current cache level in cssr
	dsb
	isb
	mov	r1, r8				@ restore machine ID
	mov	r2, r13				@ restore atags pointer
	mov     pc, lr				@ return
ENDPROC(__v7_invalidate_dcache_all)
