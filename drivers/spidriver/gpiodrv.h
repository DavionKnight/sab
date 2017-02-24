/*
*  COPYRIGHT NOTICE
*  Copyright (C) 2016 HuaHuan Electronics Corporation, Inc. All rights reserved
*
*  Author       	:fzs
*  File Name        	:/home/kevin/works/projects/H20RN-2000/drivers/gpio/gpiodrv.h
*  Create Date        	:2016/10/26 19:55
*  Last Modified      	:2016/10/26 19:55
*  Description    	:
*/

/*初始化gpio驱动*/
int gpiodrv_init();

/*gpio驱动退出*/
void gpiodrv_exit();

/*获取相应gpio上的值*/
unsigned char gpio_get_value(unsigned gpio);

/*设置gpio为输入*/
int gpio_direction_input(unsigned gpio);

/*设置gpio值为value,该函数只用于看门狗，其他不用*/
int gpio_output(unsigned gpio, unsigned char value);

/*设置gpio为输出，值为value*/
int gpio_direction_output(unsigned gpio, unsigned char value);


/******************watchdog interface**************************/

/*设备复位*/
int device_rst(void);

/*看门狗使能，必须首先调用*/
int wdt_en(void);

/*看门狗喂狗*/
int wdt_wdi(void);

/*将看门狗设置为长周期*/
int wdt_lp_set(void);

/*将看门狗设置为短周期*/
int wdt_sp_set(void);






