/*
*  COPYRIGHT NOTICE
*  Copyright (C) 2016 HuaHuan Electronics Corporation, Inc. All rights reserved
*
*  Author       	:fzs
*  File Name        	:/home/kevin/works/projects/H20RN-2000/drivers/gpio/gpiodrv.c
*  Create Date        	:2016/10/26 19:55
*  Last Modified      	:2016/10/26 19:55
*  Description    	:
*/

#include<stdio.h>
#include<stdlib.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/ipc.h>  
#include <sys/sem.h>  
#include <errno.h>

#define ARM5626X_GPIO_PINS	16

//#define GPIO_DIR		0x00
#define GPIO_DIR		0x08

//#define GPIO_ODR		0x04

#define GPIO_DAT		0x04
//#define GPIO_DAT		0x08

//#define GPIO_IER		0x0c
//#define GPIO_IMR		0x10
//#define GPIO_ICR		0x14

//#define GPIO_BASE		0xffe0f000
#define GPIO_BASE		0x1800a000
#define MAP_SIZE 		4096UL
#define MAP_MASK 		(MAP_SIZE - 1)

volatile unsigned char *gpioBase = 0;
int fd_mmap;
struct sembuf bufLock, bufUnlock;
int semid;
int initialized = 0;

void setbits32(volatile unsigned int *addr, unsigned int v)
{ 
	unsigned int val = *addr;
	val |= v;	
	*addr = val;
}
void clrbits32(volatile unsigned int *addr, unsigned int v)
{ 
	unsigned int val = *addr;
	val &= ~v;	
	*addr = val;
}

union semun
{
  int val;			/* value for SETVAL */
  struct semid_ds *buf;		/* buffer for IPC_STAT & IPC_SET */
  unsigned short int *array;	/* array for GETALL & SETALL */
  struct seminfo *__buf;	/* buffer for IPC_INFO */
  struct __old_semid_ds *__old_buf;
};

static inline unsigned int  mpc8xxx_gpio2mask(unsigned int gpio)
{
	return 1u << gpio;
}


static int gpiodrv_mmap_init()
{
	if((fd_mmap = open("/dev/mem", O_RDWR | O_SYNC)) == -1) 
	{
		return -1;
	}
	gpioBase = (unsigned char*)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_mmap, GPIO_BASE);
	return 0;
}

static void gpiodrv_mmap_exit(void)
{
	munmap((void *)gpioBase,MAP_SIZE);
	close(fd_mmap);
}

static int gpiodrv_semlock_init()  
{  
	union semun arg;

	semid = semget(0x0813, 1, IPC_CREAT|IPC_EXCL|0666);
	if(-1 == semid)
	{
		initialized = 1;
		semid = semget(0x0813, 1, IPC_CREAT|0666);
		if (semid == -1){  
			return -1;  
		}  
	}
	else
	{
		initialized = 0;
		arg.val = 1;
		semctl(semid, 0, SETVAL, arg);  
	}
	bufLock.sem_num = 0;  
	bufLock.sem_op = -1;  
	bufLock.sem_flg = SEM_UNDO;  

	bufUnlock.sem_num = 0;  
	bufUnlock.sem_op = 1;  
	bufUnlock.sem_flg = SEM_UNDO;  

	return 0;
}

static inline unsigned char gpio_is_valid(int number)
{
	/* only some non-negative numbers are valid */
	return ((unsigned)number) < ARM5626X_GPIO_PINS;
}

int gpio_output(unsigned gpio, unsigned char value)
{
	unsigned int data = 0;

	if(!gpio_is_valid(gpio))
	{
		printf("gpio %d is valid\n",gpio);
		return -1;
	}

	semop(semid, &bufLock, 1);

	if (value)
		setbits32((unsigned int *)(gpioBase + GPIO_DAT), mpc8xxx_gpio2mask(gpio));
	else
		clrbits32((unsigned int *)(gpioBase + GPIO_DAT), mpc8xxx_gpio2mask(gpio));

	semop(semid, &bufUnlock, 1);

	return 0;
}
int gpio_direction_output(unsigned gpio, unsigned char value)
{
	unsigned int data = 0;

	if(!gpio_is_valid(gpio))
	{
		printf("gpio %d is valid\n",gpio);
		return -1;
	}

	semop(semid, &bufLock, 1);

	/*set direction*/
	setbits32((unsigned int *)(gpioBase + GPIO_DIR), mpc8xxx_gpio2mask(gpio));

	if (value)
		setbits32((unsigned int *)(gpioBase + GPIO_DAT), mpc8xxx_gpio2mask(gpio));
	else
		clrbits32((unsigned int *)(gpioBase + GPIO_DAT), mpc8xxx_gpio2mask(gpio));

	semop(semid, &bufUnlock, 1);

	return 0;
}

int gpio_direction_input(unsigned gpio)
{
	if(!gpio_is_valid(gpio))
	{
		printf("gpio %d is valid\n",gpio);
		return -1;
	}

	semop(semid, &bufLock, 1);

	/*set direction*/
	clrbits32((unsigned int *)(gpioBase + GPIO_DIR), mpc8xxx_gpio2mask(gpio));

	semop(semid, &bufUnlock, 1);

	return 0;

}

unsigned char gpio_get_value(unsigned gpio)
{
	unsigned int val = *(gpioBase + GPIO_DAT);
	return (*(gpioBase + GPIO_DAT) & mpc8xxx_gpio2mask(gpio))? 1:0;
}

int gpiodrv_init()
{
	if(gpiodrv_semlock_init())
	{
		printf("spidrv semlock init err\n");
		return -1;
	}
	if(gpiodrv_mmap_init())
	{
		printf("spidrv mmap init err\n");
		return -1;
	}
	return 0;
}

void gpiodrv_exit()
{
	gpiodrv_mmap_exit();
	return ;
}

