#!/bin/bash

export PATH=/opt/arm/usr/bin:$PATH
TARGET_ARCH=arm


arm-linux-gcc netlink_state.c -o netlink_state
arm-linux-gcc eth0ioctl.c -o eth0ioctl 
arm-linux-gcc rawSocket.c -o rawSocket -lpthread
arm-linux-gcc vitess.c -o vitess 

cp netlink_state /tftpboot/
cp eth0ioctl /tftpboot/
cp rawSocket /tftpboot/
cp vitess /tftpboot/


