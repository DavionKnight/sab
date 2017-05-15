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
	unsigned short addr = 0;//,len;
	unsigned char data[32] = {0};
	unsigned char slot_num = 0;
	int i = 0;	
	int fd = 0;

struct stat buf;
char dat[20];
int len = 0, filelength = 0;
	
	
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
		fpga_flash_read(addr, (unsigned char *)data, len, 0);
		printf("The result:\n");
		for(i = 0; i < len; i++)
		{	
			printf("0x%02x ",data[i]);
			if((i+1)%16 == 0)
				printf("\n");
		}
		printf("\n");
	}
	else if (argc == 4 && argv[1][0] == 'w')
	{
		char data[20];
		sscanf(argv[2], "%hx", &addr);
		sprintf(data, "./%s", argv[3]);
		
		printf("file path: %s\n", data);
		if(fd = open(data, O_RDONLY) > 0)
		{

			
			if(stat(data, &buf) == 0)
			{
				filelength = (int)buf.st_size;
			}
			printf("filelength = %d byte\n", filelength);

			if(filelength <= 0)
			{
				return -1;
			}

			fpga_flash_erase();
			len = 0;
			while(len < filelength)
			{
				len = read(fd, dat, 16);
				fpga_flash_write(addr, dat, len, 0); 
				
				addr += len;
				len += len;
				printf("##### len = %d, addr = 0x%x", len, addr);
			}
			close(fd);
			//fpga_flash_reconfig ();
		}
		else
		{
			//printf("write addr 0x%x data 0x%04x\n", addr, data);
			//fpga_write(addr, (unsigned char *)&data, sizeof(data), 0); 
			//printf("open file error");
			perror("open file error: ");
		}
	}
	else
	{
		printf("\nfpgaflash read <addr:hex> <len:dec>\n");
		printf("fpgaflash write <addr:hex> <data:file name>\n\n");
		printf("demo:	fpgaflash read 0x0 2\n");
		printf("	fpgaflash write 0x00 fpga.bin\n");
	}
	
	spidrv_exit();

	return 0;	
}



