#!/bin/sh
# set -e

. ./headers.sh

for FOLDER in $MAJOR_FOLDERS
do
    cd $FOLDER && DEST_DIR="$SYSROOT" make object-install
    cd ../
done
