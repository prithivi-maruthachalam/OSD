#!/bin/sh
# set -e

. ./build.sh # this will create and populate sysroot with the executables

mkdir -p isodir/boot/grub
cp sysroot/boot/theOS.kernel isodir/boot/theOS.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentr "theOS" {
    multiboot /boot/theOS.kernel
}
EOF

mkdir -o grub-mkrescue theOS.iso