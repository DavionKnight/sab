#!/bin/sh

export PATH=/opt/arm/usr/bin:$PATH
mkimage -A arm -O linux -T script -C none -a 0 -e 0 -n "set envs but address script" -d ./set_env.script ./set_env.uscr
#cp ./set_env.uscr ../Release_version

