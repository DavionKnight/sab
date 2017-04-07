#If KERNELRELEASE is defined, we've been invoked from the

# kernel build system and can use its language.
#CC = /opt/cross/eldk42/usr/bin/ppc_82xx-gcc
#CCDEPMODE = depmode=none
#CFLAGS = -g -O2
#CPP = /opt/cross/eldk42/usr/bin/ppc_82xx-gcc -E
#CPPFLAGS = 
#CRLDP = 
#CRLDPINCLUDE = 
#CRLDPLIB = 
#CURSES = 
#2.6.18-8.el5
KERNELDIR ?= /home/kevin/works/projects/H20RN-2000/linux-2.6-cloud-2000

ifneq ($(KERNELRELEASE),)

obj-m += outband.o
outband-objs := drv_bcm53101.o drv_outband.o

a:

	echo defined	 

# Otherwise we were called directly from the command

# line; invoke the kernel build system.

else

	KERNELDIR ?= /lib/modules/$(shell uname -r)/build

	PWD := $(shell pwd) 

default:

	echo no defined

	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif

clean: 
	rm *.ko *.mod.c *.o *.symvers *.order


