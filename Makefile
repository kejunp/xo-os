# XO-OS Makefile
# Builds both the UEFI bootloader and kernel

# Toolchain
CC = clang
LD = ld.lld
OBJCOPY = llvm-objcopy

# Directories
BOOT_DIR = boot
KERNEL_DIR = kernel
BUILD_DIR = build
ESP_DIR = $(BUILD_DIR)/esp

# Compiler flags
CFLAGS = -target x86_64-unknown-windows -ffreestanding -fno-stack-protector \
         -fno-stack-check -fshort-wchar -mno-red-zone -Wall -Wextra \
         -I. -Wno-unused-parameter

# Bootloader specific flags
BOOT_CFLAGS = $(CFLAGS) -DGNU_EFI_USE_MS_ABI -fno-builtin

# Kernel specific flags
KERNEL_CFLAGS = -target x86_64-elf -ffreestanding -fno-stack-protector \
                -mcmodel=kernel -mno-mmx -mno-sse -mno-sse2 -Wall -Wextra \
                -I$(KERNEL_DIR) -Wno-unused-parameter

# Linker flags
BOOT_LDFLAGS = -target x86_64-unknown-windows -nostdlib -Wl,-entry:efi_main \
               -Wl,-subsystem:efi_application -fuse-ld=lld -Wl,-stack:0x100000

KERNEL_LDFLAGS = -T $(KERNEL_DIR)/linker.ld -nostdlib

# Source files
BOOT_SOURCES = $(BOOT_DIR)/boot.c
KERNEL_SOURCES = $(KERNEL_DIR)/main.c

# Target files
BOOTLOADER_EFI = $(BUILD_DIR)/BOOTX64.EFI
KERNEL_ELF = $(BUILD_DIR)/kernel.elf

# Default target
all: $(BOOTLOADER_EFI) $(KERNEL_ELF) esp

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build bootloader
$(BOOTLOADER_EFI): $(BOOT_SOURCES) $(BUILD_DIR)
	$(CC) $(BOOT_CFLAGS) $(BOOT_LDFLAGS) -o $@ $(BOOT_SOURCES)

# Build kernel
$(KERNEL_ELF): $(KERNEL_SOURCES) $(BUILD_DIR)
	$(CC) $(KERNEL_CFLAGS) -c -o $(BUILD_DIR)/main.o $(KERNEL_DIR)/main.c
	$(LD) $(KERNEL_LDFLAGS) -o $@ $(BUILD_DIR)/main.o

# Create ESP (EFI System Partition) layout
esp: $(BOOTLOADER_EFI) $(KERNEL_ELF)
	mkdir -p $(ESP_DIR)/EFI/BOOT
	cp $(BOOTLOADER_EFI) $(ESP_DIR)/EFI/BOOT/
	cp $(KERNEL_ELF) $(ESP_DIR)/kernel.elf

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

# Create a disk image for testing
disk-image: esp
	@echo "Creating disk image for QEMU testing..."
	mkdir -p $(BUILD_DIR)/mnt
	dd if=/dev/zero of=$(BUILD_DIR)/disk.img bs=1M count=64
	parted $(BUILD_DIR)/disk.img mklabel gpt
	parted $(BUILD_DIR)/disk.img mkpart ESP fat32 1MiB 63MiB
	parted $(BUILD_DIR)/disk.img set 1 esp on
	@echo "Formatting ESP partition..."
	sudo losetup -P /dev/loop0 $(BUILD_DIR)/disk.img
	sudo mkfs.fat -F32 /dev/loop0p1
	sudo mount /dev/loop0p1 $(BUILD_DIR)/mnt
	sudo cp -r $(ESP_DIR)/* $(BUILD_DIR)/mnt/
	sudo umount $(BUILD_DIR)/mnt
	sudo losetup -d /dev/loop0

# Test with QEMU
test: disk-image
	qemu-system-x86_64 -drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE.fd \
	                    -drive if=pflash,format=raw,file=/usr/share/OVMF/OVMF_VARS.fd \
	                    -drive format=raw,file=$(BUILD_DIR)/disk.img \
	                    -m 512M -enable-kvm -cpu host

# Quick test without disk image (if you have ESP files available)
test-quick: esp
	qemu-system-x86_64 -drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE.fd \
	                    -drive if=pflash,format=raw,file=/usr/share/OVMF/OVMF_VARS.fd \
	                    -drive format=raw,file=fat:rw:$(ESP_DIR) \
	                    -m 512M -enable-kvm -cpu host

# Show file information
info: $(BOOTLOADER_EFI) $(KERNEL_ELF)
	@echo "=== Bootloader Info ==="
	file $(BOOTLOADER_EFI)
	ls -lh $(BOOTLOADER_EFI)
	@echo ""
	@echo "=== Kernel Info ==="
	file $(KERNEL_ELF)
	ls -lh $(KERNEL_ELF)
	readelf -h $(KERNEL_ELF)
	@echo ""
	@echo "=== Kernel Entry Point ==="
	readelf -h $(KERNEL_ELF) | grep "Entry point"

.PHONY: all clean esp disk-image test test-quick info

