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
#include <net/if.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <errno.h>
#include <linux/sockios.h>
#include <linux/netlink.h>

#define ETCPHYRD	9
#define SIOCGETCPHYRD		(SIOCDEVPRIVATE + ETCPHYRD)

int main(int argc, char *argv[])
{
	int ret = 0;
	unsigned char addr = 0,len;
	unsigned int data = 0;
	unsigned char slot_num = 0;
	int i = 0;	
    struct ifreq ifr;
    int error;
    int setval[2]={0};
	int sockfd = 0;

	sockfd = socket ( AF_INET, SOCK_DGRAM, 0 );

	if (argc == 2)
	{
			sscanf(argv[1], "%hx", &addr);
#if 0
			printf("0 %s\n",argv[0]);
#endif
			printf("addr 0x%02x :", addr);

			strcpy ( ifr.ifr_name, "eth0");

			ifr.ifr_data = ( void * ) setval;
			error = ioctl (sockfd, SIOCGETCPHYRD, &ifr);
			if ( error < 0 )
			{
					printf ( "eth0_get_port:  error=%x errno=%x\n", error, errno );
					return 0;
			}
			printf(" 0x%04x\n",setval[1]);
	}



	else if (argc == 5 && argv[1][0] == 'w') {
#if 0
		unsigned int data;
		sscanf(argv[2], "%hhx", &slot_num);
		sscanf(argv[3], "%hx", &addr);
		sscanf(argv[4], "%x", &data);
		printf("slot num %x,write addr 0x%x data 0x%04x\n", slot_num, addr, data);
		fpga_write(addr, (unsigned char *)&data, sizeof(data), slot_num); 
#endif
	}
	else
	{
		printf("\nvitess  <addr:hex> \n");
		printf("vitess  <addr:hex> <data:hex>\n\n");
	}
	

	return 0;	
}



