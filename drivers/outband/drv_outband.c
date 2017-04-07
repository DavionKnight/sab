//kernel module: netlink-exam-kern.c  
#include <linux/slab.h>
#include <linux/configfs.h>  
#include <linux/module.h>  
#include <linux/netlink.h>  
#include <linux/sched.h>  
#include <net/sock.h>  
#include <linux/proc_fs.h>  
#include <net/net_namespace.h>
#include <linux/time.h>
#include <linux/irq.h>
#include <linux/workqueue.h>	/* We need tq_struct.	 */
#include "phy_api.h"

#ifndef NETLINK_2000
#define NETLINK_2000 18
#endif

struct sock *netlink_sock;

#ifndef GROUP_MASK 
#define GROUP_MASK 3
#endif

struct message_info {
        unsigned char port; /* 0: outband port*/
        unsigned char status; /* 1: linkup, 0:linkdown */
        unsigned char speed;  /* 1: 10M, 2: 100M, 3:1000M */
	unsigned char duplex; /* current duplex: 0=half, 1=full */
};

struct message_info link_sock_mes;
extern struct sock *netlink_sock;

/*
extern unsigned long  linkstatus_callback_funcs;
#define LINKSTATUS_FUNC_MAP(func) (linkstatus_callback_funcs=(unsigned long)func)
#define LINKSTATUS_FUNC_CALL(func_type) ((func_type)(linkstatus_callback_funcs))	
typedef int*( *linkstatus_callbak_func) (int,int,int,int);
*/

extern void  linkstatus_register_callback(  int *call_back );

  
static void recv_sock_handler(struct sk_buff * sk)  
{  
  printk("start to recv_handler()....\n");

  //wake_up(sk->sk->sk_sleep);  
}  


int linkstatus_change(int port,int speed,int status,int duplex)
{
    	struct sk_buff * skb0 = NULL;
    	struct nlmsghdr * nlhdr = NULL;
	int myspeed=0;
	//printk("port=%d speed=%d status=%d duplex=%d\n",port,speed,status,duplex);
	if(speed == 10)
		myspeed = 1;
	else if(speed == 100)
		myspeed = 2;
	else if(speed == 1000)
		myspeed = 3;

	skb0 = alloc_skb(64, GFP_KERNEL);
	if(skb0)
	{
		nlhdr = NLMSG_PUT(skb0, 0, 0, 0, 64-sizeof(*nlhdr));

               	link_sock_mes.port = 0;
		link_sock_mes.speed= myspeed;
		link_sock_mes.status=status;
		link_sock_mes.duplex=duplex;

               	memcpy(NLMSG_DATA(nlhdr), (unsigned char*)&link_sock_mes.port, 4);

               	nlhdr->nlmsg_len = NLMSG_LENGTH(4);
               	nlhdr->nlmsg_pid = 0;
               	nlhdr->nlmsg_flags= 0;

               	NETLINK_CB(skb0).pid = 0;
               	NETLINK_CB(skb0).dst_group = GROUP_MASK;


               	netlink_broadcast(netlink_sock, skb0, 0, GROUP_MASK, GFP_KERNEL);

	}
	else
	{
		printk("Error to malloc nlhdr...\n");
		return 0;
	}
	return 1;

        nlmsg_failure:
        if(skb0 != 0)
                        kfree_skb(skb0);
	return 0;
}

int genphy_read_link(unsigned char *link_status)
{
	unsigned short value[4];
	int ret = 0;

	ret = bcm53101_read(BCM53101_C, 0x11, MII_BMSR, value);	
	if(ret < 0)
		return ret;
	ret = bcm53101_read(BCM53101_C, 0x11, MII_BMSR, value);	
//printk("value=0x%x 0x%x 0x%x 0x%x, 0x%08x\n",value[3],value[2],value[1],value[0],
	if(ret < 0)
		return ret;
	if ((value[0] & 0x4) == 0)
		*link_status = 0;
	else
		*link_status = 1;
	return 0;
}

static struct timer_list link_scan_timer; 
unsigned char outband_link_status = 0;

void outband_phy_link_status_scan(void)
{
	unsigned char link_status = 0;
	genphy_read_link(&link_status);

	if(outband_link_status != link_status)
	{
		outband_link_status = link_status;
		linkstatus_change(0, 0, link_status, 0);	
//		printk("link status change:%d\n",link_status);
	}

        link_scan_timer.expires = jiffies+(HZ*1);                                        
        add_timer(&link_scan_timer);
}

int outband_init(void)
{
  	printk("start to outband_init()...ver1.00\n");
        init_timer(&link_scan_timer);                                                    

        link_scan_timer.function = outband_phy_link_status_scan;  
	printk("HZ=%d\n",HZ);                            
        link_scan_timer.expires = jiffies + (HZ * 1);                                         
        add_timer(&link_scan_timer); 
	
   	netlink_sock = netlink_kernel_create(&init_net, NETLINK_2000, 0, recv_sock_handler, NULL,THIS_MODULE);
    	if (!netlink_sock) {
                printk("Fail to create netlink_sock socket.....\n");
   	}
	return 0;
}

void outband_exit(void)  
{
	if(netlink_sock)
  		sock_release(netlink_sock->sk_socket);  
	del_timer(&link_scan_timer);
}  




