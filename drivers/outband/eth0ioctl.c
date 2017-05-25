
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
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <errno.h>
#include <linux/sockios.h>

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

#define GET_PORT_LINKSTATE 0x10000
#define CLEAR_COUNTER_FLAG 	0x0100
#define GET_COUNTER_FLAG	0x1000
#define GET_SPEED_FLAG		0x80

int outband_duplex = ET_1000FULL;

static int sockfd;

int hsl_outband_eth0_init(void)
{
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(sockfd < 0)
        {
                printf("can't open socket eth0......\n");
                return 0;
        }
        return 1;
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
        int error;
	int setval[2];

        if((type > HslStatTypeCountMax) || (type <= 0))
        {
                printf("Invalid input para = %x\n", type);
                return 0;
        }

        strcpy(ifr.ifr_name, OUTBAND_STRING);

        setval[0] = CLEAR_COUNTER_FLAG | type;
        ifr.ifr_data = (void*)setval;
        error = ioctl(sockfd, SIOCWANDEV, &ifr);

        if(error < 0)
        {
                printf("hsl_eth0_clear_counter:  error=%x errno=%x\n",error,errno);
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

        strcpy(ifr.ifr_name, OUTBAND_STRING);

        setval[0] = GET_COUNTER_FLAG | type;
        ifr.ifr_data = (void*)setval;
        error = ioctl(sockfd, SIOCWANDEV, &ifr);

        if(error < 0)
        {
                printf("hsl_eth0_get_counter:  error=%x errno=%x\n",error,errno);
                return error;
        }
        
        *val = *(int*)(ifr.ifr_data);
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
	int error;
	int portspeed[3];
	
	error = eth0_get_port(&portspeed[0],&portspeed[1],&portspeed[2]);
	
	if(error == 1)
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
	int portspeed[3];
	
	error = eth0_get_port(&portspeed[0],&portspeed[1],&portspeed[2]);
	
	if(error == 1)
	{
		*duplex = portspeed[2];  /*0=half, 1=full */
	}
	return(error);	
} 

/*
 * return value:
 * 0x7f: auto
 * other: force
 */
int hsl_eth0_get_autoneg(int* autoneg )
{
	int error;
	int portspeed[3];
	
	error = eth0_get_port(&portspeed[0],&portspeed[1],&portspeed[2]);
	
	if(error == 1)
	{
		*autoneg = portspeed[0];  /* 0x7f is auto; */
	}
	return(error);		
}

/*
enum IFNET_SPEED
{
    IFNET_SPEED_INVALID = 0,
    IFNET_SPEED_GE,
    IFNET_SPEED_FE,
};
*/
/*
 * inpurt:
 * 1 -2 : 100M
 *
 * return:
 * 1: success
 * other value: fail
 */
int hsl_eth0_set_speed( int  speed)
{
	int error;
	
	if((speed <= 0) || (speed > IFNET_SPEED_FE))
		printf("Invalid input para = %x\n",speed);


	error = eth0_set_port(outband_duplex); /* outband only support 10 or 100M */
	
	return(error);
}

/*
 * inpurt:
 * 1 : FULL
 * 2 : HALF
 *
 * return:
 * 1: success
 * other value: fail
 */
int hsl_eth0_set_duplex ( int duplex )
{
	int error;

	if((duplex <= 0) || (duplex > IFNET_HALFDUPLEX))
		printf("Invalid input para = %x\n",duplex);

	if(duplex == IFNET_FULLDUPLEX)
		outband_duplex = ET_100FULL;
	else
		outband_duplex =  ET_100HALF;

	error = eth0_set_port(outband_duplex); /* outband only support 10 or 100M */
	
	return(error);	

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
	int error;

	if((autoneg <= 0) || (autoneg > IFNET_SPEED_MODE_FORCE))
		printf("Invalid input para = %x\n",autoneg);

	if(autoneg == IFNET_SPEED_MODE_AUTO)
	{
		error = eth0_set_port(ET_AUTO); /* outband only support 10 or 100M */
	}
	else
		error = eth0_set_port(outband_duplex); /* outband only support 10 or 100M */
	
	return(error);
	
}

int eth0_set_port(int type)
{
        struct ifreq ifr;
        int error;
	int setval[2];
	

	type = type & 0xff;
        if((type < 0) || (type > ET_AUTO))
        {
                printf("Invalid input para = %x\n",type);
                return 0;
	}
        strcpy(ifr.ifr_name, OUTBAND_STRING);

        setval[0] = type;
        ifr.ifr_data = (void*)setval;
        error = ioctl(sockfd, SIOCWANDEV, &ifr);

        if(error < 0)
        {
                printf("eth0_get_port:  error=%x errno=%x\n",error,errno);
                return error;
        }
        return 1;		
}


int eth0_get_port(int *forcespeed, int *speed, int *duplex)
{
        struct ifreq ifr;    
        int error;
	int setval[3];
	
        if((forcespeed == NULL) || (speed == NULL) || (duplex == NULL))
        {
                printf("Invalid input para = NULL\n");
                return 0;
        }


        strcpy(ifr.ifr_name, OUTBAND_STRING);

        setval[0] = GET_SPEED_FLAG;
        ifr.ifr_data = (void*)setval;
        error = ioctl(sockfd, SIOCWANDEV, &ifr);

        if(error < 0)
        {
                printf("eth0_get_port:  error=%x errno=%x\n",error,errno);
                return error;
        }
        
        *forcespeed  = setval[0];
        *speed = setval[1];
        *duplex = setval[2];
        printf(" forcespeed = %x speed=%d duplex=%d\n", setval[0], setval[1],setval[2]);
        return 1;	
}
/*
 * return:
 * 1: success
 * other value: fail
 */
int hsl_eth0_get_linkstate( unsigned int *val)
{
        struct ifreq ifr;
        int sockfd;                     /* socket fd we use to manipulate stuff with */
        int error;
	int setval[2];

        if(val == NULL)
        {
                printf("Invalid input para = NULL\n");
                return 0;
        }
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(sockfd < 0)
        {
                printf("can't open socket eth0......\n");
                return 0;
        }

        strcpy(ifr.ifr_name, OUTBAND_STRING);

        setval[0] = GET_PORT_LINKSTATE;
        ifr.ifr_data = (void*)setval;
        error = ioctl(sockfd, SIOCWANDEV, &ifr);

        if(error < 0)
        {
                printf("hsl_eth0_get_counter:  error=%x errno=%x\n",error,errno);
                close(sockfd);
                return error;
        }
        
        *val = *(int*)(ifr.ifr_data);

        close(sockfd);
        return 1;
}




#if 1
int main(void) /* for test */
{
        unsigned int val=0;
        char mac[6];
        
         hsl_outband_eth0_init();
        #if 0
        val = 0;
        hsl_eth0_set_enable(val);
        #endif
        
        #if 1
	hsl_eth0_get_counter(1,&val);
	printf("rx good pkt=%d\n",val);
	hsl_eth0_get_counter(10,&val);
	printf("tx good pkt=%d\n",val);
	#endif
	
	#if 1
	int i;
	hsl_eth0_get_mac(mac);
	for(i=0; i<6;i++)
		printf("mac=%x-\n", (char)mac[i]);
	#endif
	
	#if 1
	hsl_eth0_get_speed(&val);
	printf("speed = %d\n", val);
	
	hsl_eth0_get_duplex(&val);
	printf("duplex = %d\n", val);
	
	hsl_eth0_get_autoneg(&val);
	printf("auto = %x\n", val);

	        hsl_eth0_get_linkstate(&val);
        printf("linkstate = %d\n", val);
#if 0
	hsl_eth0_set_speed(ET_100HALF);

	hsl_eth0_get_speed(&val);
	printf("22speed = %d\n", val);
#endif

#if 0
	hsl_eth0_set_autoneg(IFNET_SPEED_MODE_AUTO);
	hsl_eth0_get_speed(&val);
	printf("33speed = %d\n", val);
#endif
	#endif

        
                
}

#endif 

