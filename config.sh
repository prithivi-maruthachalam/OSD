# Defines the various environment variables we need for compiling

# list of major project directories
MAJOR_FOLDERS="libc kernel"

# for the host prefix
export HOST="i386-elf"

# binutils
export AR="$HOST-ar" # to create the archive of .o files
export CC="$HOST-gcc"
export AS="$HOST-as"

# filesystem
export PRE="/usr"
export LIB="$PRE/lib"
export INCLUDE_DIR="$PRE/include"
export BOOT_DIR="/boot"

# sysroot
export SYSROOT="$(pwd)/sysroot"
export CC="$CC --sysroot=$SYSROOT"

if echo $HOST | grep -Eq -- '-elf$'
then
    CC="$CC -isystem=$INCLUDE_DIR"
fi
