cscope -Rb
ctags -R
export PATH=/opt/arm/usr/bin:/opt/arm/bin:$PATH
#export PATH=/opt/mips/eldk4.1/usr/bin:/opt/mips/eldk4.1/bin:$PATH

#mips_4KC-gcc spi.c spidrv.c fpgadrv.c dplldrv.c  --shared -fPIC  -o libspidrv.so -DH20RN181 -L ./
rm libspidrv.so
rm fpga
arm-linux-gcc spi.c spidrv.c fpgadrv.c dplldrv.c  gpiodrv.c --shared -fPIC  -o libspidrv.so -DH20RN181_ARM 
arm-linux-gcc fpga.c -o fpga -lspidrv -L ./
arm-linux-gcc dpll.c -o dpll -lspidrv -L ./
#arm-linux-gcc epcs.c -o epcs -lspidrv -L ./
#arm-linux-gcc eeprom.c -o eeprom -lspidrv -L ./

cp libspidrv.so /tftpboot
cp fpga /tftpboot
cp dpll /tftpboot
#cp epcs /tftpboot
#cp eeprom /tftpboot



