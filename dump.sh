#!/bin/sh
# set -e

file=$1

. ./build.sh
objdump -l -d -f -h sysroot/boot/theOS.kernel > ${file:='t_disAsm.txt'}