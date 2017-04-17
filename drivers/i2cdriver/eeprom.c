//#include "sbchipc.h"
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <string.h>		// for strcpy()
//#include "spidrv.h"
/*#include <sys/types.h>	// for open lseek
#include <sys/stat.h>	// for open
#include <sys/time.h>	
#include <time.h>

#include <unistd.h>		// for lseek read write
*/
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <termios.h>
#include <pthread.h>

#include <sys/ipc.h>  
#include <sys/sem.h>  

#include "eepdrv.h"

int main(int argc, char *argv[])
{
	int i,value,ret;	
	int count = 0,flag=0; 
	unsigned int addr = 0;
	unsigned int offset = 0;
	unsigned char data[32] = {0};
	int chip_select = 0;

	int fd;
	unsigned int *virt_addr;
	/* mmap SERIAL_IO_SEL register */
	if ( ( fd = open ( "/dev/mem", O_RDWR | O_SYNC ) ) == -1 )
	{
		return -1;
	}
	virt_addr = (unsigned int *) mmap ( NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x18008000 );

	printf("------mapbase=%p------\n",virt_addr);

	printf("------i2c_init before!!!------\n");
	value = i2cdrv_init(virt_addr);
	printf("------i2c_init finished!!!------\n");


//value = eeprom_read(0x56,0x0c,data,2);
//i2c_read ( 0x51, 2, 0, data, 1);


//for (i = 0 ; i < count ; i++)
//	printf(" %02x",data[i]);
//printf(" \n");	


//return 0;
	if (argc == 5 && argv[1][0] == 'r') 
	{
		sscanf(argv[2], "%x", &addr);
		sscanf(argv[3], "%x", &offset);
		sscanf(argv[4], "%d", &count);

		printf("### addr = %x, offset = %d, count = %d\n", addr, offset, count);
	  
		value = eeprom_read(addr,offset,data,count);


		//i2c_read ( 0x51, 2, 0, data, 1);
		//i2c_read (0x56, 0xc, 0, data, 2 );

		//i2c_write (0x56, 0xc, 0, "a5a5", 2);

		
		for(i = 0; i < count; i++)
		{
			printf("0x%x ", data[i]);
		
		}
		printf("\n ***** read end ***** \n");




		printf("------eeprom_read finished!!!------\n");
		if(value)
			printf("fpga_spi_read failed!\n");		
		printf("read %d bytes at 0x%04x:", count, (unsigned short)addr);
		for (i = 0 ; i < count ; i++)
			printf(" %02x",data[i]);
		printf(" done\n");	
		
	}
	else if(argc == 4 && argv[1][0] == 'w') 
	{
		
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
	
			value = eeprom_write(addr,0x0,data,count);
			if(value)
				printf("eeprom i2c write failed!\n");
			for (i = 0 ; i < count ; i++)
				printf(" %02x",data[i]);
			printf(" done\n");			
		}
	}
//	mmap_exit();
	return 0;
	
}



