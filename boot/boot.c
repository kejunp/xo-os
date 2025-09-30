#include <stdint.h>
#include <stddef.h>

// UEFI Basic Types
typedef struct _EFI_SYSTEM_TABLE EFI_SYSTEM_TABLE;
typedef struct _EFI_BOOT_SERVICES EFI_BOOT_SERVICES;
typedef struct _EFI_RUNTIME_SERVICES EFI_RUNTIME_SERVICES;
typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL EFI_GRAPHICS_OUTPUT_PROTOCOL;
typedef struct _EFI_LOADED_IMAGE_PROTOCOL EFI_LOADED_IMAGE_PROTOCOL;

typedef void* EFI_HANDLE;
typedef uint64_t EFI_STATUS;
typedef uint64_t UINTN;
typedef uint16_t CHAR16;

// UEFI Status Codes
#define EFI_SUCCESS               0x0000000000000000ULL
#define EFI_LOAD_ERROR            0x8000000000000001ULL
#define EFI_INVALID_PARAMETER     0x8000000000000002ULL
#define EFI_UNSUPPORTED           0x8000000000000003ULL
#define EFI_BAD_BUFFER_SIZE       0x8000000000000004ULL
#define EFI_BUFFER_TOO_SMALL      0x8000000000000005ULL
#define EFI_NOT_READY             0x8000000000000006ULL
#define EFI_DEVICE_ERROR          0x8000000000000007ULL
#define EFI_NOT_FOUND             0x800000000000000EULL
#define EFI_OUT_OF_RESOURCES      0x8000000000000009ULL

// UEFI Memory Types
typedef enum {
  EfiReservedMemoryType,
  EfiLoaderCode,
  EfiLoaderData,
  EfiBootServicesCode,
  EfiBootServicesData,
  EfiRuntimeServicesCode,
  EfiRuntimeServicesData,
  EfiConventionalMemory,
  EfiUnusableMemory,
  EfiACPIReclaimMemory,
  EfiACPIMemoryNVS,
  EfiMemoryMappedIO,
  EfiMemoryMappedIOPortSpace,
  EfiPalCode,
  EfiPersistentMemory,
  EfiMaxMemoryType
} EFI_MEMORY_TYPE;

typedef struct {
  uint32_t Type;
  uint64_t PhysicalStart;
  uint64_t VirtualStart;
  uint64_t NumberOfPages;
  uint64_t Attribute;
} EFI_MEMORY_DESCRIPTOR;

// UEFI Graphics Types
typedef struct {
  uint32_t RedMask;
  uint32_t GreenMask;
  uint32_t BlueMask;
  uint32_t ReservedMask;
} EFI_PIXEL_BITMASK;

typedef enum {
  PixelRedGreenBlueReserved8BitPerColor,
  PixelBlueGreenRedReserved8BitPerColor,
  PixelBitMask,
  PixelBltOnly,
  PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

typedef struct {
  uint32_t Version;
  uint32_t HorizontalResolution;
  uint32_t VerticalResolution;
  EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
  EFI_PIXEL_BITMASK PixelInformation;
  uint32_t PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
  uint32_t MaxMode;
  uint32_t Mode;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
  UINTN SizeOfInfo;
  uint64_t FrameBufferBase;
  UINTN FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

// UEFI Time Structure
typedef struct {
  uint16_t Year;
  uint8_t Month;
  uint8_t Day;
  uint8_t Hour;
  uint8_t Minute;
  uint8_t Second;
  uint8_t Pad1;
  uint32_t Nanosecond;
  int16_t TimeZone;
  uint8_t Daylight;
  uint8_t Pad2;
} EFI_TIME;

typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

typedef struct {
  uint16_t ScanCode;
  CHAR16 UnicodeChar;
} EFI_INPUT_KEY;

// UEFI Protocol Structures
struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
  EFI_STATUS (*Reset)(EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This, int ExtendedVerification);
  EFI_STATUS (*ReadKeyStroke)(EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This, EFI_INPUT_KEY *Key);
  void* WaitForKey;
};

struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
  EFI_STATUS (*Reset)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, int ExtendedVerification);
  EFI_STATUS (*OutputString)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, CHAR16 *String);
  void* TestString;
  void* QueryMode;
  void* SetMode;
  void* SetAttribute;
  EFI_STATUS (*ClearScreen)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This);
  void* SetCursorPosition;
  void* EnableCursor;
  void* Mode;
};

struct _EFI_GRAPHICS_OUTPUT_PROTOCOL {
  void* QueryMode;
  void* SetMode;
  void* Blt;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode;
};

struct _EFI_BOOT_SERVICES {
  char _pad[24];
  void* RaiseTPL;
  void* RestoreTPL;
  EFI_STATUS (*AllocatePages)(int, EFI_MEMORY_TYPE, UINTN, uint64_t*);
  EFI_STATUS (*FreePages)(uint64_t, UINTN);
  EFI_STATUS (*GetMemoryMap)(UINTN*, EFI_MEMORY_DESCRIPTOR*, UINTN*, UINTN*, uint32_t*);
  EFI_STATUS (*AllocatePool)(EFI_MEMORY_TYPE, UINTN, void**);
  EFI_STATUS (*FreePool)(void*);
  char _pad2[152];
  EFI_STATUS (*LocateProtocol)(void*, void*, void**);
  char _pad3[96];
  EFI_STATUS (*ExitBootServices)(EFI_HANDLE, UINTN);
};

struct _EFI_RUNTIME_SERVICES {
  char _pad[24];
  EFI_STATUS (*GetTime)(EFI_TIME *Time, void *Capabilities);
  EFI_STATUS (*SetTime)(EFI_TIME *Time);
  // Other runtime services...
  char _pad2[200];
};

struct _EFI_SYSTEM_TABLE {
  uint64_t Signature;
  uint32_t Revision;
  uint32_t HeaderSize;
  uint32_t CRC32;
  uint32_t Reserved;
  uint16_t FirmwareVendor;
  uint32_t FirmwareRevision;
  EFI_HANDLE ConsoleInHandle;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn;
  EFI_HANDLE ConsoleOutHandle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
  EFI_HANDLE StandardErrorHandle;
  void* StdErr;
  EFI_RUNTIME_SERVICES *RuntimeServices;
  EFI_BOOT_SERVICES *BootServices;
  UINTN NumberOfTableEntries;
  void* ConfigurationTable;
};

#define XO_BOOT_INFO_MAGIC 0x584F424F4F54  // "XOBOOT"
#define XO_MAX_MEMORY_ENTRIES 256
#define XO_MAX_CMDLINE_LENGTH 1024

typedef enum {
  XO_MEMORY_AVAILABLE = 1,
  XO_MEMORY_RESERVED = 2,
  XO_MEMORY_ACPI_RECLAIMABLE = 3,
  XO_MEMORY_ACPI_NVS = 4,
  XO_MEMORY_BAD = 5,
  XO_MEMORY_BOOTLOADER_CODE = 6,
  XO_MEMORY_BOOTLOADER_DATA = 7,
  XO_MEMORY_RUNTIME_CODE = 8,
  XO_MEMORY_RUNTIME_DATA = 9,
  XO_MEMORY_CONVENTIONAL = 10,
  XO_MEMORY_UNUSABLE = 11,
  XO_MEMORY_PERSISTENT = 12
} xo_memory_type_t;

typedef struct {
  uint64_t base_address;
  uint64_t length;
  xo_memory_type_t type;
  uint32_t attributes;
} xo_memory_entry_t;

typedef struct {
  uint64_t framebuffer_address;
  uint32_t framebuffer_width;
  uint32_t framebuffer_height;
  uint32_t framebuffer_pitch;
  uint32_t framebuffer_bpp;
  uint32_t red_mask_size;
  uint32_t red_field_position;
  uint32_t green_mask_size;
  uint32_t green_field_position;
  uint32_t blue_mask_size;
  uint32_t blue_field_position;
  uint32_t reserved_mask_size;
  uint32_t reserved_field_position;
} xo_graphics_info_t;

typedef struct {
  uint64_t acpi_rsdp_address;
  uint64_t smbios_address;
  uint64_t device_tree_address;
  uint32_t device_tree_size;
  uint32_t cpu_count;
  uint64_t cpu_features;
} xo_hardware_info_t;

typedef struct {
  uint64_t kernel_physical_address;
  uint64_t kernel_virtual_address;
  uint64_t kernel_size;
  uint64_t kernel_entry_point;
  uint64_t initrd_address;
  uint64_t initrd_size;
  char cmdline[XO_MAX_CMDLINE_LENGTH];
} xo_kernel_info_t;

typedef struct {
  uint64_t efi_system_table;
  uint64_t efi_runtime_services;
  uint8_t runtime_services_supported;
  uint32_t efi_version;
  uint64_t loader_signature;
} xo_uefi_info_t;

typedef struct {
  uint64_t magic;
  uint32_t version;
  uint32_t size;

  xo_memory_entry_t memory_map[XO_MAX_MEMORY_ENTRIES];
  uint32_t memory_map_entries;
  uint64_t total_memory;
  uint64_t available_memory;

  xo_graphics_info_t graphics;
  xo_hardware_info_t hardware;
  xo_kernel_info_t kernel;
  xo_uefi_info_t uefi;

  uint64_t bootloader_timestamp;
  uint32_t checksum;
} xo_boot_info_t;

// Global variables
static EFI_SYSTEM_TABLE *gST = NULL;
static EFI_BOOT_SERVICES *gBS = NULL;
static EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *gConOut = NULL;

// Protocol GUIDs (simplified representations)
static uint8_t gEfiGraphicsOutputProtocolGuid[16] = {
  0x9b, 0xc0, 0xb0, 0x3e, 0x74, 0x2f, 0x44, 0x9b,
  0x85, 0x85, 0x3e, 0xd4, 0x5e, 0x5e, 0x3c, 0xd9
};

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

static uint32_t calculate_checksum(const xo_boot_info_t *info) {
  const uint8_t *data = (const uint8_t*)info;
  uint32_t checksum = 0;
  UINTN size = sizeof(xo_boot_info_t) - sizeof(uint32_t); // Exclude checksum field

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
static EFI_STATUS get_memory_map(xo_boot_info_t *boot_info) {
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

    xo_memory_entry_t *entry = &boot_info->memory_map[boot_info->memory_map_entries];
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
static EFI_STATUS init_graphics(xo_boot_info_t *boot_info) {
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
  xo_boot_info_t boot_info = {0};

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
  boot_info.magic = XO_BOOT_INFO_MAGIC;
  boot_info.version = 1;
  boot_info.size = sizeof(xo_boot_info_t);
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
  boot_info.uefi.efi_system_table = (uint64_t)SystemTable;
  boot_info.uefi.efi_version = SystemTable->Revision;
  boot_info.uefi.runtime_services_supported = (SystemTable->RuntimeServices != NULL) ? 1 : 0;
  boot_info.uefi.loader_signature = 0x584F424F4F544552ULL; // "XOBOOTER"

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

  print_ascii("\r\nBootloader ready. In a real OS, kernel would be loaded here.\r\n");
  print_ascii("Press any key to exit...\r\n");

  // Wait for user input
  wait_for_key();

  return EFI_SUCCESS;
}

