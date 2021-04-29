#!/bin/sh
# set -e

. ./iso.sh

qemu-system-i386 -m 4G -d int -cdrom theOS.iso
