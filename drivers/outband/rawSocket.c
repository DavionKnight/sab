
/*
 *  COPYRIGHT NOTICE
 *  Copyright (C) 2016 HuaHuan Electronics Corporation, Inc. All rights reserved
 *
 *  Author       	:tianzhy
 *  File Name        	:
 *  Create Date        	:2016/11/02
 *  Last Modified      	:
 *  Description    	:
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/if_ether.h>
#include <linux/in.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <pthread.h>

int sock;
struct ifreq ethreq;
struct sockaddr_ll sock_link;

#define DEBUG_OUTBAND


#ifdef DEBUG_OUTBAND

typedef unsigned int i32_t;
typedef unsigned short i16_t;
typedef unsigned char ui8_t;
 
typedef struct hsl_pkt_recv_info_s
{
    i32_t  unit;                
    i32_t  slot;
    i32_t  port;                /** pannel port id */          
    ui8_t *data;                    /** pointer to pkt data */
    i16_t  len;                                 /** pkt length */
         i16_t  vlan;
         i16_t  cvlan;
         i16_t  priority;
}hsl_pkt_recv_info_t;

typedef struct hsl_pkt_send_info_s
{
         i32_t  gport;         
         i32_t  trunk_id;
         ui8_t *data;                        /** pointer to pkt data */
         i16_t  len;                                     /** pkt length */
         i32_t flags;           /* 发送的报文标记，如是否由硬件添加CRC信息 */
}hsl_pkt_send_info_t;

int count=0;
int hal_pkt_recv ( hsl_pkt_recv_info_t *pktinfo )
{
	//return 1;
	
	if(pktinfo->data[1] == 0x1d)
	{
		count ++;
		printf("%d\n", count);	
	}
//	return 1;
	printf("len = %d\n", pktinfo->len);
	printf("Dest MAC addr:%02x:%02x:%02x:%02x:%02x:%02x\n",pktinfo->data[0],pktinfo->data[1],pktinfo->data[2],pktinfo->data[3],pktinfo->data[4],pktinfo->data[5]);
	printf("srcc MAC addr:%02x:%02x:%02x:%02x:%02x:%02x\n\n\n",pktinfo->data[6],pktinfo->data[7],pktinfo->data[8],pktinfo->data[9],pktinfo->data[10],pktinfo->data[11]);
	
	return 1;
	
}
#else

extern int hal_pkt_recv ( hsl_pkt_recv_info_t *pktinfo );

#endif

i32_t hal_eth0_pkt_send(hsl_pkt_send_info_t* pktinfo)
{
	int ret;
	
	if(pktinfo == NULL ) 
	{
		printf("hal_eth0_pkt_send error\n");
		return 0;
	}
	
	if(pktinfo->data == NULL)
	{
		printf("hal_eth0_pkt_send data error\n");
		return 0;
	}	
	
	if((ret = sendto(sock, pktinfo->data, pktinfo->len, 0, (struct sockaddr *)&sock_link, sizeof(sock_link)))<0)
	{
		printf("failed to send to RAW ret = %d\n",ret);
		return 0;
	}
	return 1;
}


int socket_rx_thread(void *arg)
{

	hsl_pkt_recv_info_t recv_info;
	int len;
	char buffer[2048];
	
	struct sched_param param;
    	param.sched_priority = 90;
    	if (sched_setscheduler(0, SCHED_RR, &param)) {       
        	perror("\n  priority set: ");
    	}
  
	while(1)
	{
		len = recvfrom(sock,buffer,2048,0,NULL,NULL);
		if( len != 0)
		{
			recv_info.unit = 1;
			recv_info.slot = 0;
			recv_info.port = 1;
			recv_info.len = len;
			recv_info.data = (ui8_t*)buffer;
			hal_pkt_recv(&recv_info);
		}
	}
}

int outbandSocketInit(void)
{
	
        pthread_t tid;
        
        /* create packet receive process thread*/
        pthread_create(&tid, NULL, (void *)socket_rx_thread, NULL);


	memset(&sock_link,0,sizeof(sock_link));
	sock_link.sll_family = AF_PACKET;
	sock_link.sll_protocol = htons(ETH_P_ALL);
	
	if (0>(sock=socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)))) 
	{
		perror("socket connect error\n");
		return 0;
	}
     
        strcpy(ethreq.ifr_name,"eth0");

	if ( ioctl( sock, SIOCGIFINDEX, &ethreq ) < 0 )
	{
		printf( "failed to get IF hw address!" );
		return 0;
	}
	sock_link.sll_ifindex = ethreq.ifr_ifindex;	
	
	return 1;
}

#ifdef DEBUG_OUTBAND
int main(int argc, char **argv) 

{
	
	hsl_pkt_send_info_t sent_info;
	char buf[2048];
	int i;

	
	outbandSocketInit();

	while (1) {
		
		for(i=0; i<100; i++)
			buf[i] = i;
			
		sent_info.data = (ui8_t*)buf;
		sent_info.len = 100;	
		//hal_eth0_pkt_send(&sent_info);
		sleep(10);
	}
}

#endif
