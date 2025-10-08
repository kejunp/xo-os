#include "boot.h"
#include "elf.h"

// Protocol GUIDs (simplified representations)
static uint8_t gEfiGraphicsOutputProtocolGuid[16] = {
  0x9b, 0xc0, 0xb0, 0x3e, 0x74, 0x2f, 0x44, 0x9b,
  0x85, 0x85, 0x3e, 0xd4, 0x5e, 0x5e, 0x3c, 0xd9
};

static uint8_t gEfiSimpleFileSystemProtocolGuid[16] = {
  0x22, 0x5b, 0x4e, 0x96, 0x59, 0x64, 0xd2, 0x11,
  0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b
};

static uint8_t gEfiLoadedImageProtocolGuid[16] = {
  0xa1, 0x31, 0x1b, 0x5b, 0x62, 0x95, 0xd2, 0x11,
  0x8e, 0x3f, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b
};

static uint8_t gEfiFileInfoGuid[16] = {
  0x92, 0xec, 0x79, 0x09, 0x96, 0x8e, 0x11, 0xd1,
  0x9f, 0x4d, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b
};

// File system functions
static EFI_STATUS load_file_from_device(EFI_HANDLE device_handle, const CHAR16 *file_name, void **file_buffer, UINTN *file_size) {
  EFI_STATUS status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *file_system = NULL;
  EFI_FILE_PROTOCOL *root_dir = NULL;
  EFI_FILE_PROTOCOL *file = NULL;
  EFI_FILE_INFO *file_info = NULL;
  UINTN info_size;
  void *buffer = NULL;

  // Get file system protocol
  status = gBS->HandleProtocol(device_handle, gEfiSimpleFileSystemProtocolGuid, (void**)&file_system);
  if (status != EFI_SUCCESS) {
    return status;
  }

  // Open root directory
  status = file_system->OpenVolume(file_system, &root_dir);
  if (status != EFI_SUCCESS) {
    return status;
  }

  // Open the file
  status = root_dir->Open(root_dir, &file, (CHAR16*)file_name, EFI_FILE_MODE_READ, 0);
  if (status != EFI_SUCCESS) {
    root_dir->Close(root_dir);
    return status;
  }

  // Get file information to determine size
  info_size = sizeof(EFI_FILE_INFO) + 256 * sizeof(CHAR16);
  status = gBS->AllocatePool(EfiLoaderData, info_size, (void**)&file_info);
  if (status != EFI_SUCCESS) {
    file->Close(file);
    root_dir->Close(root_dir);
    return status;
  }

  status = file->GetInfo(file, gEfiFileInfoGuid, &info_size, file_info);
  if (status != EFI_SUCCESS) {
    gBS->FreePool(file_info);
    file->Close(file);
    root_dir->Close(root_dir);
    return status;
  }

  // Allocate buffer for file
  UINTN buffer_size = (UINTN)file_info->FileSize;
  status = gBS->AllocatePool(EfiLoaderData, buffer_size, &buffer);
  if (status != EFI_SUCCESS) {
    gBS->FreePool(file_info);
    file->Close(file);
    root_dir->Close(root_dir);
    return status;
  }

  // Read file
  status = file->Read(file, &buffer_size, buffer);
  if (status != EFI_SUCCESS) {
    gBS->FreePool(buffer);
    gBS->FreePool(file_info);
    file->Close(file);
    root_dir->Close(root_dir);
    return status;
  }

  // Cleanup
  gBS->FreePool(file_info);
  file->Close(file);
  root_dir->Close(root_dir);

  *file_buffer = buffer;
  *file_size = buffer_size;
  return EFI_SUCCESS;
}

static EFI_STATUS load_kernel_file(EFI_HANDLE image_handle, const CHAR16 *kernel_name, void **kernel_buffer, UINTN *kernel_size) {
  EFI_STATUS status;
  EFI_LOADED_IMAGE_PROTOCOL *loaded_image = NULL;

  // Get loaded image protocol to find our device
  status = gBS->HandleProtocol(image_handle, gEfiLoadedImageProtocolGuid, (void**)&loaded_image);
  if (status != EFI_SUCCESS) {
    return status;
  }

  // Try to load from the same device as the bootloader
  return load_file_from_device(loaded_image->DeviceHandle, kernel_name, kernel_buffer, kernel_size);
}

// ELF loading functions
static int validate_elf_header(const elf64_ehdr *header) {
  // Check ELF magic number
  if (header->e_ident[EI_MAG0] != ELFMAG0 ||
      header->e_ident[EI_MAG1] != ELFMAG1 ||
      header->e_ident[EI_MAG2] != ELFMAG2 ||
      header->e_ident[EI_MAG3] != ELFMAG3) {
    return 0;
  }

  // Check for 64-bit ELF
  if (header->e_ident[EI_CLASS] != ELFCLASS64) {
    return 0;
  }

  // Check for little-endian
  if (header->e_ident[EI_DATA] != ELFDATA2LSB) {
    return 0;
  }

  // Check for executable
  if (header->e_type != ET_EXEC) {
    return 0;
  }

  // Check for x86_64 architecture
  if (header->e_machine != EM_X86_64) {
    return 0;
  }

  return 1;
}

static EFI_STATUS load_elf_segments(const void *elf_data, UINTN elf_size, uint64_t *entry_point) {
  const elf64_ehdr *elf_header = (const elf64_ehdr*)elf_data;
  EFI_STATUS status;

  // Validate ELF header
  if (!validate_elf_header(elf_header)) {
    return EFI_INVALID_PARAMETER;
  }

  *entry_point = elf_header->e_entry;

  // Check bounds
  if (elf_header->e_phoff + (elf_header->e_phnum * elf_header->e_phentsize) > elf_size) {
    return EFI_INVALID_PARAMETER;
  }

  // Process program headers
  const elf64_phdr *program_headers = (const elf64_phdr*)((const uint8_t*)elf_data + elf_header->e_phoff);

  for (int i = 0; i < elf_header->e_phnum; i++) {
    const elf64_phdr *phdr = &program_headers[i];

    // Only load LOAD segments
    if (phdr->p_type != PT_LOAD) {
      continue;
    }

    // Validate segment bounds
    if (phdr->p_offset + phdr->p_filesz > elf_size) {
      return EFI_INVALID_PARAMETER;
    }

    // Allocate pages for the segment
    uint64_t pages = (phdr->p_memsz + 4095) / 4096; // Round up to page boundary
    uint64_t segment_address = phdr->p_paddr;

    status = gBS->AllocatePages(1, EfiLoaderData, pages, &segment_address); // 1 = AllocateAddress
    if (status != EFI_SUCCESS) {
      // Try allocating at any address and hope it works
      status = gBS->AllocatePages(0, EfiLoaderData, pages, &segment_address); // 0 = AllocateAnyPages
      if (status != EFI_SUCCESS) {
        return status;
      }
    }

    // Copy segment data from file
    const uint8_t *src = (const uint8_t*)elf_data + phdr->p_offset;
    uint8_t *dst = (uint8_t*)(void*)(uintptr_t)segment_address;

    // Copy file data
    for (UINTN j = 0; j < phdr->p_filesz; j++) {
      dst[j] = src[j];
    }

    // Zero remaining memory (BSS)
    for (UINTN j = phdr->p_filesz; j < phdr->p_memsz; j++) {
      dst[j] = 0;
    }
  }

  return EFI_SUCCESS;
}

// Utility functions
static void print_string(const CHAR16 *str) {
  if (gConOut && str) {
    gConOut->OutputString(gConOut, (CHAR16*)str);
  }
}

static void print_ascii(const char *str) {
  CHAR16 wide_str[256];
  int i = 0;

  while (str[i] && i < 255) {
    wide_str[i] = (CHAR16)str[i];
    i++;
  }
  wide_str[i] = 0;

  print_string(wide_str);
}

static void uint_to_string(uint64_t value, char *buffer, int base) {
  char digits[] = "0123456789ABCDEF";
  char temp[32];
  int i = 0;

  if (value == 0) {
    buffer[0] = '0';
    buffer[1] = 0;
    return;
  }

  while (value > 0) {
    temp[i++] = digits[value % base];
    value /= base;
  }

  int j = 0;
  while (i > 0) {
    buffer[j++] = temp[--i];
  }
  buffer[j] = 0;
}

static void print_number(uint64_t value) {
  char buffer[32];
  uint_to_string(value, buffer, 10);
  print_ascii(buffer);
}

static void print_hex(uint64_t value) {
  char buffer[32];
  print_ascii("0x");
  uint_to_string(value, buffer, 16);
  print_ascii(buffer);
}

static uint64_t get_timestamp(void) {
  EFI_TIME time;
  EFI_STATUS status;

  if (!gST || !gST->RuntimeServices) {
    return 0;
  }

  status = gST->RuntimeServices->GetTime(&time, NULL);
  if (status != EFI_SUCCESS) {
    return 0;
  }

  // Simple timestamp: seconds since year 2000
  uint64_t timestamp = 0;
  timestamp += (time.Year - 2000) * 365 * 24 * 3600;
  timestamp += time.Month * 30 * 24 * 3600;
  timestamp += time.Day * 24 * 3600;
  timestamp += time.Hour * 3600;
  timestamp += time.Minute * 60;
  timestamp += time.Second;

  return timestamp;
}

static EFI_STATUS wait_for_key(void) {
  EFI_INPUT_KEY key;
  EFI_STATUS status;

  if (!gST || !gST->ConIn) {
    return EFI_NOT_FOUND;
  }

  // Wait for key press
  do {
    status = gST->ConIn->ReadKeyStroke(gST->ConIn, &key);
  } while (status == EFI_NOT_READY);

  return status;
}

static uint32_t count_bits(uint32_t mask) {
  uint32_t count = 0;
  while (mask) {
    count += mask & 1;
    mask >>= 1;
  }
  return count;
}

static uint32_t find_bit_position(uint32_t mask) {
  uint32_t position = 0;
  if (mask == 0) return 0;

  while ((mask & 1) == 0) {
    mask >>= 1;
    position++;
  }
  return position;
}

static uint32_t calculate_checksum(const struct xo_boot_info *info) {
  const uint8_t *data = (const uint8_t*)info;
  uint32_t checksum = 0;
  UINTN size = sizeof(struct xo_boot_info) - sizeof(uint32_t); // Exclude checksum field

  for (UINTN i = 0; i < size; i++) {
    checksum += data[i];
  }

  return ~checksum + 1; // Two's complement
}

static xo_memory_type_t efi_to_xo_memory_type(uint32_t efi_type) {
  switch (efi_type) {
    case EfiConventionalMemory:
      return XO_MEMORY_AVAILABLE;
    case EfiReservedMemoryType:
      return XO_MEMORY_RESERVED;
    case EfiACPIReclaimMemory:
      return XO_MEMORY_ACPI_RECLAIMABLE;
    case EfiACPIMemoryNVS:
      return XO_MEMORY_ACPI_NVS;
    case EfiUnusableMemory:
      return XO_MEMORY_BAD;
    case EfiLoaderCode:
    case EfiBootServicesCode:
      return XO_MEMORY_BOOTLOADER_CODE;
    case EfiLoaderData:
    case EfiBootServicesData:
      return XO_MEMORY_BOOTLOADER_DATA;
    case EfiRuntimeServicesCode:
      return XO_MEMORY_RUNTIME_CODE;
    case EfiRuntimeServicesData:
      return XO_MEMORY_RUNTIME_DATA;
    case EfiPersistentMemory:
      return XO_MEMORY_PERSISTENT;
    default:
      return XO_MEMORY_RESERVED;
  }
}

// Memory map functions
static EFI_STATUS get_memory_map(struct xo_boot_info *boot_info) {
  EFI_STATUS status;
  UINTN map_size = 0;
  EFI_MEMORY_DESCRIPTOR *memory_map = NULL;
  UINTN map_key;
  UINTN descriptor_size;
  uint32_t descriptor_version;

  // Get memory map size
  status = gBS->GetMemoryMap(&map_size, memory_map, &map_key, &descriptor_size, &descriptor_version);
  if (status != EFI_BUFFER_TOO_SMALL) {
    return status;
  }

  // Allocate buffer with some extra space
  map_size += 2 * descriptor_size;
  status = gBS->AllocatePool(EfiLoaderData, map_size, (void**)&memory_map);
  if (status != EFI_SUCCESS) {
    return status;
  }

  // Get actual memory map
  status = gBS->GetMemoryMap(&map_size, memory_map, &map_key, &descriptor_size, &descriptor_version);
  if (status != EFI_SUCCESS) {
    gBS->FreePool(memory_map);
    return status;
  }

  // Convert to XO format
  UINTN num_descriptors = map_size / descriptor_size;
  boot_info->memory_map_entries = 0;
  boot_info->total_memory = 0;
  boot_info->available_memory = 0;

  for (UINTN i = 0; i < num_descriptors && boot_info->memory_map_entries < XO_MAX_MEMORY_ENTRIES; i++) {
    EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR*)((uint8_t*)memory_map + i * descriptor_size);

    struct xo_memory_entry *entry = &boot_info->memory_map[boot_info->memory_map_entries];
    entry->base_address = desc->PhysicalStart;
    entry->length = desc->NumberOfPages * 4096; // EFI pages are 4KB
    entry->type = efi_to_xo_memory_type(desc->Type);
    entry->attributes = (uint32_t)desc->Attribute;

    boot_info->total_memory += entry->length;
    if (entry->type == XO_MEMORY_AVAILABLE) {
      boot_info->available_memory += entry->length;
    }

    boot_info->memory_map_entries++;
  }

  gBS->FreePool(memory_map);
  return EFI_SUCCESS;
}

// Graphics initialization
static EFI_STATUS init_graphics(struct xo_boot_info *boot_info) {
  EFI_STATUS status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;

  status = gBS->LocateProtocol(gEfiGraphicsOutputProtocolGuid, NULL, (void**)&gop);
  if (status != EFI_SUCCESS || !gop) {
    // No graphics output protocol found
    boot_info->graphics.framebuffer_address = 0;
    return EFI_NOT_FOUND;
  }

  // Get current graphics mode
  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *mode = gop->Mode;
  if (!mode || !mode->Info) {
    return EFI_DEVICE_ERROR;
  }

  // Fill graphics info
  boot_info->graphics.framebuffer_address = mode->FrameBufferBase;
  boot_info->graphics.framebuffer_width = mode->Info->HorizontalResolution;
  boot_info->graphics.framebuffer_height = mode->Info->VerticalResolution;
  boot_info->graphics.framebuffer_pitch = mode->Info->PixelsPerScanLine * 4; // Assume 32bpp
  boot_info->graphics.framebuffer_bpp = 32;

  // Handle pixel format
  switch (mode->Info->PixelFormat) {
    case PixelRedGreenBlueReserved8BitPerColor:
      boot_info->graphics.red_mask_size = 8;
      boot_info->graphics.red_field_position = 0;
      boot_info->graphics.green_mask_size = 8;
      boot_info->graphics.green_field_position = 8;
      boot_info->graphics.blue_mask_size = 8;
      boot_info->graphics.blue_field_position = 16;
      boot_info->graphics.reserved_mask_size = 8;
      boot_info->graphics.reserved_field_position = 24;
      break;

    case PixelBlueGreenRedReserved8BitPerColor:
      boot_info->graphics.blue_mask_size = 8;
      boot_info->graphics.blue_field_position = 0;
      boot_info->graphics.green_mask_size = 8;
      boot_info->graphics.green_field_position = 8;
      boot_info->graphics.red_mask_size = 8;
      boot_info->graphics.red_field_position = 16;
      boot_info->graphics.reserved_mask_size = 8;
      boot_info->graphics.reserved_field_position = 24;
      break;

    case PixelBitMask:
      // Use provided bitmask information
      {
        EFI_PIXEL_BITMASK *mask = &mode->Info->PixelInformation;
        boot_info->graphics.red_mask_size = count_bits(mask->RedMask);
        boot_info->graphics.red_field_position = find_bit_position(mask->RedMask);
        boot_info->graphics.green_mask_size = count_bits(mask->GreenMask);
        boot_info->graphics.green_field_position = find_bit_position(mask->GreenMask);
        boot_info->graphics.blue_mask_size = count_bits(mask->BlueMask);
        boot_info->graphics.blue_field_position = find_bit_position(mask->BlueMask);
        boot_info->graphics.reserved_mask_size = count_bits(mask->ReservedMask);
        boot_info->graphics.reserved_field_position = find_bit_position(mask->ReservedMask);
      }
      break;

    default:
      return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

// Main entry point
EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
  EFI_STATUS status;
  struct xo_boot_info boot_info = {0};

  // Initialize globals
  gST = SystemTable;
  gBS = SystemTable->BootServices;
  gConOut = SystemTable->ConOut;

  // Clear screen and print banner
  if (gConOut) {
    gConOut->ClearScreen(gConOut);
    print_ascii("XO-OS UEFI Bootloader v1.0\r\n");
    print_ascii("==========================\r\n\r\n");
  }

  // Initialize boot info structure
  boot_info.magic = (uint64_t)XO_BOOT_INFO_MAGIC;
  boot_info.version = 1;
  boot_info.size = sizeof(struct xo_boot_info);
  boot_info.bootloader_timestamp = get_timestamp();

  // Get memory map
  print_ascii("Getting memory map...\r\n");
  status = get_memory_map(&boot_info);
  if (status != EFI_SUCCESS) {
    print_ascii("ERROR: Failed to get memory map\r\n");
    return status;
  }

  // Initialize graphics
  print_ascii("Initializing graphics...\r\n");
  status = init_graphics(&boot_info);
  if (status == EFI_SUCCESS) {
    print_ascii("Graphics initialized successfully\r\n");
  } else {
    print_ascii("WARNING: Graphics initialization failed\r\n");
  }

  // Fill UEFI info
  boot_info.uefi.efi_system_table = (uint64_t)(uintptr_t)(void*)SystemTable;
  boot_info.uefi.efi_version = SystemTable->Revision;
  boot_info.uefi.runtime_services_supported = (SystemTable->RuntimeServices != NULL) ? 1 : 0;
  boot_info.uefi.loader_signature = (uint64_t)0x584F424F4F544552ULL; // "XOBOOTER"

  // Calculate checksum
  boot_info.checksum = calculate_checksum(&boot_info);

  print_ascii("\r\nBoot information prepared:\r\n");
  print_ascii("- Memory entries: ");
  print_number(boot_info.memory_map_entries);
  print_ascii("\r\n- Total memory: ");
  print_number(boot_info.total_memory / (1024*1024));
  print_ascii(" MB\r\n- Available memory: ");
  print_number(boot_info.available_memory / (1024*1024));
  print_ascii(" MB\r\n- Graphics framebuffer: ");
  if (boot_info.graphics.framebuffer_address) {
    print_hex(boot_info.graphics.framebuffer_address);
    print_ascii(" (");
    print_number(boot_info.graphics.framebuffer_width);
    print_ascii("x");
    print_number(boot_info.graphics.framebuffer_height);
    print_ascii(")\r\n");
  } else {
    print_ascii("Not found\r\n");
  }
  print_ascii("- UEFI version: ");
  print_hex(boot_info.uefi.efi_version);
  print_ascii("\r\n- Boot timestamp: ");
  print_number(boot_info.bootloader_timestamp);
  print_ascii(" seconds since 2000\r\n");

  // Load kernel
  print_ascii("\r\nLoading kernel...\r\n");
  void *kernel_buffer = NULL;
  UINTN kernel_size = 0;
  uint64_t kernel_entry_point = 0;

  // Define kernel filename (UTF-16)
  CHAR16 kernel_filename[] = L"kernel.elf";

  status = load_kernel_file(ImageHandle, kernel_filename, &kernel_buffer, &kernel_size);
  if (status != EFI_SUCCESS) {
    print_ascii("ERROR: Failed to load kernel file: ");
    print_hex(status);
    print_ascii("\r\nPress any key to exit...\r\n");
    wait_for_key();
    return status;
  }

  print_ascii("Kernel loaded successfully (");
  print_number(kernel_size);
  print_ascii(" bytes)\r\n");

  // Parse and load ELF
  print_ascii("Parsing ELF and loading segments...\r\n");
  status = load_elf_segments(kernel_buffer, kernel_size, &kernel_entry_point);
  if (status != EFI_SUCCESS) {
    print_ascii("ERROR: Failed to load ELF segments: ");
    print_hex(status);
    print_ascii("\r\nPress any key to exit...\r\n");
    wait_for_key();
    gBS->FreePool(kernel_buffer);
    return status;
  }

  print_ascii("ELF segments loaded successfully\r\n");
  print_ascii("Kernel entry point: ");
  print_hex(kernel_entry_point);
  print_ascii("\r\n");

  // Fill kernel info in boot structure
  boot_info.kernel.kernel_entry_point = kernel_entry_point;
  boot_info.kernel.kernel_physical_address = kernel_entry_point; // For now, assume same
  boot_info.kernel.kernel_virtual_address = kernel_entry_point;  // For now, assume same
  boot_info.kernel.kernel_size = kernel_size;

  // Recalculate checksum
  boot_info.checksum = calculate_checksum(&boot_info);

  // Get final memory map for kernel
  print_ascii("Getting final memory map...\r\n");
  UINTN map_size = 0;
  EFI_MEMORY_DESCRIPTOR *memory_map = NULL;
  UINTN map_key;
  UINTN descriptor_size;
  uint32_t descriptor_version;

  // Get memory map size
  status = gBS->GetMemoryMap(&map_size, memory_map, &map_key, &descriptor_size, &descriptor_version);
  if (status != EFI_BUFFER_TOO_SMALL) {
    print_ascii("ERROR: Failed to get memory map size\r\n");
    gBS->FreePool(kernel_buffer);
    return status;
  }

  // Allocate buffer
  map_size += 2 * descriptor_size;
  status = gBS->AllocatePool(EfiLoaderData, map_size, (void**)&memory_map);
  if (status != EFI_SUCCESS) {
    print_ascii("ERROR: Failed to allocate memory map buffer\r\n");
    gBS->FreePool(kernel_buffer);
    return status;
  }

  // Get final memory map
  status = gBS->GetMemoryMap(&map_size, memory_map, &map_key, &descriptor_size, &descriptor_version);
  if (status != EFI_SUCCESS) {
    print_ascii("ERROR: Failed to get final memory map\r\n");
    gBS->FreePool(memory_map);
    gBS->FreePool(kernel_buffer);
    return status;
  }

  // Exit boot services
  print_ascii("Exiting UEFI boot services...\r\n");
  status = gBS->ExitBootServices(ImageHandle, map_key);
  if (status != EFI_SUCCESS) {
    print_ascii("ERROR: Failed to exit boot services: ");
    print_hex(status);
    print_ascii("\r\n");
    gBS->FreePool(memory_map);
    gBS->FreePool(kernel_buffer);
    return status;
  }

  // Boot services are no longer available - we're now in the kernel environment
  // Transfer control to kernel
  typedef void (*kernel_entry_func)(struct xo_boot_info *boot_info);
  kernel_entry_func kernel_main = (kernel_entry_func)(void*)(uintptr_t)kernel_entry_point;

  // Jump to kernel!
  kernel_main(&boot_info);

  // Should never reach here
  return EFI_SUCCESS;
}

