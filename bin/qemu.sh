#!/bin/sh
# set -e

. ./bin/iso.sh

mkdir -p logs && touch logs/log.txt
qemu-system-i386 -m 4G -serial file:logs/log.txt -cdrom theOS.iso
