#!/bin/bash
# set -e

. ./config.sh

# first "installer" after config, so make sysroot dir
echo "Doing headers shit"
mkdir -p $SYSROOT

for FOLDER in $MAJOR_FOLDERS
do
    cd $FOLDER && DEST_DIR="$SYSROOT" make install-headers
    cd ../
done
