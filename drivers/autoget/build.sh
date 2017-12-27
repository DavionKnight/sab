#!/bin/bash

export PATH=/opt/arm/usr/bin:$PATH

arm-linux-gcc autoget.c -o autoget

cp autoget /tftpboot/


