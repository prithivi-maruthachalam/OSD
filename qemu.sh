#!/bin/sh
# set -e

. ./iso.sh

qemu-system-i386 -d cpu_reset -cdrom theOS.iso
