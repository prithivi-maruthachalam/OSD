CFLAGS=-02 -g -ffreestanding -Wall -Wextra

FREE_OBJS=\
$(ARCH_FREEOBJS) \
stdio/printf.o \
stdio/putchar.o \
stdio/puts.o \
stdlib/abort.o \
string/memcmp.o \
string/memcpy.o \
string/memmove.o \
string/memset.o \
string/strlen.o \

HOSTED_OBJS=\
$(ARCH_HOSTEDOBJS)

OBJS=$(FREE_OBJS) $(HOSTED_OBJS)

TARGET_DIR=$(FILE_PREFIX)/local
INCLUDE_DIR=$(TARGET_DIR)/include
LIB_DIR=$(TARGET_DIR)/lib


.PHONY: install install-headers install-libs



install: install-headers install-libs

install-headers:
	mkdir -p $(DEST_DIR)$(INCLUDE_DIR)
	cp -R --preserve=timestamps include/. $(DEST_DIR)$(INCLUDE_DIR)/