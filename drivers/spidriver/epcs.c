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

#define BLOCK_SIZE	32	

int file_write(int fd_pof)
{
	int fd,len,ret=0;
	unsigned char buf[65] = {0};	
	unsigned int  faddr = 0;

	while((len =read(fd_pof, buf,BLOCK_SIZE))>0)
        {

		fpga_flash_write(faddr, buf, BLOCK_SIZE);
		if (ret != 0)
		{
			printf("write error\n");
			break;
		}
		faddr += BLOCK_SIZE;
#if 0
		j++;
		if((j+1)%100==0)
			printf("j = %d\n",j+1);	
#endif
		usleep(10);
	}
	return ret;
}


int main(int argc, char *argv[])
{
	int fd_app;
	int ret;

	if(argc<2)
	{
		printf("Please input pof name");
		return 0;
	}
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
	ret = fpga_flash_erase();
	if(ret < 0)
	{
		printf("erase error\n");
		return -1;
	}
	printf("fpga file is %s\nupdating..\n",argv[1]);
	fd_app = open(argv[1],O_RDWR);

	if(file_write(fd_app)<0)
	{
		printf("error\n");
	}
	else
		printf("update fpga program successfully!\n");	
	fpga_flash_reconfig();
	close(fd_app);
	return 0;
}



