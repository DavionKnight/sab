/*
 * @file	eeprom_api.c
 * @date	2015-12-31
 * @author   	zhangjj<zhangjj@huahuan.com>
 */


#include <stdio.h>
#include <linux/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "api.h"

#define I2C_BASE 	0x1000
#define I2C_RSV		(I2C_BASE + 0x0)
#define I2C_CMD		(I2C_BASE + 0x1)
#define I2C_DATA	(I2C_BASE + 0x40)

#define BUFSIZE	256

enum eeprom_addr{
	LOCALBOARD	= 0x00,
	BACKBOARD	= 0x50,
	FAN		= 0x51,
	PWR_A		= 0x52,
	PWR_B		= 0x53,
	CK01		= 0x4F	 
};

int i2c_cmdword_set(int fd, enum eeprom_addr i2c_addr, unsigned char rd_wr)
{
	int cmd_rsv = 0x00010000;
	struct fpga_msg msg;
	unsigned int cmd;

	memset(&msg, 0, sizeof(msg));
	msg.addr = I2C_RSV;
	msg.len = 4;
	memcpy(msg.buf,&cmd_rsv,sizeof(cmd_rsv));
	fpga_write(msg.addr,msg.buf,4,0xe);
	fpga_read(msg.addr,msg.buf,4,0xe);

	memset(&msg, 0, sizeof(msg));
	msg.addr = I2C_CMD;
	msg.len = 4;
	cmd = 0;
	cmd |= 0x00020000;
	cmd |= 1<<15;	//cmd enable
	cmd |= rd_wr ?(1<<14):0; /*0 write, 1 read*/
	cmd |= 1<<13;
	cmd |= 1<<12;
	cmd |= i2c_addr?(1<<11):0;
	i2c_addr &= 0x7;
	cmd |= i2c_addr << 8;

	memcpy(msg.buf, &cmd, 4);
	fpga_write(I2C_CMD, msg.buf, 4, 0xe);

	return 0;
}
int EepromWrite(enum eeprom_addr eep_addr,unsigned char *buf, unsigned short len)
{
	int fd,ret, i = 0, j = 0;
        struct fpga_msg msg;
	unsigned char *str = buf;
	
	if((len != 256)||!buf)
	{
		printf("para error\n");
		return -1;
	}

	for(i = 0; i<BUFSIZE/4; i++)	
	{
	        msg.addr = I2C_DATA+i;
	        msg.len = 4;
	        memcpy(msg.buf, str, 4);
	        fpga_write(msg.addr, msg.buf, 4, 0xe);

		str += 4;
	}
	ret = i2c_cmdword_set(fd, eep_addr, 0);
	usleep(300000);
	return ret;
}

int EepromRead(enum eeprom_addr eep_addr,unsigned char *buf, unsigned short len)
{
        int fd,ret = 0, i = 0;
        struct fpga_msg msg;
	unsigned char *str = buf;

        if((len != 256)||!buf)
        {
                printf("para error\n");
                return -1;
        }

        ret = i2c_cmdword_set(fd, eep_addr, 1);
	usleep(300000);
        for(i = 0; i<BUFSIZE/4; i++)
        {
                msg.addr = I2C_DATA+i;
                msg.len = 4;
                fpga_read(msg.addr, msg.buf, 4, 0xe);
	//	printf("buf[i+0]=0x%02x,buf[i+1]=0x%02x,buf[i+2]=0x%02x,buf[i+3]=0x%02x", msg.buf[0], msg.buf[1], msg.buf[2], msg.buf[3]);
                memcpy(str, msg.buf, 4);
		str += 4;
        }

        return ret;
}

#if 1
int main(int argc, char *argv[])
{
	unsigned char buf[BUFSIZE];
	int i=0,ret = 0;
	unsigned char eepaddr = BACKBOARD;
	if(-1 == spidrv_init())
	{
		printf("spidrv_init error\n");
		return 0;
	}
	if (argc == 3 && argv[1][0] == 'r') 
	{
		sscanf(argv[2], "%hhx", &eepaddr);
		printf("addr = 0x%02x\n",eepaddr);
		memset(buf, 0, BUFSIZE);
		ret = EepromRead(eepaddr, buf, BUFSIZE);
		for(i=0;i<256;i++)
		{
			if(!(i%16))
				printf("\n");	
			printf("0x%02x ",buf[i]);
		}
		printf("\n");	

	}
	else if (argc == 3 && argv[1][0] == 'w') 
	{
		sscanf(argv[2], "%hhx", &eepaddr);
		printf("addr = 0x%02x\n",eepaddr);
		memset(buf, 0, BUFSIZE);
		for(i=0;i<256;i++)
			buf[i] = 0xff-i;
		ret = EepromWrite(eepaddr, buf, BUFSIZE);
	}
	else if (argc == 3 && argv[1][0] == 'c') 
	{
		sscanf(argv[2], "%hhx", &eepaddr);
		printf("addr = 0x%02x\n",eepaddr);
		memset(buf, 0, BUFSIZE);
		for(i=0;i<256;i++)
			buf[i] = 0;
		ret = EepromWrite(eepaddr, buf, BUFSIZE);
	}
	else
	{
		printf("eeprom r|w|c {addr}\n");
		printf("eeprom r 50\n\n");
		printf("addr like:\n");
		printf("LOCALBOARD	= 0x00\n");
		printf("BACKBOARD	= 0x50\n");
		printf("FAN		= 0x51\n");
		printf("PWR_A		= 0x52\n");
		printf("PWR_B		= 0x53\n");
		printf("CK01		= 0x4F\n");
	}	

	return 0;
}
#endif
