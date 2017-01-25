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

#define HOME_BIN_AUTORUN	"/home/bin/autorun.sh"
#define HOME_BIN_VTYSH		"/home/bin/vtysh"
#define BIN_LOGIN		"/bin/login"
#define BIN_SH			"/bin/sh"

#define START_LINUXSH	0	
#define START_VTYSH	1	

int execute_and_wait(int opt)
{
	pid_t cpid;
	int status;

	if(START_LINUXSH == opt)
	{
		char *argv[]={BIN_LOGIN, NULL};
		int ret =execv(BIN_LOGIN, argv);

		_exit(1);

	}
	if(START_VTYSH == opt)
	{
		cpid = fork();

		if(cpid == 0) //child 
		{
			char *argv[]={HOME_BIN_VTYSH, NULL};
			int ret =execv(HOME_BIN_VTYSH, argv);
	
			_exit(1);
		}
		else //father 
		{
			/*status 低8bits是信号，高8bits记录exit的返回值*/
			/*当正常返回的时候，waitpid返回收集到的子进程的进程ID*/
			int ret = waitpid(cpid,&status,0);
			printf("\nvtysh exit,return code:[status=0x%x][pid=%d]\n", status, ret);
			usleep(10000);
			exit(0);
		}

	}
}

static void
vtysh_signal_set (int signo, void (*func)(int))
{
  struct sigaction sig;
  struct sigaction osig;

  sig.sa_handler = func;
  sigemptyset (&sig.sa_mask);
  sig.sa_flags = 0;
#ifdef SA_RESTART
  sig.sa_flags |= SA_RESTART;
#endif /* SA_RESTART */

  sigaction (signo, &sig, &osig);
}

void vtysh_signal_init ()
{
  vtysh_signal_set (SIGINT, SIG_IGN);
  vtysh_signal_set (SIGTERM, SIG_IGN);
  vtysh_signal_set (SIGQUIT, SIG_IGN);
}

int main()
{
	char *argv[]={NULL};

	/*we must ignor SIGINT*/
	vtysh_signal_init ();
	if((!access(HOME_BIN_AUTORUN, 0))&&(!access(HOME_BIN_VTYSH, 0)))
	{
		printf("App is existent, startup...\n");
//		signal(SIGINT, SIG_DFL);
		execute_and_wait(START_VTYSH);
	}
	else
		execute_and_wait(START_LINUXSH);

	return 0;
}



