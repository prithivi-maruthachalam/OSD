INCLUDE_DIR=$(FILE_PREFIX)/include
LIB_DIR=$(FILE_PREFIX)/lib

#objs that we created
FREE_OBJS=\
$(ARCH_FREEOBJS) \
stdio/printf.o \
stdio/logf.o \
stdio/putchar.o \
stdio/puts.o \
stdlib/abort.o \
stdlib/itoa.o \
string/memcmp.o \
string/memcpy.o \
string/memmove.o \
string/memset.o \
string/strlen.o \
utils/ceilPowTwo.o \

#we will get ehere later from libc.
HOSTED_OBJS=\
$(ARCH_HOSTEDOBJS)

#libc objs - also contains the free objs
OBJS=$(FREE_OBJS) $(HOSTED_OBJS)
#libk objs - just the free standing ones
LIBK_OBJS=$(FREE_OBJS:.o=.libk.o)


#the requied binaries - the archive
#we'll put in a libc.a in the future
BIN_ARCHIVES=libk.a 


#flags
CFLAGS=-O2 -g -ffreestanding -Wall -Wextra
LIBK_CFLAGS=$(CFLAGS) -D__is_libk -Iinclude


.PHONY: install-headers install-libs clean object-install

# basically just putting the headers into the sysroot include directory
install-headers:
	mkdir -p $(DEST_DIR)$(INCLUDE_DIR)
	cp -R --preserve=timestamps include/* $(DEST_DIR)$(INCLUDE_DIR)/

# to create the bin archives
libk.a: $(LIBK_OBJS)
	$(AR) rcs libk.a $(LIBK_OBJS)

libc.a: $(OBJS)
	$(AR) rcs libc.a $(OBJS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)
%.libk.o: %.c
	$(CC) -c $< -o $@ $(LIBK_CFLAGS)

#creating the archive and then placing it in sysroot's lib directory
install-libs: $(BIN_ARCHIVES)
	mkdir -p $(DEST_DIR)$(LIB_DIR)
	cp $(BIN_ARCHIVES) $(DEST_DIR)$(LIB_DIR)/

clean:
	rm -f $(BIN_ARCHIVES)
	rm -f $(LIBK_OBJS)
	rm -f $(LIBK_OBJS:.o=.d) *.d

object-install: install-libs