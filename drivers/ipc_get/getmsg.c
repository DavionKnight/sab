/*
*  COPYRIGHT NOTICE
*  Copyright (C) 2016 HuaHuan Electronics Corporation, Inc. All rights reserved
*
*  Author       	:fzs
*  File Name        	:/home/kevin/works/projects/XLDK-3.6.2/XLDK/drivers/getmsg.c
*  Create Date        	:2017/07/08 22:55
*  Last Modified      	:2017/07/08 22:55
*  Description    	:
*/

#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <linux/sysctl.h>


typedef unsigned int __be32;
typedef unsigned short __be16;

typedef unsigned int u_int;

typedef unsigned int    u_int32_t;
typedef unsigned short  u_int16_t;
typedef unsigned char   u_int8_t;

typedef long long int64_t;
typedef unsigned long long uint64_t;
typedef unsigned long long u_int64_t;

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef unsigned char uchar;

typedef unsigned short u_int16;
typedef unsigned int  u_int32;
typedef unsigned char u_char;

/* IPC 类型 */
enum IPC_TYPE
{
   IPC_TYPE_INVALID = 0,
   IPC_TYPE_LSP,    /* mpls lsp */
   IPC_TYPE_ILM,    /* mpls ilm */
   IPC_TYPE_NHLFE,  /* mpls nhlfe */
   IPC_TYPE_PW,     /* mpls pw */
   IPC_TYPE_VSI,    /* mpls vsi */
   IPC_TYPE_MPLSOAM,/* mpls oam*/
   IPC_TYPE_ROUTE,  /* ip 路由 */
   IPC_TYPE_FIB,    /* ip fib */
   IPC_TYPE_NHP,    /* ip nhp */
   IPC_TYPE_L3IF,   /* ip l3if */
   IPC_TYPE_ARP,    /* arp 全局信息*/
   IPC_TYPE_ARPIF,  /* arp 接口信息 */
   IPC_TYPE_NDP,    /* ndp 全局信息*/
   IPC_TYPE_NDPIF,  /* ndp 接口信息*/
   IPC_TYPE_IFM,    /* 接口管理 */
   IPC_TYPE_FTM,    /* 软转发 */
   IPC_TYPE_VLAN,   /* vlan */
   IPC_TYPE_L2IF,   /* l2if */
   IPC_TYPE_MPLSIF, /* mpls if */
   IPC_TYPE_DEVM,   /* 设备管理*/
   IPC_TYPE_CES,    /* ces */
   IPC_TYPE_ACL,    /* qos acl */
   IPC_TYPE_CAR,    /* qos car */
   IPC_TYPE_QOS,    /* QOS 全局信息 */
   IPC_TYPE_HQOS,
   IPC_TYPE_QOSIF,  /* QOS 接口配置 */
   IPC_TYPE_CFM,    /* CFM 消息 */
   IPC_TYPE_PROTO,  /* 协议注册*/
   IPC_TYPE_PACKET, /* 转发报文 */
   IPC_TYPE_VTY,    /* VTY 消息 */
   IPC_TYPE_SYSLOG, /* syslog 消息 */
   IPC_TYPE_AAA,    /* AAA 消息 */
   IPC_TYPE_BFD,    /* BFD 消息 */
   IPC_TYPE_TRUNK,  /* trunk */
   IPC_TYPE_MAC,    /* mac 地址消息 */
   IPC_TYPE_SYNCE,  /* synce 消息 */
   IPC_TYPE_TUNNEL, /* tunnel 消息 */
   IPC_TYPE_ALARM,  /* 告警消息 */
   IPC_TYPE_FILE,	/* 文件管理消息 */
   IPC_TYPE_SNMP,   /* SNMP 消息 */
   IPC_TYPE_SLA,	/* SLA 消息 */
   IPC_TYPE_OSPF,	/* OSPF 消息 */
   IPC_TYPE_RIP,	/* RIP 消息 */
   IPC_TYPE_ISIS,	/* ISIS 消息 */
   IPC_TYPE_OFP,    /* openflow 消息 */
   IPC_TYPE_ELPS,   /* ELPS 消息 */
   IPC_TYPE_BGP,    /* BGP消息 */
   IPC_TYPE_MAX = 255
};

/* ipc 消息头 */
struct ipc_msghdr
{
	int        module_id;  /* module id of receiver */
	uint16_t   sender_id;  /* module id of sender */
	uint16_t   data_len;   /* IPC data length */
	enum IPC_TYPE  msg_type;	 /* IPC msg type */
	uint8_t    msg_subtype;      /* subtype of msg, app can customize if needed */
	uint8_t    opcode;     /* operation code of data */
	uint8_t    data_num;   /* number of data */
	uint8_t    priority;   /* 消息的优先级 */
	uint8_t    unit;       /* unit 号，用于设备间的消息, 0 表示框内 */
	uint8_t    slot;       /* slot 号，用于板间的消息, 0 表示板内 */
	uint16_t   sequence;   /* 消息的序号 */
	uint32_t   msg_index;  /* maybe ifindex、lsp_index、pw_index and so on */
};


#define IPC_HEADER_LEN    24      /* IPC 消息头长度 */
#define IPC_QUEUE_LEN     1000    /* IPC 队列长度 */
#define IPC_MSG_LEN       2000    /* 用于控制消息的长度 */
#define IPC_MSG_LEN1      9960    /* 用于 9600 packet 的长度 */

struct ipc_pkt
{
    struct ipc_msghdr msghdr;
	uint8_t msg_data[IPC_MSG_LEN1];
};

int ipc_recv_pkt(int id, struct ipc_pkt *pmsg, int module_id)
{
	int ret = -1;

	if(id < 0)
	{
		printf("ipc_recv error, ipc id = 0!\n");
		return -1;
	}

	return msgrcv(id, (void *)pmsg, IPC_MSG_LEN1+IPC_HEADER_LEN-4, module_id, IPC_NOWAIT);
}

/* 控制消息 ipc 消息体 */
struct ipc_mesg
{
    struct ipc_msghdr msghdr;
	uint8_t msg_data[2000];
};

int ipc_connect(int key)
{
	int id;

	id = msgget(key, 0);
	if (id < 0)
	{
		printf("Connect message queue failed, key:%d,id:%d\n", key, id);
		return -1;
	}

	return id;
}
int main(int argc, char *argv[])
{
	unsigned int queue = 0;
	struct ipc_pkt mesg;
	int id=0;

	const char *mod[] = {
		"MODULE_ID_INVALID",
		"MODULE_ID_IFM",
		"MODULE_ID_DEVM",
		"MODULE_ID_VTY",
		"MODULE_ID_MPLS",
		"MODULE_ID_FTM",
		"MODULE_ID_HAL",
		"MODULE_ID_HSL",
		"MODULE_ID_QOS",
		"MODULE_ID_OSPF",
		"MODULE_ID_ISIS",
		"MODULE_ID_RIP",
		"MODULE_ID_BGP",
		"MODULE_ID_ROUTE",
		"MODULE_ID_ARP",
		"MODULE_ID_LDP",
		"MODULE_ID_RSVPTE",
		"MODULE_ID_VRRP",
		"MODULE_ID_DHCP",
		"MODULE_ID_LACP",
		"MODULE_ID_L2",
		"MODULE_ID_SYSLOG",
		"MODULE_ID_FILE",
		"MODULE_ID_PING",
		"MODULE_ID_CES",
		"MODULE_ID_AAA",
		"MODULE_ID_SYSTEM",
		"MODULE_ID_BFD"};
	const char *ipctype[] = {
		"IPC_TYPE_INVALID",
		"IPC_TYPE_LSP",    /* mpls lsp */
		"IPC_TYPE_ILM",    /* mpls ilm */
		"IPC_TYPE_NHLFE",  /* mpls nhlfe */
		"IPC_TYPE_PW",     /* mpls pw */
		"IPC_TYPE_VSI",    /* mpls vsi */
		"IPC_TYPE_MPLSOAM",/* mpls oam*/
		"IPC_TYPE_ROUTE",  /* ip 路由 */
		"IPC_TYPE_FIB",    /* ip fib */
		"IPC_TYPE_NHP",    /* ip nhp */
		"IPC_TYPE_L3IF",   /* ip l3if */
		"IPC_TYPE_ARP",    /* arp 全局信息*/
		"IPC_TYPE_ARPIF",  /* arp 接口信息 */
		"IPC_TYPE_IFM",    /* 接口管理 */
		"IPC_TYPE_FTM",    /* 软转发 */
		"IPC_TYPE_VLAN",   /* vlan */
		"IPC_TYPE_L2IF",   /* l2if */
		"IPC_TYPE_MPLSIF", /* mpls if */
		"IPC_TYPE_DEVM",   /* 设备管理*/
		"IPC_TYPE_CES",    /* ces */
		"IPC_TYPE_ACL",    /* qos acl */
		"IPC_TYPE_CAR",
		"IPC_TYPE_QOS",    /* QOS 全局信息 */
		"IPC_TYPE_HQOS"
		"IPC_TYPE_QOSIF",  /* QOS 接口配置 */
		"IPC_TYPE_CFM",    /* CFM 消息 */
		"IPC_TYPE_PROTO",  /* 协议注册*/
		"IPC_TYPE_PACKET", /* 转发报文 */
		"IPC_TYPE_VTY",    /* VTY 消息 */
		"IPC_TYPE_SYSLOG", /* syslog 消息 */
		"IPC_TYPE_AAA",    /* AAA 消息 */
		"IPC_TYPE_BFD",    /* BFD 消息 */
		"IPC_TYPE_TRUNK",  /* trunk */
		"IPC_TYPE_MAC",    /* mac 地址消息 */
		"IPC_TYPE_SYNCE",  /* syce 消息 */
		"IPC_TYPE_TUNNEL", /* tunnel 消息 */
		"IPC_TYPE_ALARM",
		"IPC_TYPE_FILE",
		"IPC_TYPE_SNMP",
		"IPC_TYPE_SLA",
		"IPC_TYPE_MAX"};

	if(argc<2)
		return 0;

	queue = (unsigned int)atoi(argv[1]);
	printf("queue id=%d\n",queue);
	id = ipc_connect(queue);

	if (id < 0)
	{
		printf( "key %d connect failed!\n", queue);
		return 0;
	}

	printf("key = %d, msqid = 0x%x\n", queue, id);

	/* 读一次队列内容 */
	memset(&mesg, 0, sizeof(struct ipc_pkt));
	if (-1 == ipc_recv_pkt(id, &mesg, 0))
	{
		printf ("Read fail! \n");
		return 0;
	}
	if(mesg.msghdr.msg_type < sizeof(ipctype)/sizeof(char *))
	{
		printf( "IPC_TYPE = %s\n", ipctype[mesg.msghdr.msg_type]);
	}
	else
	{
		printf( "IPC_TYPE = %d\n", mesg.msghdr.msg_type);
	}
	if(mesg.msghdr.sender_id < sizeof(mod)/sizeof(char *))
	{
		printf( "sender_id = %s\n", mod[mesg.msghdr.sender_id]);
	}
	else
	{
		printf( "sender_id = %d\n", mesg.msghdr.sender_id);
	}

	if((unsigned)mesg.msghdr.module_id < sizeof(mod)/sizeof(char *))
	{
		printf( "module_id = %s\n", mod[mesg.msghdr.module_id]);
	}
	else
	{
		printf( "module_id = %d\n", mesg.msghdr.module_id);
	}

	printf( "subtype = %d\n", mesg.msghdr.msg_subtype);
	printf( "opcode = %d\n", mesg.msghdr.opcode);
	printf( "data_len = %d\n", mesg.msghdr.data_len);
	
	int i=0;
	unsigned char *p=(unsigned char *)&mesg;
	for(i;i<sizeof(mesg);i++)
	{
		if(i%15==0)
			printf("\n");
		printf(" 0x%x",p[i]);
	}
			printf("\n");

	return 0;
}




