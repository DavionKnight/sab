
/*
 * Copyright (C) 2016 Huahuan Corporation. All rights reserved.
 * 
 * writen by tianzhy 2016-10
 *
 *  
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <errno.h>
#include <linux/sockios.h>

#define BCM53101_A 0
#define BCM53101_B 1
#define BCM53101_C 2

struct bcm53101_t{
	unsigned char  which_sw;
        unsigned char  page;
        unsigned char  addr;
        unsigned short value[4];
}__attribute__((packed));

enum {
    HslGoodPktsRcv = 1, //收报文个数 used
    HslGoodOctetsRcv = 2, //收报文字节数 used
    HslInDiscards = 3,//收报文丢弃个数
    HslUcPktsRcv = 4,//收报文广播个数
    HslMcPktsRcv = 5,//收报文组播个数
    HslBcPktsRcv = 6,
    HslBadCrc = 7, /* used as rx error */
    HslUndersize = 8,
    HslOversize = 9,
    HslGoodPktsSent = 10,//发报文个数 used
    HslGoodOctetsSent = 11, //发报文字节数 used
    HslUcPktsSent = 12,//发报文广播个数
    HslMcPktsSent = 13,//发报文广播个数
    HslBcPktsSent = 14,
    HslErrSent = 15, /* used */
    HslOutDiscards = 16,//发报文个数
    Hslprotocoldroppkt = 17,
    HslStatTypeCountMax
 };
struct outband_counter_t{
	/*Rx*/
	unsigned int goodpktsRx;
	unsigned int goodOctetsRx;
	unsigned int discardPktsRx;
	unsigned int UcPktsRx;
	unsigned int McPktsRx;
	unsigned int BcPktsRx;
	unsigned int badCrcPktsRx;
	unsigned int underSizePktsRx;
	unsigned int overSizePktsRx;
	/*Tx*/
	unsigned int goodPktsTx;
	unsigned int goodOctetsTx;
	unsigned int UcPktsTx;
	unsigned int McPktsTx;
	unsigned int BcPktsTx;
	unsigned int ErrPktsTx;
	unsigned int OutDiscardsPktsTx;
	unsigned int ProtocolDropPktsTX;
	unsigned int StatTypeCountMaxTx;
};
struct outband_counter_t outband_counter = {0}; 

enum IFNET_SPEED
{
    IFNET_SPEED_INVALID = 0,
    IFNET_SPEED_GE,
    IFNET_SPEED_FE,
};
 
 enum IFNET_SPEED_MODE
{
    IFNET_SPEED_MODE_INVALID = 0,
    IFNET_SPEED_MODE_AUTO,
    IFNET_SPEED_MODE_FORCE
};

 enum IFNET_DUPLEX
{
    IFNET_DUPLEX_INVALID = 0,
    IFNET_FULLDUPLEX,
    IFNET_HALFDUPLEX
};

/* forcespeed values */
#define ET_AUTO    0x7f 
#define ET_10HALF   0
#define ET_10FULL   1
#define ET_100HALF  2
#define ET_100FULL  3
#define ET_1000HALF 4
#define ET_1000FULL 5

#define OUTBAND_STRING "eth0"

#define CLEAR_COUNTER_FLAG 	0x0100
#define GET_COUNTER_FLAG	0x1000
#define GET_SPEED_FLAG		0x80

static int sockfd;
int bcmfd;

int hsl_outband_eth0_init(void)
{
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(sockfd < 0)
        {
                printf("can't open socket eth0......\n");
                return -1;
        }
	bcmfd = open( "/dev/bcm53101", O_RDWR);
	if(bcmfd == -1)
		return -1;
        return 0;
}
	
/*the read length actually is 8 bytes
but we just use the low 4bytes */
int bcm53101_read(unsigned char which_sw, unsigned char page, unsigned char addr, unsigned int *value)
{
	int ret = 0;
	struct bcm53101_t bcmstr;
	
	bcmstr.which_sw = which_sw;
	bcmstr.page= page;
	bcmstr.addr= addr;
	ret = read(bcmfd, &bcmstr, sizeof(bcmstr));	
	if(ret < 0)
		return ret;

	*value = bcmstr.value[1]<<16 | bcmstr.value[0];
//	printf("addr=%x,*value=%x\n",addr,*value);

	return 0;
}
int bcm53101_write(unsigned char which_sw, unsigned char page, unsigned char addr, unsigned int *value)
{
	int ret = 0;
	struct bcm53101_t bcmstr={0};
	
	bcmstr.which_sw = which_sw;
	bcmstr.page= page;
	bcmstr.addr= addr;
	bcmstr.value[1] = (*value>>16)&0xffff;
	bcmstr.value[0] = *value&0xffff;

	ret = write(bcmfd, &bcmstr, sizeof(bcmstr));	
	if(ret < 0)
		return ret;

//	printf("addr=%x,*value=%x\n",addr,*value);

	return 0;
}

/*
 * inpurt:
 * 1: linkup 
 * 0: linkdown
 *
 * return:
 * 1: success
 * other value: fail
 */

int hsl_eth0_set_enable(int enable)
{
	struct ifreq ifr;
        int error;

	if((enable > 1) || (enable < 0))
	{
		printf("Invalid input para = %x\n", enable);
		return 0;
	}

        strcpy(ifr.ifr_name, OUTBAND_STRING);

	ifr.ifr_flags = enable;
	error = ioctl(sockfd, SIOCSIFFLAGS, &ifr);

        if(error < 0)
	{
                printf("hsl_eth0_set_enable:  error=%x errno=%x\n",error,errno);		
		return error;
	}

	return 1;        
}

/*
 * inpurt:
 * 1 - 17 : every input can clear all counters
 *
 * return:
 * 1: success
 * other value: fail
 */
int hsl_eth0_clear_counter(int type)
{
        struct ifreq ifr;
        unsigned int value = 0;

        if((type > HslStatTypeCountMax) || (type <= 0))
        {
                printf("Invalid input para = %x\n", type);
                return 0;
        }
	value = 0x81;
	bcm53101_write(BCM53101_C, 0x2, 0, &value);
	usleep(10000);
	value = 0x80;
	bcm53101_write(BCM53101_C, 0x2, 0, &value);
	
        return 0;
}

int outband_counter_get()
{
	unsigned int value = 0;
	
	memset(&outband_counter, 0, sizeof(outband_counter));
	/*get tx good bytes*/
	bcm53101_read(BCM53101_C, 0x21, 0, &value);
	outband_counter.goodOctetsTx = value;
	/*get tx drop pkts*/
	bcm53101_read(BCM53101_C, 0x21, 0x8, &value);
	outband_counter.ErrPktsTx = value;
	/*get tx bc pkts*/
	bcm53101_read(BCM53101_C, 0x21, 0x10, &value);
	outband_counter.BcPktsTx = value;
	outband_counter.goodPktsTx += value;
	/*get tx mc pkts*/
	bcm53101_read(BCM53101_C, 0x21, 0x14, &value);
	outband_counter.McPktsTx = value;
	outband_counter.goodPktsTx += value;
	/*get tx uc pkts*/
	bcm53101_read(BCM53101_C, 0x21, 0x18, &value);
	outband_counter.UcPktsTx = value;
	outband_counter.goodPktsTx += value;

	/*get rx good bytes*/
	bcm53101_read(BCM53101_C, 0x21, 0x88, &value);
	outband_counter.goodOctetsRx= value;
//	printf("goodOctetsRx=%x\n",outband_counter.goodOctetsRx);
	/*get drop pkts*/
	bcm53101_read(BCM53101_C, 0x21, 0x90, &value);
	outband_counter.discardPktsRx += value;
	/*get drop pkts  rx discard*/
	bcm53101_read(BCM53101_C, 0x21, 0xc0, &value);
	outband_counter.discardPktsRx += value;
	/*get uc pkts*/
	bcm53101_read(BCM53101_C, 0x21, 0x94, &value);
	outband_counter.UcPktsRx= value;
	outband_counter.goodpktsRx += value;
	/*get mc pkts*/
	bcm53101_read(BCM53101_C, 0x21, 0x98, &value);
	outband_counter.McPktsRx= value;
	outband_counter.goodpktsRx += value;
	/*get bc pkts*/
	bcm53101_read(BCM53101_C, 0x21, 0x9c, &value);
	outband_counter.BcPktsRx= value;
	outband_counter.goodpktsRx += value;
	/*get fcs(crc) pkts*/
	bcm53101_read(BCM53101_C, 0x21, 0x84, &value);
	outband_counter.badCrcPktsRx =value;

	return 0;
}

/*
 * inpurt:
 * 1 - 17 : every input can clear all counters
 *
 * return:
 * 1: success
 * other value: fail
 */

int hsl_eth0_get_counter(int type, unsigned int *val)
{
        struct ifreq ifr;
        int error;
	int setval[2];

        if((type > HslStatTypeCountMax) || (type <= 0))
        {
                printf("Invalid input para = %x\n", type);
                return 0;
        }

        if(val == NULL)
        {
                printf("Invalid input para = NULL\n");
                return 0;
        }

	outband_counter_get();
       	*val = (unsigned int) *((unsigned int *)&outband_counter+(type-1));

        return 1;
}

/*
 * inpurt:
 * 
 *
 * return:
 * 1: success
 * other value: fail
 */

int hsl_eth0_get_mac(char *mac)
{
        struct ifreq ifr;
        int error;
	char mem[6];

        if(mac == NULL)
        {
                printf("Invalid input para = NULL\n");
                return 0;
        }


        strcpy(ifr.ifr_name, OUTBAND_STRING);
	/*
        memcpy(ifr.ifr_hwaddr.sa_data, mac, 6);
        */
        ifr.ifr_hwaddr.sa_family = 1;
	//ifr.ifr_ifru.ifru_mtu=3000;
        ifr.ifr_data = mem;
        error = ioctl(sockfd, SIOCGIFHWADDR, &ifr);


        if(error < 0)
        {
                printf("hsl_eth0_get_mac:  error=%x errno=%x\n",error,errno);
                return error;
        }
        
        memcpy(mac,ifr.ifr_hwaddr.sa_data,6);
        return 1;
}

/*
 * return value:
 * 10 or 100
 */
int hsl_eth0_get_speed( int* speed)
{
	int error = 0;
	int portspeed[3] = {0};
	
	error = eth0_get_port(&portspeed[0],&portspeed[1],&portspeed[2]);
	
//	printf("error = %d,hsl_eth0_get_speed = %d %d\n",error, portspeed[1],portspeed[2]);
	if(error == 0)
		*speed = portspeed[1];
	
	return(error);
		
}
/*
 * return value:
 * 0: half
 * 1: full
 */
int hsl_eth0_get_duplex (  int* duplex  )
{
	int error;
	int portspeed[3] = {0};
	
	error = eth0_get_port(&portspeed[0],&portspeed[1],&portspeed[2]);
	
	if(error == 0)
	{
		*duplex = portspeed[2];  /*0=half, 1=full */
	}
	return(error);	
} 

int hsl_eth0_get_autoneg(int* autoneg )
{
	unsigned int value = 0;
	
	bcm53101_read(BCM53101_C, 0x11, 0, &value);
	
	if((0x1000&value) != 0)
		*autoneg = 1; 
	else
		*autoneg = 0; 
	return 0;		
}

/*
 * inpurt:
 * 1-2 : 100M
 *
 * return:
 * 0: success
 * -1:fail
 */
int hsl_eth0_set_speed( int  speed)
{
	unsigned int value,setval;
	
	if((speed <= 0) || (speed > IFNET_SPEED_FE))
	{
		printf("Invalid input para = %x\n",speed);
		return -1;
	}

	hsl_eth0_set_autoneg(2);
	bcm53101_read(BCM53101_C, 0x11, 0, &value);
	value |=1<<13;
	bcm53101_write(BCM53101_C, 0x11, 0, &value);
	
	return 0;
}

/*
 * inpurt:
 * 1 : FULL
 * 2 : HALF
 *
 * return:
 * 0: success
 * -1:fail
 */
int hsl_eth0_set_duplex ( int duplex )
{
	unsigned int value,setval;

	if((duplex <= 0) || (duplex > IFNET_HALFDUPLEX))
	{
		printf("Invalid input para = %x\n",duplex);
		return -1;
	}

	if(duplex == IFNET_FULLDUPLEX)

	hsl_eth0_set_autoneg(2);
	bcm53101_read(BCM53101_C, 0x11, 0, &value);
	setval = (duplex == IFNET_FULLDUPLEX)?(value|(1<<8)):(value&(!(1<<8)));
	bcm53101_write(BCM53101_C, 0x11, 0, &setval);
	
	return 0;	

} 

/*
 * inpurt:
 * 1 : auto
 * 2 : force
 *
 * return:
 * 1: success
 * other value: fail
 */
int hsl_eth0_set_autoneg(int autoneg )
{
	unsigned int value,setval;

	bcm53101_read(BCM53101_C, 0x11, 0, &value);
	setval = (autoneg==1)?(value|1<<12):(value&(!(1<<12)));
	bcm53101_write(BCM53101_C, 0x11, 0, &setval);
	
	return 0;
	
}


int eth0_get_port(int *forcespeed, int *speed, int *duplex)
{
        int error;
	unsigned int value = 0, status = 0;
	
        if((forcespeed == NULL) || (speed == NULL) || (duplex == NULL))
        {
                printf("Invalid input para = NULL\n");
                return 0;
        }

	/* 1.get link status*/
	bcm53101_read(BCM53101_C, 0x11, 0x2, &value);
	bcm53101_read(BCM53101_C, 0x11, 0x2, &value);
	if ((value & 0x4) == 0)
	{
                printf("outband port is not link\n");
		return -1;
	}
	/* 2.get autonegotiation status
		auto->get auto speed and duplex
		forced->get auto speed and duplex*/
	bcm53101_read(BCM53101_C, 0x11, 0x32, &value);
	if ((value & 0x8000) != 0)
	{
		status = (value>>8)&0x7;
		
		if(0x5 == status)
		{
			*speed = IFNET_SPEED_FE;
			*duplex = IFNET_FULLDUPLEX;
		}
		else
		{
			*speed = IFNET_SPEED_INVALID;
			*duplex = IFNET_DUPLEX_INVALID;
		}
	}
	else
	{
		bcm53101_read(BCM53101_C, 0x11, 0x0, &value);
		if ((value&0x2000) != 0)
			*speed = IFNET_SPEED_FE;
		else
			*speed = IFNET_SPEED_INVALID;
		if ((value&0x0100) != 0)
			*duplex = IFNET_FULLDUPLEX;
		else
			*duplex = IFNET_DUPLEX_INVALID;
        }
	
//        printf("speed=%d duplex=%d\n", *speed, *duplex);
        return 0;	
}





#if 1
int main(void) /* for test */
{
        unsigned int val=0;
        char mac[6];
        
        hsl_outband_eth0_init();

        #if 1
        val = 1;
        hsl_eth0_set_enable(val);
        #endif
	#if 1
	int i;
	hsl_eth0_get_mac(mac);
	for(i=0; i<6;i++)
		printf("mac=%02x\n", (char)mac[i]);
	#endif

        
        #if 1
	hsl_eth0_get_counter(2,&val);
	printf("count val=%x\n",val);
	hsl_eth0_clear_counter(1);
	usleep(1000000);
	hsl_eth0_get_counter(2,&val);
	printf("count val=%x\n",val);
	#endif
	
	
	#if 1
	hsl_eth0_get_speed(&val);
	printf("speed = %d\n", val);
	
	hsl_eth0_get_duplex(&val);
	printf("duplex = %d\n", val);
	
	hsl_eth0_get_autoneg(&val);
	printf("auto = %x\n", val);
	#endif

        
                
}

#endif 

