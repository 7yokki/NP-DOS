PROJECT := np-dos
BUILD := build
ISO_ROOT := $(BUILD)/iso
CC := gcc
LD := ld
AS := nasm
OBJCOPY := objcopy

CFLAGS := -std=gnu11 -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -mno-sse -mno-sse2 -mno-mmx -mcmodel=kernel -m64 -O2 -Wall -Wextra -Werror -Iinclude
LDFLAGS := -nostdlib -no-pie -z max-page-size=0x1000 -T boot/linker.ld

KERNEL_C := $(wildcard kernel/*.c drivers/*.c fs/*.c lib/*.c)
KERNEL_OBJ := $(patsubst %.c,$(BUILD)/%.o,$(KERNEL_C))
BOOT_OBJ := $(BUILD)/boot/boot.o $(BUILD)/boot/interrupts.o

.PHONY: all clean iso run test apps

all: iso apps

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/boot/boot.o: boot/boot.asm
	@mkdir -p $(dir $@)
	$(AS) -f elf64 $< -o $@

$(BUILD)/boot/interrupts.o: boot/interrupts.asm
	@mkdir -p $(dir $@)
	$(AS) -f elf64 $< -o $@

$(BUILD)/npdos.elf: $(BOOT_OBJ) $(KERNEL_OBJ)
	$(LD) $(LDFLAGS) -o $@ $^
	$(OBJCOPY) --only-keep-debug $@ $(BUILD)/npdos.debug

iso: $(BUILD)/npdos.elf
	@mkdir -p $(ISO_ROOT)/boot/grub
	cp $(BUILD)/npdos.elf $(ISO_ROOT)/boot/npdos.elf
	cp boot/grub.cfg $(ISO_ROOT)/boot/grub/grub.cfg
	grub-mkrescue -o $(BUILD)/$(PROJECT).iso $(ISO_ROOT) >/dev/null
	grub-file --is-x86-multiboot2 $(BUILD)/npdos.elf

$(BUILD)/hello-asm.o: apps/hello.asm
	@mkdir -p $(dir $@)
	$(AS) -f elf64 $< -o $@

$(BUILD)/hello-asm.elf: $(BUILD)/hello-asm.o
	$(LD) -nostdlib -z max-page-size=0x1000 -T apps/app.ld -o $@ $<

$(BUILD)/hello-c.o: apps/hello.c
	@mkdir -p $(dir $@)
	$(CC) -std=gnu11 -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -mno-sse -mno-sse2 -mno-mmx -m64 -O2 -Wall -Wextra -Werror -c $< -o $@

$(BUILD)/hello-c.elf: $(BUILD)/hello-c.o
	$(LD) -nostdlib -z max-page-size=0x1000 -T apps/app.ld -o $@ $<

apps: $(BUILD)/hello-asm.elf $(BUILD)/hello-c.elf
	readelf -h $(BUILD)/hello-asm.elf | grep -E 'Class:|Machine:|Type:'
	readelf -h $(BUILD)/hello-c.elf | grep -E 'Class:|Machine:|Type:'

test: iso apps
	bash tests/elf-test.sh
	bash tests/boot-test.sh

run: iso
	qemu-system-x86_64 -cdrom $(BUILD)/$(PROJECT).iso -m 128M -display none -serial stdio

clean:
	rm -rf $(BUILD)
