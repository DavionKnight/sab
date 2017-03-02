#!/bin/sh

export ARCH=arm

export PATH=/opt/arm/usr/bin:$PATH
export CROSS_COMPILE=arm-linux-

make Image
cp ./arch/arm/boot/Image ../../mkubi/Image
echo cp ./arch/arm/boot/Image ../../mkubi/Image
cd ../../mkubi
./build.sh
#cp ./arch/powerpc/boot/p1020rdb-pc.dtb ../mkubi/bin/1604-dtb
#cp ./arch/powerpc/boot/uImage ../mkubi/bin/uImage

