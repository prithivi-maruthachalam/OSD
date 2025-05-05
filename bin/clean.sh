#!/bin/sh

set -e
. ./bin/config.sh

for FOLDER in $MAJOR_FOLDERS
do
    cd $FOLDER && make clean
    cd ../
done

rm -rf sysroot
rm -rf isodir
rm -f *.iso
