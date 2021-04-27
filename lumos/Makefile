INCLUDE_DIR=$(FILE_PREFIX)/include
LIB_DIR=$(FILE_PREFIX)/lib
BOOT_DIR=/boot

ARCH_DIR=arch/$(TARGET_ARCH)

KERNEL_LIBS=\

KERNEL_ARCH_OBJS=\
$(ARCH_DIR)/boot.o	\
$(ARCH_DIR)/tty.o	\
$(ARCH_DIR)/loadGDT.o	\
$(ARCH_DIR)/loadIDT.o	\
$(ARCH_DIR)/gdt.o	\
$(ARCH_DIR)/isr.o	\
$(ARCH_DIR)/timer.o	\
$(ARCH_DIR)/keyboard.o	\
$(ARCH_DIR)/pmm.o	\


KERNEL_OBJS=\
$(KERNEL_ARCH_OBJS)	\
kernel/kernel.o	\
$(ARCH_DIR)/idt.o	\
$(ARCH_DIR)/interrupts.o	\



CFLAGS=-O2 -g -ffreestanding -Wall -Wextra
LIBK_CFLAGS=-D__is_libk -Iinclude
KERNEL_CFLAGS=-D__is_kernel -Iinclude
LINK_FLAGS= -nostdlib -lk -lgcc

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
	$(CC) -c $< -o $@ $(CFLAGS) $(KERNEL_CFLAGS) 

%.o: %.S 
	$(CC) -c $< -o $@ $(CFLAGS) $(KERNEL_CFLAGS)

$(ARCH_DIR)/crtbegin.o $(ARCH_DIR)/crtend.o:
	OBJ=`$(CC) $(CFLAGS) $(LIBK_CFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

install-kernel: theOS.kernel
	mkdir -p $(DEST_DIR)$(BOOT_DIR)
	cp theOS.kernel $(DEST_DIR)$(BOOT_DIR)/

install-headers:
	mkdir -p $(DEST_DIR)$(INCLUDE_DIR)
	cp -R --preserve=timestamps include/* $(DEST_DIR)$(INCLUDE_DIR)

clean:
	rm -f theOS.kernel
	rm -f $(OBJS) *.o
	rm -f $(OBJS:.o=.d) $(LIBK_OBJS:.o=.d) *.d

object-install: install-kernel