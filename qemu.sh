#!/bin/sh
# set -e

. ./iso.sh

qemu-system-i386 -m 4G -serial file:logs/log.txt -cdrom theOS.iso
