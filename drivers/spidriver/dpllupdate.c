
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#define ctrl_dpll "./dpll"
#define ctrl_fpga "./fpga"

void show_help(char *name)
{
	printf( "please input :\n  %s [-80..80]\n", name );
}

int main(int argc, char *argv[])
{
	float	ppm;
	char	cmd[100];
	unsigned short reg;
	unsigned char data[2];

	if (argc != 2)
	{
		show_help(argv[0]);
		return 1;
	}

	ppm	= atof(argv[1]);
	if (ppm < -80.0 || ppm > 80)
	{
		show_help(argv[0]);
		return 1;
	}

	reg  = (unsigned short)(ppm/0.002452866 + 32768);
	data[0] = (unsigned char)(reg >> 8);
	data[1] = (unsigned char)(reg);
	
	//sprintf(cmd, "%s write 0012 0000\n", ctrl_fpga);
	//system(cmd);
	sprintf(cmd, "%s write 0003 85\n", ctrl_dpll);
	system(cmd);
	//sprintf(cmd, "%s write 0012 0000\n", ctrl_fpga);
	//system(cmd);
	sprintf(cmd, "%s write 0004 %02x\n", ctrl_dpll, data[0] );
	system(cmd);
	//sprintf(cmd, "%s write 0012 0000\n", ctrl_fpga);
	//system(cmd);
	sprintf(cmd, "%s write 0005 %02x\n", ctrl_dpll, data[1] );
	system(cmd);
	//sprintf(cmd, "%s write 0012 0000\n", ctrl_fpga);
	//system(cmd);
	sprintf(cmd, "%s write 0003 00\n", ctrl_dpll);
	system(cmd);
	
	
	

    return 0;
}

