#!/bin/sh
# set -e

. ./iso.sh

mkdir logs && logs/log.txt
qemu-system-i386 -m 4G -serial file:logs/log.txt -cdrom theOS.iso
