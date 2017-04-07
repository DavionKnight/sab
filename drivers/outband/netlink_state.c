/*
*  COPYRIGHT NOTICE
*  Copyright (C) 2016 HuaHuan Electronics Corporation, Inc. All rights reserved
*
*  Author       	:fzs
*  File Name        	:/home/kevin/works/projects/H20RN-2000/drivers/outband/netlink_state.c
*  Create Date        	:2016/12/15 19:03
*  Last Modified      	:2016/12/15 19:03
*  Description    	:
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

struct message_info {
        unsigned char port; /* 0: outband port*/
        unsigned char status; /* 1: linkup, 0:linkdown */
        unsigned char speed;  /* 1: 10M, 2: 100M, 3:1000M */
        unsigned char duplex; /* current duplex: 0=half, 1=full */
};

//netlink application receiver: demo.c  
#include <stdio.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <linux/netlink.h>  

#define MAX_MSGSIZE 1024  

#define NETLINK_OUTBAND 23

#ifndef NETLINK_MASK
#define NETLINK_MASK 3
#endif
  
int netlink_group_mask(int group)
{
  return group ? 1<<(group-1) : 0;
}
  
int main(void)  
{  
        struct sockaddr_nl saddr, daddr;  
        struct nlmsghdr *nlhdr = NULL;  
        struct msghdr msg;  
        struct iovec iov;  
        int sd;  
        int ret = 1;  
        unsigned char rx[10];


        sd = socket(AF_NETLINK, SOCK_RAW,NETLINK_OUTBAND);  
        memset(&saddr, 0, sizeof(saddr));  
        memset(&daddr, 0, sizeof(daddr));  
  
        saddr.nl_family = AF_NETLINK;        
        saddr.nl_pid = getpid();  
        saddr.nl_groups = netlink_group_mask(NETLINK_MASK);  
        bind(sd, (struct sockaddr*)&saddr, sizeof(saddr));  
  
        nlhdr = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_MSGSIZE));  
          
        while (1) {  
                memset(nlhdr, 0, NLMSG_SPACE(MAX_MSGSIZE));  
  
                iov.iov_base = (void *)nlhdr;  
                iov.iov_len = NLMSG_SPACE(MAX_MSGSIZE);  
                msg.msg_name = (void *)&daddr;  
                msg.msg_namelen = sizeof(daddr);  
                msg.msg_iov = &iov;  
                msg.msg_iovlen = 1;  

//                gettimeofday(&tv[0],NULL);

                ret = recvmsg(sd, &msg, 0);  
                
                if (ret == 0) {  
                        printf("Exit.\n");  
                        exit(0);  
                }  
                else if (ret == -1) {  
                        perror("recvmsg:");  
                        exit(1);  
                }  
                memcpy(rx,NLMSG_DATA(nlhdr), 4);
               printf("RECV: port=%d status=%d speed=%d duplex%d ....\n", rx[0],rx[1],rx[2],rx[3]);
  
        }  
   
        close(sd);  
}  



