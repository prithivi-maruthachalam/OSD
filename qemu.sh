#!/bin/sh
# set -e

. ./iso.sh

qemu-system-i386 -d int -cdrom theOS.iso
