#!/bin/bash
# Defines the various environment variables we need for compiling

# list of major project directories
MAJOR_FOLDERS="libc kernel"

# for the host prefix
export HOST="i686-elf"

# binutils
export AR="$HOST-ar" # to create the archive of .o files
export CC="$HOST-gcc"
export AS="$HOST-as"

# filesystem
export FILE_PREFIX="/usr"
export LIB_DIR="$FILE_PREFIX/lib"
export INCLUDE_DIR="$FILE_PREFIX/include"
export BOOT_DIR="/boot"

# sysroot
export SYSROOT="$(pwd)/sysroot"
export CC="$CC --sysroot=$SYSROOT"

if echo $HOST | grep -Eq -- '-elf$'
then
    CC="$CC -isystem=$INCLUDE_DIR"
fi

# TODO: Deal with the i686 and i386 thing