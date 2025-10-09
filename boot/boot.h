#pragma once

#include "helpers/defs.h"

// Compiler intrinsics and runtime support
static inline void __chkstk(void) {
  // Stack checking stub - not needed for our bootloader
}

static inline void *memset(void *s, int c, size_t n) {
  unsigned char *p = (unsigned char*)s;
  while (n--) {
    *p++ = (unsigned char)c;
  }
  return s;
}

// UEFI Basic Types
typedef struct _EFI_SYSTEM_TABLE EFI_SYSTEM_TABLE;
typedef struct _EFI_BOOT_SERVICES EFI_BOOT_SERVICES;
typedef struct _EFI_RUNTIME_SERVICES EFI_RUNTIME_SERVICES;
typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL EFI_GRAPHICS_OUTPUT_PROTOCOL;
typedef struct _EFI_LOADED_IMAGE_PROTOCOL EFI_LOADED_IMAGE_PROTOCOL;

typedef void* EFI_HANDLE;
typedef unsigned long EFI_STATUS;
typedef uint64_t UINTN;
typedef uint16_t CHAR16;

// UEFI Status Codes (using proper size for unsigned long)
#define EFI_SUCCESS               0x00000000UL
#define EFI_LOAD_ERROR            0x80000001UL
#define EFI_INVALID_PARAMETER     0x80000002UL
#define EFI_UNSUPPORTED           0x80000003UL
#define EFI_BAD_BUFFER_SIZE       0x80000004UL
#define EFI_BUFFER_TOO_SMALL      0x80000005UL
#define EFI_NOT_READY             0x80000006UL
#define EFI_DEVICE_ERROR          0x80000007UL
#define EFI_NOT_FOUND             0x8000000EUL
#define EFI_OUT_OF_RESOURCES      0x80000009UL

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
  char _pad2[88];
  EFI_STATUS (*HandleProtocol)(EFI_HANDLE, void*, void**);
  char _pad3[64];
  EFI_STATUS (*LocateProtocol)(void*, void*, void**);
  char _pad4[96];
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

#define XO_BOOT_INFO_MAGIC 0x584F424F4F54ULL  // "XOBOOT"
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

struct xo_memory_entry {
  uint64_t base_address;
  uint64_t length;
  xo_memory_type_t type;
  uint32_t attributes;
};

struct xo_graphics_info {
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
};

struct xo_hardware_info {
  uint64_t acpi_rsdp_address;
  uint64_t smbios_address;
  uint64_t device_tree_address;
  uint32_t device_tree_size;
  uint32_t cpu_count;
  uint64_t cpu_features;
};

struct xo_kernel_info {
  uint64_t kernel_physical_address;
  uint64_t kernel_virtual_address;
  uint64_t kernel_size;
  uint64_t kernel_entry_point;
  uint64_t initrd_address;
  uint64_t initrd_size;
  char cmdline[XO_MAX_CMDLINE_LENGTH];
};

struct xo_uefi_info {
  uint64_t efi_system_table;
  uint64_t efi_runtime_services;
  uint8_t runtime_services_supported;
  uint32_t efi_version;
  uint64_t loader_signature;
};

struct xo_boot_info {
  uint64_t magic;
  uint32_t version;
  uint32_t size;

  struct xo_memory_entry memory_map[XO_MAX_MEMORY_ENTRIES];
  uint32_t memory_map_entries;
  uint64_t total_memory;
  uint64_t available_memory;

  struct xo_graphics_info graphics;
  struct xo_hardware_info hardware;
  struct xo_kernel_info kernel;
  struct xo_uefi_info uefi;

  uint64_t bootloader_timestamp;
  uint32_t checksum;
};

// Global variables
static EFI_SYSTEM_TABLE *gST = NULL;
static EFI_BOOT_SERVICES *gBS = NULL;
static EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *gConOut = NULL;

// EFI File System Structures
typedef struct _EFI_FILE_PROTOCOL EFI_FILE_PROTOCOL;
typedef struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

typedef struct {
  uint64_t Size;
  uint64_t FileSize;
  uint64_t PhysicalSize;
  uint64_t CreateTime[2];
  uint64_t LastAccessTime[2];
  uint64_t ModificationTime[2];
  uint64_t Attribute;
  CHAR16 FileName[1];
} EFI_FILE_INFO;

#define EFI_FILE_MODE_READ   0x0000000000000001ULL
#define EFI_FILE_MODE_WRITE  0x0000000000000002ULL
#define EFI_FILE_MODE_CREATE 0x8000000000000000ULL

struct _EFI_FILE_PROTOCOL {
  uint64_t Revision;
  EFI_STATUS (*Open)(EFI_FILE_PROTOCOL *This, EFI_FILE_PROTOCOL **NewHandle, CHAR16 *FileName, uint64_t OpenMode, uint64_t Attributes);
  EFI_STATUS (*Close)(EFI_FILE_PROTOCOL *This);
  EFI_STATUS (*Delete)(EFI_FILE_PROTOCOL *This);
  EFI_STATUS (*Read)(EFI_FILE_PROTOCOL *This, UINTN *BufferSize, void *Buffer);
  EFI_STATUS (*Write)(EFI_FILE_PROTOCOL *This, UINTN *BufferSize, void *Buffer);
  EFI_STATUS (*GetPosition)(EFI_FILE_PROTOCOL *This, uint64_t *Position);
  EFI_STATUS (*SetPosition)(EFI_FILE_PROTOCOL *This, uint64_t Position);
  EFI_STATUS (*GetInfo)(EFI_FILE_PROTOCOL *This, void *InformationType, UINTN *BufferSize, void *Buffer);
  EFI_STATUS (*SetInfo)(EFI_FILE_PROTOCOL *This, void *InformationType, UINTN BufferSize, void *Buffer);
  EFI_STATUS (*Flush)(EFI_FILE_PROTOCOL *This);
};

struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
  uint64_t Revision;
  EFI_STATUS (*OpenVolume)(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *This, EFI_FILE_PROTOCOL **Root);
};

struct _EFI_LOADED_IMAGE_PROTOCOL {
  uint32_t Revision;
  EFI_HANDLE ParentHandle;
  EFI_SYSTEM_TABLE *SystemTable;
  EFI_HANDLE DeviceHandle;
  void *FilePath;
  void *Reserved;
  uint32_t LoadOptionsSize;
  void *LoadOptions;
  void *ImageBase;
  uint64_t ImageSize;
  EFI_MEMORY_TYPE ImageCodeType;
  EFI_MEMORY_TYPE ImageDataType;
  void *Unload;
};

