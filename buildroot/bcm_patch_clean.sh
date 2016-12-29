#!/bin/bash

./bcm_patch.sh

## some hack for clean
make -C output/build/linux-3.6.5 ARCH=arm clean
rm -f output/build/linux-3.6.5/arch/arm/configs/buildroot_defconfig
make -C output/build/uboot-2012.10  distclean
rm   -f output/build/uboot-2012.10/*_config
