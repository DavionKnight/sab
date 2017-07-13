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



int main()
{
	unsigned int i = 0, j = 0;
	unsigned char *p=NULL;

	for(i=0;i<10;i++)
	{
		p = malloc(0xa00000);
		printf("malloc %s\n",p==NULL?"failed":"succ");
	sleep(1);
		for(j=0;j<0xa00000;j++)
			*(p+j) = 0x55;
		
	}
	system("free > aa.txt");

	return 0;
}



