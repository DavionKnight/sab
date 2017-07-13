#!/bin/bash

export PATH=/opt/arm/usr/bin:$PATH

arm-linux-gcc ramtest.c -o ramtest 

cp ramtest /tftpboot/


