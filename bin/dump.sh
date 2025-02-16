#!/bin/sh
# set -e

file=$1

. ./bin/build.sh
mkdir -p dumpDir/ && objdump -l -D -f -h -x -s sysroot/boot/theOS.kernel > dumpDir/${file:='t_disAsm.txt'}