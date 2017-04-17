#!/bin/bash

export PATH=/opt/arm/usr/bin:$PATH

rm libi2cdrv.so
rm eeprom

arm-linux-gcc i2c.c i2cdrv.c eepdrv.c -shared -fPIC -o libi2cdrv.so

arm-linux-gcc eeprom.c -o eeprom -li2cdrv -L ./


cp libi2cdrv.so /home/work/share/
cp eeprom /home/work/share/

echo "copy libi2cdrv.so success!"
