/**********************************************
 * @file	rfpga.c
 * @author	zhangjj <zhangjj@bjhuahuan.com>
 * @date	2015-08-20
 *********************************************/

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <string.h>
#include "spidrv.h"


int main(int argc, char *argv[])
{
	int ret = 0;
	unsigned short addr = 0,len;
	unsigned char data[32] = {0};
	unsigned char slot_num = 0;
	int i = 0;	

	if(-1 == spidrv_init())
	{
		printf("spidrv_init error\n");
		return 0;
	}
	if(-1 == gpiodrv_init())
	{
		printf("gpiodrv_init error\n");
		return 0;
	}

	if (argc == 4 && argv[1][0] == 'r') {
		sscanf(argv[2], "%hx", &addr);
		sscanf(argv[3], "%hd", &len);
#if 0
		printf("0 %s\n",argv[0]);
		printf("1 %s\n",argv[1]);
		printf("2 %s\n",argv[2]);
#endif
		printf("addr 0x%04x len %d:\n", (unsigned short)addr, len);
		fpga_read(addr, (unsigned char *)data, len, 0);
		printf("The result:\n");
		for(i = 0; i < len; i++)
		{	
			printf("0x%02x ",data[i]);
			//if((i+1)%16 == 0)
			//	printf("\n");
		}
		printf("\n");
	}
	else if (argc == 4 && argv[1][0] == 'w') {
		/*unsigned int data;
		sscanf(argv[2], "%hx", &addr);
		sscanf(argv[3], "%x", &data);
		printf("write addr 0x%x data 0x%04x\n", addr, data);
		fpga_write(addr, (unsigned char *)&data, sizeof(data), 0); */
		int count = 0, value = 0;
		sscanf(argv[2], "%x", &addr);	
		count = strlen(argv[3]);
 		
		if (count % 2 != 0) 
		{
			printf("error: unaligned byte:hex\n");
		} 
		else 
		{		
			int i; char *cp;
			unsigned char tmp;
			cp = argv[3];
			for(i = 0; *cp; i++, cp++) 
			{
				tmp = *cp - '0';
				if(tmp > 9)
					tmp -= ('A' - '0') - 10;
				if(tmp > 15)
					tmp -= ('a' - 'A');
				if(tmp > 15) 
				{
					printf("Hex conversion error on %c, mark as 0.\n", *cp);
					tmp = 0;
				}
				if((i % 2) == 0)
					data[i / 2] = (tmp << 4);
				else
					data[i / 2] |= tmp;
			}

			count >>= 1;
			printf("write %d bytes at 0x%04x:", count, (unsigned short)addr);			
			for (i = 0 ; i < count ; i++)
				printf(" %02x",data[i]);
			fpga_write(addr, data, count, 0);
			//fpga_write(addr, (unsigned char *)&data, sizeof(data), 0);
			printf(" done\n");	
			printf("\n");						
		}		
	}
	else
	{
		printf("\nfpga read <addr:hex> <len:dec>\n");
		printf("fpga write <addr:hex> <data:hex>\n\n");
		printf("demo:	fpga read 0x0 2\n");
		printf("	fpga write 0x85 0xee\n");
	}
	
	spidrv_exit();

	return 0;	
}



