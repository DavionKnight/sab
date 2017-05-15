#include <stdio.h>
#include <stdlib.h>
#include <string.h>		// for strcpy()

#include <sys/types.h>	// for open lseek
#include <sys/stat.h>	// for open
#include <fcntl.h>		// for open

#include <unistd.h>		// for lseek read write
#include "spidrv.h"

void usage(void)
{
	printf("dpll read <reg:hex> <len:dec>\n");
	printf("dpll write <reg:hex> <val:hex>\n");
	printf("dpll test\n");
}

void pdata(unsigned char *pdata, int count)
{
	int i;
	for (i = 0; i < count; i++) {
		printf(" %02x", pdata[i]);
	}
}


int main(int argc, char *argv[])
{
	int fd;
	int count = 0; 
	unsigned char data[32] = {0};
	int ret;
	unsigned short addr=0, len=0;


	if (argc != 2 && argc != 4) {
		usage();
		return 0;
	}
	
	if(-1 == spidrv_init())
	{
		printf("spidrv_init error\n");
		return 0;
	}
	if(-1 == gpiodrv_init())
	{
		printf("spidrv_init error\n");
		return 0;
	}

	char buf[10];
	
	dpll_read(0x00,buf, 2);
	printf("##### test: 0x%x 0x%x\n", buf[0], buf[1]);
	
	if (argc == 2 && argv[1][0] == 't') {
	//	selftest(fd);
	}
	else if (argc == 4 && argv[1][0] == 'r') 
	{
		sscanf(argv[2], "%hx", &addr);
		sscanf(argv[3], "%hd", &len);
		printf("read %d bytes at 0x%04x:", len, (unsigned short)addr);
		if(!dpll_read(addr,(unsigned char *)data, len))
		{
			pdata(data, len);
			printf(" done\n");
        } 
		else 
		{
            printf(" error\n");
        }
	}
	else if (argc == 4 && argv[1][0] == 'w') {
                sscanf(argv[2], "%hx", &addr);

                len = strlen(argv[3]);
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

                        len >>= 1;
                        printf("write %d bytes at 0x%04x:", len, (unsigned short)addr);
                        pdata(data, len);
                        if (!dpll_write(addr, data, len))
                                printf(" done\n");
			else
				printf(" error\n");
		}
	}
	else {
		usage();
	}

	
	return 0;
}

