/*
*  COPYRIGHT NOTICE
*  Copyright (C) 2016 HuaHuan Electronics Corporation, Inc. All rights reserved
*
*  Author               :yuhongyue
*  File Name            :i2cdrv.c
*  Create Date          :2016/12/5
*  Last Modified        :2016/12/5
*  Description      :
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/sem.h>

#include <sys/mman.h>




#include "i2c.h"
#include "i2cdrv.h"
#include "eepdrv.h"

//#define NULL (void *)0


int i2cdrv_semid;
//unsigned int *mapbase = NULL;
unsigned int *mapbase = NULL;
struct sembuf i2cdrv_sembufLock, i2cdrv_sembufUnlock;   
static int i2cdrv_initialized = 0;

//#define H20RN2000


//#ifdef H20RN2000
#ifdef HT201

#if 0
struct fsl_i2c *i2c_dev[2] = {
	(struct fsl_i2c *) (CONFIG_SYS_IMMR + CONFIG_SYS_I2C_OFFSET),
#ifdef CONFIG_SYS_I2C2_OFFSET
	(struct fsl_i2c *) (CONFIG_SYS_IMMR + CONFIG_SYS_I2C2_OFFSET)
#endif
};
#endif

union semun
{
    int val; //信号量初始值
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfo *__buf;
};

union semun arg;

int fd_mmap;

int i2cdrv_mmap_init ( void *ptr )
{
#if 0
    int fd;
    unsigned int *virt_addr;
    /* mmap SERIAL_IO_SEL register */
    if ( ( fd_mmap = open ( "/dev/mem", O_RDWR | O_SYNC ) ) == -1 )
    {
        return -1;
    }
    mapbase = mmap ( NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_mmap, I2C_REGISTER_BASE );
	printf("------mapbase=%x------\n",mapbase);
#else
    if ( NULL == ptr )
    {
        printf ( "segmentfault in i2cdrv_mmap_init\n" );
        return -1;
    }
    mapbase = ptr;
	printf("##### mapbase = 0x%x\n", mapbase);
#endif
    return 0;
}

int eeprom_init(void)
{
	i2c_init(400000,0x51);

	return 0;
}

int i2cdrv_setup_init(void)
{
	if (eeprom_init() < 0) 
	{
		return -1;//ENXIO;
	}

	return 0;
}

void i2cdrv_mmap_exit ( void )
{
    munmap ( mapbase, MAP_SIZE );
    close ( fd_mmap );
}

#endif

int i2cdrv_semlock_init ( void )
{
    int ret = 0;

    i2cdrv_semid = semget ( 0x0812, 1, IPC_CREAT | IPC_EXCL | 0666 );
    if ( i2cdrv_semid == -1 )
    {
        i2cdrv_initialized = 1;
        i2cdrv_semid = semget ( 0x0812, 1, IPC_CREAT | 0666 );
        if ( i2cdrv_semid == -1 )
        {
            printf ( "i2cdrv_semid=%d\n", i2cdrv_semid );
            return -1;
        }
    }
    else
    {

        i2cdrv_initialized = 0;
        arg.val = 1;
#if defined(H20RN181)
        ret = semctl ( i2cdrv_semid, 0, SETVAL, arg.val );
#endif
#if defined(H20RN1000) || defined(H20RN2000) || defined(HT201)
        ret = semctl ( i2cdrv_semid, 0, SETVAL, arg );
#endif

    }

    i2cdrv_sembufLock.sem_num = 0;
    i2cdrv_sembufLock.sem_op = -1;
    i2cdrv_sembufLock.sem_flg = SEM_UNDO;

    i2cdrv_sembufUnlock.sem_num = 0;
    i2cdrv_sembufUnlock.sem_op = 1;
    i2cdrv_sembufUnlock.sem_flg = SEM_UNDO;
    return ret;
}


int i2cdrv_init ( void *ptr )
{
    int ret;

//    memset ( &spidev, 0, sizeof ( spidev ) );

    if ( i2cdrv_semlock_init() )
    {
        printf ( "i2cdrv semlock init err\n" );
        return -1;
    }
    if ( i2cdrv_mmap_init ( ptr ) )
    {
        printf ( "i2cdrv mmap init err\n" );
        return -1;
    }
//	i2c_dev[1] = (struct fsl_i2c *) (mapbase + CONFIG_SYS_I2C2_OFFSET);
    if ( i2cdrv_setup_init() )
    {
        printf ( "i2cdrv setup init error\n" );
        return -1;
    }
    printf ( "Initialize i2cdrv successfully\n" );

    return 0;
}


int i2cdrv_exit ( void )
{
    i2cdrv_mmap_exit();
    return 0;
}




