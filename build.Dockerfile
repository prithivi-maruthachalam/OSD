FROM debian:bookworm-slim

# internal ARGS
ARG BINUTILS="binutils_source"
ARG GCC="gcc_source"
ARG ARCHIVE_EXT=".tar.xz"

RUN apt-get update \
    && apt-get upgrade -y

# add i386 architecture because that's what we're building lumos for
RUN dpkg --add-architecture i386 \
    && apt-get update \
    && apt-get upgrade -y

# install dev dependencies
RUN apt-get install -y \
    build-essential	\
    bison \
    flex \
    libgmp3-dev \
    libmpc-dev \
    libmpfr-dev \
    texinfo \
    libisl-dev \
    wget \
    grub-pc-bin:i386 \
    shim-unsigned \
    xorriso \
    mtools

# variables for cross compiler build
ENV HOME=/root
ENV PREFIX="$HOME/opt/cross"
ENV TARGET="i686-elf"
ENV PATH="$PREFIX/bin:$PATH"
ENV SRC="$HOME/src"

# directories for cross compiler installation
RUN mkdir -p ${PREFIX}
RUN mkdir ${SRC}

# download binutils
WORKDIR ${SRC}
RUN wget https://sourceware.org/pub/binutils/snapshots/binutils-2.43.90.tar.xz -O "$BINUTILS$ARCHIVE_EXT"
RUN mkdir $BINUTILS && tar -xf "$BINUTILS$ARCHIVE_EXT" -C $BINUTILS --strip-components 1
RUN rm "$BINUTILS$ARCHIVE_EXT"

# download gcc
WORKDIR ${SRC}
RUN wget https://sourceware.org/pub/gcc/snapshots/14-20250201/gcc-14-20250201.tar.xz -O "$GCC$ARCHIVE_EXT"
RUN mkdir $GCC && tar -xf "$GCC$ARCHIVE_EXT" -C $GCC --strip-components 1
RUN rm "$GCC$ARCHIVE_EXT"

# build and install binutils
RUN mkdir ${SRC}/build-binutils
WORKDIR ${SRC}/build-binutils
RUN ${SRC}/$BINUTILS/configure \
    --target=${TARGET} \
    --prefix=${PREFIX} \
    --with-sysroot \
    --disable-nls \
    --disable-werror
RUN make && make install

# build and install gcc
RUN which -- $TARGET-as || (echo $TARGET-as is not in the PATH && exit 1)
RUN mkdir ${SRC}/build-gcc
WORKDIR ${SRC}/build-gcc
RUN ${SRC}/${GCC}/configure \
    --target=${TARGET} \
    --prefix=${PREFIX} \
    --disable-nls \
    --enable-languages=c,c++ \
    --without-headers \
    --disable-hosted-libstdcxx
RUN make all-gcc \
    && make all-target-libgcc \
    && make all-target-libstdc++-v3 \
    && make install-gcc && \
    make install-target-libgcc \
    && make install-target-libstdc++-v3
