INCLUDE_DIR=$(FILE_PREFIX)/include
LIB_DIR=$(FILE_PREFIX)/lib
BOOT_DIR=/boot

ARCH_DIR=arch/i686

KERNEL_LIBS=\

KERNEL_ARCH_OBJS=\
$(ARCH_DIR)/boot.o	\
$(ARCH_DIR)/tty.o	\

KERNEL_OBJS=\
$(KERNEL_ARCH_OBJS)	\
kernel/kernel.o	\



CFLAGS=-O2 -g -ffreestanding -Wall -Wextra
LIBK_CFLAGS=-D__is_libk -Iinclude
LINK_FLAGS= -nostdlib -lk -lgcc

OBJS_PRIM=\
$(ARCH_DIR)/crti.o	\
$(ARCH_DIR)/crtbegin.o	\
$(KERNEL_OBJS)	\
$(ARCH_DIR)/crtend.o	\
$(ARCH_DIR)/crtn.o	\

OBJS=\
$(ARCH_DIR)/crti.o	\
$(ARCH_DIR)/crtbegin.o	\
$(KERNEL_OBJS)	\
$(ARCH_DIR)/crtend.o	\
$(ARCH_DIR)/crtn.o	\

.PHONY: install-headers install-kernel clean object-install

#generate the .kernel(bin) and place it in the boot directory
theOS.kernel: $(OBJS) $(ARCH_DIR)/linker.ld
	$(CC) -T $(ARCH_DIR)/linker.ld -o $@ $(CFLAGS) $(OBJS) $(LINK_FLAGS)

%.o: %.c
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS) $(LIBK_CFLAGS)

%.o: %.S 
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(LIBK_CFLAGS)

$(ARCH_DIR)/crtbegin.o $(ARCH_DIR)/crtend.o:
	OBJ=`$(CC) $(CFLAGS) $(LIBK_CFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@
	echo $(OBJ)


install-kernel: theOS.kernel
	echo "kernel: installing .kernel"
	mkdir -p $(DEST_DIR)$(BOOT_DIR)
	cp theOS.kernel $(DEST_DIR)$(BOOT_DIR)/
	echo "kernel: done installing .kernel\n\n"
	pwd

install-headers:
	echo "kernel: installing headers"
	mkdir -p $(DEST_DIR)$(INCLUDE_DIR)
	cp -R --preserve=timestamps include/* $(DEST_DIR)$(INCLUDE_DIR)
	echo "kernel: done installing headers\n\n"

clean:
	rm -f theOS.kernel
	rm -f $(OBJS_PRIM) *.o
	rm -f $(OBJS_PRIM:.o=.d) $(LIBK_OBJS:.o=.d) *.d

object-install: install-kernel