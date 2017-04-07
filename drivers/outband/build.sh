#!/bin/bash
ctags -R
cscope -Rbq

export KPATH=/home/kevin/works/projects/XLDK/kernel/linux-3.6.5
export K_SRC_PATH=/home/kevin/works/projects/XLDK/kernel/linux-3.6.5 

export PATH=/opt/arm/usr/bin:$PATH
TARGET_ARCH=arm

#export CROSS_COMPILE=arm-linux-


rm ./build -rf


#make ARCH=arm CROSS_COMPILE=arm-linux- 

arm-linux-gcc netlink_state.c -o netlink_state
arm-linux-gcc getstate.c -o getstate
arm-linux-gcc eth0ioctl.c -o eth0ioctl 
arm-linux-gcc rawSocket.c -o rawSocket -lpthread

cp outband.ko /tftpboot/
cp netlink_state /tftpboot/
cp getstate /tftpboot/
cp eth0ioctl /tftpboot/
cp rawSocket /tftpboot/


