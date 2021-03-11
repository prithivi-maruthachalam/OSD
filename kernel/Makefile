INCLUDE_DIR=$(FILE_PREFIX)/include
LIB_DIR=$(FILE_PREFIX)/lib
BOOT_DIR=/boot

ARCH_DIR=./arch/1686

KERNEL_LIBS=\

KERNEL_ARCH_OBJS=\
$(ARCH_DIR)/boot.S \
$(ARCH_DIR)/tty.c

KERNEL_OBJS=\
$(KRENEL_ARCH_OBJS) \
kernel/kernel.o

OBJS=\
$(ARCH_DIR)/crti.o\
$(ARCH_DIR)/crtbegin.o\
$(KERNEL_OBJS) \
$(ARCH_DIR)/crtend.o\
$(ARCH_DIR)/crtn.o\

CFLAGS=-O2 -g -ffreestanding -Wall -Wextra
LIBK_CFLAGS=-D__is_libk -Iinclude
LINK_FLAGS=-nostdlib -lk -lgcc

.PHONY: install-headers install-kernel clean object-install

#generate the .kernel(bin) and place it in the boot directory
theOS.kernel $(OBJS) $(ARCH_DIR)/linker.ld: 
	$(CC) -T $(ARCH_DIR)/linker.ld $(CFLAGS) $(LINK_FLAGS) $(OBJS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(LIBK_CFLAGS)

%.o: %.S 
	$(CC) -c -$< -o $@ $(CFLAGS) $(LIBK_CFLAGS) 

install-kernel: theOS.kernel
	echo "kernel: installing .kernel"
	mkdir -p $(DEST_DIR)$(BOOT_DIR)
	cp theOS.kernel $(DEST_DIR)$(BOOT_DIR)/
	echo "kernel: done installing .kernel\n\n"

install-headers:
	echo "kernel: installing headers"
	mkdir -p $(DEST_DIR)$(INCLUDE_DIR)
	cp -R --preserve=timestamps include/* $(DEST_DIR)$(INCLUDE_DIR)
	echo "kernel: done installing headers\n\n"

clean:
	rm -f theOS.kernel
	rm -f $(OBJS) *.o

object-install: install-kernel