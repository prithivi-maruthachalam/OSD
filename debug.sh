#!/bin/sh
# set -e

. ./iso.sh

qemu-system-i386 -s -S -cdrom theOS.iso