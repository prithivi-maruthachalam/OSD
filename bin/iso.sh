#!/bin/sh
# set -e

. ./bin/build.sh # this will create and populate sysroot with the executables

mkdir -p isodir/boot/grub
cp sysroot/boot/theOS.kernel isodir/boot/theOS.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "theOS" {
    multiboot /boot/theOS.kernel
}
EOF

grub-mkrescue /usr/lib/grub/i386-pc -o theOS.iso isodir