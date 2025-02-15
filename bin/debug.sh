#!/bin/sh
# set -e

. ./bin/iso.sh

qemu-system-i386 -s -S -cdrom theOS.iso &
gdb -ex "set architecture i386:x86-64:intel" -ex "target remote localhost:1234" -ex "symbol-file sysroot/boot/theOS.kernel"