/*
*  COPYRIGHT NOTICE
*  Copyright (C) 2016 HuaHuan Electronics Corporation, Inc. All rights reserved
*
*  Author       	:Kevin_fzs
*  File Name        	:/home/kevin/works/projects/IPRAN/drivers/inittab/HHlogin.c
*  Create Date        	:2016/08/11 17:37
*  Last Modified      	:2016/08/11 17:37
*  Description    	:
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/reboot.h>
int main()
{
	unsigned char str[100]={0};
	unsigned char str1[50]={0};
	unsigned char str2[50]={0};
	unsigned char str3[50]={0};
	unsigned char str4[50]={0};
	unsigned char str_sys[100]={0};
	int ret = 0;
	while(1)
	{
		printf("     	Use command to upgrade system:\n\n");	
		printf("		get [file] [local ip] [server ip]\n\n");	
		printf("example:get product_v1.0 192.168.1.1 192.168.1.254\n");	
		gets(str);
		sscanf(str,"%s%s%s%s",str1,str2,str3,str4);
//		printf("str:%s\n",str1);
//		printf("str:%s\n",str2);
//		printf("str:%s\n",str3);
//		printf("str:%s\n",str4);
		if(strncmp("get", str1, 3))
			continue;
		sprintf(str_sys, "ifconfig eth0 %s",str3);
//		printf("str_sys:%s\n",str_sys);
		ret = system(str_sys);
		if(ret)
		{
			printf("parameter error:%s\n",str_sys);
			continue;
		}
		sprintf(str_sys,"tftp -g -l /data/ver/%s -r %s %s",str2,str2,str4);
//		printf("str_sys:%s\n",str_sys);
		ret = system(str_sys);
		if(ret)
		{
			printf("parameter error:%s\n",str_sys);
			continue;
		}

		/*untar file*/
		system("mkdir -p /data/ver/bin");
		sprintf(str_sys,"tar -xvf /data/ver/%s -C /data/ver",str2);
//		printf("str_sys:%s\n",str_sys);
		ret = system(str_sys);
		if(ret)
		{
			printf("parameter error:%s\n",str_sys);
			continue;
		}
		/*untar cucc packet*/
		sprintf(str_sys,"%s","tar -xvf /data/ver/*sys_v001r001 -C /data/ver");
		ret = system(str_sys);
		if(ret)
		{
			printf("parameter error:%s\n",str_sys);
			continue;
		}

		sprintf(str_sys,"%s","tar -xvf /data/ver/*app* -C /data/ver/bin");
//		printf("str_sys:%s\n",str_sys);
		ret = system(str_sys);
		if(ret)
		{
			printf("parameter error:%s\n",str_sys);
			continue;
		}
		/*mv auto1 autorun.sh*/
		if(access("/home/bin/autorun1.sh", 0))
		{
			printf("/home/bin/autorun1.sh not found\n");
			continue;
		}
			
		ret = system("cp /home/bin/autorun1.sh /home/bin/autorun.sh");

		printf("system upgrade successfully, system reboot..\n");
		sleep(2);
		sync();
		system("reboot -nf");
		//reboot(RB_AUTOBOOT);

	
	}

	return 0;
}



