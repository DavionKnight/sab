//#include "sbchipc.h"
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <string.h>		// for strcpy()
#include <sys/types.h>	// for open lseek
#include <sys/stat.h>	// for open
#include <sys/time.h>	
#include <time.h>

#include <unistd.h>		// for lseek read write

#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <termios.h>
#include <pthread.h>

#include <sys/ipc.h>  
#include <sys/sem.h>  

typedef struct{
	long 	addr;
	unsigned int  len;
	unsigned char buf[256];
}w25_rw_date_t;
/***********************macro definiton start***************************************/
#define W25_ERASE_CHIP          	9
#define W25_ERASE_SECTOR    		10
#define W25P16_READ		   			11
#define W25P16_WRITE	   			12
#define W25P1165_ID    				13
#define SPI_IOC_OPER_FLASH			14
#define SPI_IOC_OPER_FLASH_DONE		15

#define TEST_ADDR 					0x0
#define BLOCK_SIZE				 	8//16	
/***********************macro definiton  end ***************************************/

extern int spi_flag;
struct sembuf bufLock, bufUnlock;
int semid;

#define MAP_SIZE 4096UL
#define SPI_REGISTER_BASE 0x18000000
struct  timeval    tv;
//struct  timezone   tz;

int main(int argc, char *argv[])
{
	int i,value,value1,len=2,m=0,n=0,ret;
	unsigned char buf[32],buf1[8],dataout;	
	int count = 0,flag=0; 
	unsigned int addr = 0;
	unsigned char data[32] = {0};
	unsigned int  retlen;
/*	if(argc<2)
	{
		printf("Please input pof name");
		return 0;
	}*/
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

#if 1
	if (argc == 4 && argv[1][0] == 'r') 
	{
		sscanf(argv[2], "%x", &addr);
		sscanf(argv[3], "%d", &count);
	
		value = fpga_flash_read(addr, buf, count);		
		printf("read %d bytes at 0x%04x:", count, (unsigned short)addr);
		for (i = 0 ; i < count ; i++)
			printf(" %02x",buf[i]);
		printf(" done\n");

		printf("\n");		
		
	}
	else if(argc == 4 && argv[1][0] == 'w') 
	{
		sscanf(argv[2], "%x", &addr);	
		count = strlen(argv[3]);
 		
		if (count % 2 != 0) {
			printf("error: unaligned byte:hex\n");
		} else {
			int i; char *cp;
			unsigned char tmp;
			cp = argv[3];
			for(i = 0; *cp; i++, cp++) {
				tmp = *cp - '0';
				if(tmp > 9)
					tmp -= ('A' - '0') - 10;
				if(tmp > 15)
					tmp -= ('a' - 'A');
				if(tmp > 15) {
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
			value = fpga_flash_write(addr, data, count);
			printf(" done\n");	
			printf("\n");				
		}
	}
	else if(argc > 1 && argv[1][0] == 'e')
	{
		/*erase serial flash*/
		value = fpga_flash_erase();	
		if (value != 0)
		{
			printf("erase error\n");
			return 0;
		}
		sleep(1);
		printf("Erase chip successfully!\n");
	}
	//else if(argc > 1 && argv[1][0] == 'h')
	else
	{
		w25_rw_date_t  w25p16_date,rr;
		unsigned int  faddr = TEST_ADDR;		//,retlen;
		int fd_app,i=0;
		int pkt = 0;
///////////////////////////////////////////////////////////		gpio_set(0);
		if(argc <2)
		{
			printf("please input pof name!\n");
			return 0;
		}
		/*erase serial flash*/
		value = fpga_flash_erase();	
		if (value != 0)
		{
			printf("erase error\n");
			return 0;
		}
		sleep(1);
		printf("Erase chip successfully!\n");
		printf("fpga file is %s\nupdating...\n",argv[1]);
		fd_app = open(argv[1],O_RDWR);
		printf("file name: %s\n", argv[1]);
		while((len =read(fd_app,buf,BLOCK_SIZE))>0)
		{
	////////		ret = ioctl(fd, W25P16_WRITE, (unsigned long)&w25p16_date);
			value = fpga_flash_write(faddr, buf, len);
			if (value != 0)
			{
				printf("write error\n");
				break;
			}
			
			fpga_flash_read(faddr, data, BLOCK_SIZE);
			
			if(bcmp(data, buf, len) != 0)
			{
				//printf("###### error ######\n faddr = 0x%x\n", faddr);
				printf("buf: \n");
				for(i = 0; i < len; ++i)
				{
					printf("0x%x ", buf[i]);
				}
				printf("\ndata: \n");
				for(i = 0; i < len; ++i)
				{
					printf("0x%x ", data[i]);
				}
				printf("\n");
				
				return -1;
			}
				printf("buf: \n");
				for(i = 0; i < len; ++i)
				{
					printf("0x%x ", buf[i]);
				}
				printf("\ndata: \n");
				for(i = 0; i < len; ++i)
				{
					printf("0x%x ", data[i]);
				}
				printf("\n");
				
			faddr += len;//BLOCK_SIZE;
			//pkt++;
			for(i=0;i<100;i++);
//			usleep(1);
			//printf("pkt num = %d\n", pkt);
		}
		//fpga_flash_reconfig();
		printf("successfully!\n");
	}
#endif
//	gettimeofday(&tv,NULL);
//	printf("tv_sec:%d--tv_usec:%d\n",tv.tv_sec,tv.tv_usec);	
	return 0;
}



