#!/bin/bash
#
#  COPYRIGHT NOTICE
#  Copyright (C) 2016 HuaHuan Electronics Corporation, Inc. All rights reserved
#
#  Author       	:fzs
#  File Name        	:/home/kevin/works/projects/XLDK-3.6.2/XLDK/bootloader/u-boot-2012.10-rc3/build.sh
#  Create Date        	:2017/01/19 15:22
#  Last Modified      	:2017/01/19 15:22
#  Description    	:
#
export PATH=$PATH:/opt/arm/usr/bin

function setup_product_string()
{
if test $1 == "ht201"
then
	sed -i 's/dnu=tftp 0x61000000 u-boot-saber2.bin;/dnu=tftp 0x61000000 ht201_main_uboot;/g' ./include/configs/saber2.h
	sed -i 's/dnu=tftp 0x61000000 ht201e_main_uboot;/dnu=tftp 0x61000000 ht201_main_uboot;/g' ./include/configs/saber2.h
	sed -i 's/"u-boot"/"ht201_main_uboot"/g' ./net/net.c
	sed -i 's/"ht201e_main_uboot"/"ht201_main_uboot"/g' ./net/net.c
	sed -i 's/saber2-ubi.fs/ht201_main_kernel/g' ./include/configs/saber2.h
	sed -i 's/ht201e_main_kernel/ht201_main_kernel/g' ./include/configs/saber2.h

else
	sed -i 's/dnu=tftp 0x61000000 u-boot-saber2.bin;/dnu=tftp 0x61000000 ht201e_main_uboot;/g' ./include/configs/saber2.h
	sed -i 's/dnu=tftp 0x61000000 ht201_main_uboot;/dnu=tftp 0x61000000 ht201e_main_uboot;/g' ./include/configs/saber2.h
	sed -i 's/"u-boot"/"ht201e_main_uboot"/g' ./net/net.c
	sed -i 's/"ht201_main_uboot"/"ht201e_main_uboot"/g' ./net/net.c
	sed -i 's/saber2-ubi.fs/ht201e_main_kernel/g' ./include/configs/saber2.h
	sed -i 's/ht201_main_kernel/ht201e_main_kernel/g' ./include/configs/saber2.h
fi
}

#make ht201
if [ "$1" == "ht201" ];then

setup_product_string $1

cd ../../buildroot
rm output/images/u-boot.bin
make saber2-uboot_defconfig
make clean
make

if [ $? -ne 0 ]                                                                       
then                                                                                  
echo "====make ht201 error!!!===="
exit                                                                               
fi 

cp output/images/u-boot.bin /tftpboot/ht201_main_uboot
if [ $? -ne 0 ]                                                                       
then                                                                                  
echo "====make ht201 error!!!===="
exit                                                                               
fi 
echo "====cp ht201_main_uboot to tftpboot OK===="

#make ht201e
elif [ "$1" == "ht201e" ];then

setup_product_string $1

cd ../../buildroot
rm output/images/u-boot.bin
make saber2-uboot_defconfig
make clean
make

if [ $? -ne 0 ]                                                                       
then                                                                                  
echo "====make ht201e error!!!===="
exit                                                                               
fi 

cp output/images/u-boot.bin /tftpboot/ht201e_main_uboot
if [ $? -ne 0 ]                                                                       
then                                                                                  
echo "====make ht201 error!!!===="
exit                                                                               
fi 
echo "====cp ht201e_main_uboot to tftpboot OK===="

#make all
elif [ "$1" == "all" ];then

./build.sh ht201
if [ $? -ne 0 ]                                                                       
then                                                                                  
echo "====make ht201 error!!!===="
exit                                                                               
fi 
./build.sh ht201e
if [ $? -ne 0 ]                                                                       
then                                                                                  
echo "====make ht201e error!!!===="
exit                                                                               
fi 

echo "====cp ht201_main_uboot to tftpboot OK===="
echo "====cp ht201e_main_uboot to tftpboot OK===="

else
echo "./build.sh [ht201 | ht201e | all]"
fi


