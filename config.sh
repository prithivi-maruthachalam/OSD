#!/bin/bash
# Defines the various environment variables we need for compiling

# list of major project directories
MAJOR_FOLDERS="libc kernel"

# for the host prefix
export TARGET_HOST="i686-elf"
export TARGET_ARCH=$(./target-to-arch.sh ${TARGET_HOST})

# binutils
export AR="$TARGET_HOST-ar" # to create the archive of .o files
export CC="$TARGET_HOST-gcc"
export AS="$TARGET_HOST-as"

# filesystem
export FILE_PREFIX="/usr"
export LIB_DIR="$FILE_PREFIX/lib"
export INCLUDE_DIR="$FILE_PREFIX/include"
export BOOT_DIR="/boot"

# sysroot
export SYSROOT="$(pwd)/sysroot"
export CC="$CC --sysroot=$SYSROOT"

if echo $TARGET_HOST | grep -Eq -- '-elf$'
then
    CC="$CC -isystem=$INCLUDE_DIR"
fi