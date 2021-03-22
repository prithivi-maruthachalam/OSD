#!/bin/sh
# set -e

file=$1

. ./build.sh
mkdir -p dumpDir/ && objdump -l -D -f -h sysroot/boot/theOS.kernel > dumpDir/${file:='t_disAsm.txt'}