#!/bin/bash

#export PATH=/opt/arm/usr/bin:$PATH
export PATH=/opt/ppc/eldk4.2/usr/bin:$PATH

ppc_85xxDP-gcc getmsg.c -o getmsg

#cp HHlogin /tftpboot/


