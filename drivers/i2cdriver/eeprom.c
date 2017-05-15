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
	unsigned int chip = 0;
	unsigned char data[32] = {0};
	int chip_select = 0;

	int fd;
	unsigned int *virt_addr;
	/* mmap SERIAL_IO_SEL register */
	if ( ( fd = open ( "/dev/mem", O_RDWR | O_SYNC ) ) == -1 )
	{
		return -1;
	}


	sscanf(argv[2], "%d", &chip_select);
	if(chip_select == 1)
	{
		virt_addr = (unsigned int *) mmap ( NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x18008000 );
	}
	else
	{
		virt_addr = (unsigned int *) mmap ( NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x1800B000 );
		
	}

	printf("##### mapbase=%p, chip_select = %d\n", virt_addr, chip_select);

	value = i2cdrv_init(virt_addr);

	if (argc == 6 && argv[1][0] == 'r') 
	{
		sscanf(argv[3], "%d", &chip);
		sscanf(argv[4], "%d", &addr);
		sscanf(argv[5], "%d", &count);
		
	  	
		value = eeprom_read(chip,addr,data,count);
		printf("------eeprom_read finished!!!------\n");
		if(value)
			printf("fpga_spi_read failed!\n");		
		printf("read %d bytes at 0x%04x:", count, (unsigned short)addr);
		for (i = 0 ; i < count ; i++)
			printf(" %02x",data[i]);
		printf(" done\n");	
		
	}
	else if(argc == 6 && argv[1][0] == 'w') 
	{
		sscanf(argv[3], "%d", &chip);
		sscanf(argv[4], "%d", &addr);	
		count = strlen(argv[5]);
		if (count % 2 != 0) 
		{
			printf("error: unaligned byte:hex\n");
		} 
		else 
		{
			int i; char *cp;
			unsigned char tmp;
			cp = argv[5];
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
	
			value = eeprom_write(chip,addr,data,count);
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



