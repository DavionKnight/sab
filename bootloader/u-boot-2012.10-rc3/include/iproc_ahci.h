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

extern int ahci_reset_controller(void);
extern void ahci_init_controller(void);
extern int SpinUpAndWait (int port, int speed);
extern void ahci_port_clear_all_serr(int port_no);
extern void ahci_port_clear_all_int(int port_no);
extern void ahci_clear_global_int_stat(void);
extern void AhciFillCmdSlot(int port, int tag, int w, int pio, int blk, int ncq, u32 lba, int rst);
extern int AhciFilSg(int port, int tag, u32 addr ,int byteCount, int bytesPerTransfer, int blank);
extern void AhciIssueCmd(int port, int tag);
extern int wait_command_done(int port, int tag, int pio, int blk);
extern int check_command_done(int port, int tag, int pio, int blk);
extern int check_command_result(int port, int tag, int pio, int blk);

void AhciPrepCmd(int port,int tag,int w,int pio,int blk,u32 lba,int rst, void* buffer, int blank);
void AhciIssueCmd(int port, int tag);

int ahci_reset_controller(void);
void ahci_enable_ahci(void);
void ahci_port_clear_all_serr(int port_no);
void ahci_port_clear_all_int(int port_no);
void ahci_global_clear_all_int(void);
void ahci_clear_global_int_stat(void);
void delay_ms(int ms);
u32 ata_wait_register( u32 reg, u32 mask, u32 val);
int SpinUpAndWait (int port, int speed);

int CmpBuf(int port, int tag, int blkCnt);
int wait_command_done(int port, int tag, int pio, int blk);

void WaitGlobalInt(int port);
int ahci_init_one(int speed);

void txData0Out(char data);
