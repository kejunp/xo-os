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
  efi_reserved_memory_type,
  efi_loader_code,
  efi_loader_data,
  efi_boot_services_code,
  efi_boot_services_data,
  efi_runtime_services_code,
  efi_runtime_services_data,
  efi_conventional_memory,
  efi_unusable_memory,
  efi_acpi_reclaim_memory,
  efi_acpi_memory_nvs,
  efi_memory_mapped_io,
  efi_memory_mapped_io_port_space,
  efi_pal_code,
  efi_persistent_memory,
  efi_max_memory_type
} EFI_MEMORY_TYPE;

typedef struct {
  uint32_t type;
  uint64_t physical_start;
  uint64_t virtual_start;
  uint64_t number_of_pages;
  uint64_t attribute;
} EFI_MEMORY_DESCRIPTOR;

// UEFI Graphics Types
typedef struct {
  uint32_t red_mask;
  uint32_t green_mask;
  uint32_t blue_mask;
  uint32_t reserved_mask;
} EFI_PIXEL_BITMASK;

typedef enum {
  pixel_red_green_blue_reserved_8_bit_per_color,
  pixel_blue_green_red_reserved_8_bit_per_color,
  pixel_bit_mask,
  pixel_blt_only,
  pixel_format_max
} EFI_GRAPHICS_PIXEL_FORMAT;

typedef struct {
  uint32_t Version;
  uint32_t horizontal_resolution;
  uint32_t vertical_resolution;
  EFI_GRAPHICS_PIXEL_FORMAT pixel_format;
  EFI_PIXEL_BITMASK pixel_information;
  uint32_t pixels_per_scan_line;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
  uint32_t max_mode;
  uint32_t mode;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
  UINTN size_of_info;
  uint64_t frame_buffer_base;
  UINTN frame_buffer_size;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

// UEFI Time Structure
typedef struct {
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t pad1;
  uint32_t nanosecond;
  int16_t time_zone;
  uint8_t daylight;
  uint8_t pad2;
} EFI_TIME;

typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

typedef struct {
  uint16_t scan_code;
  CHAR16 unicode_char;
} EFI_INPUT_KEY;

// UEFI Protocol Structures
struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
  EFI_STATUS (*reset)(EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This, int extended_verification);
  EFI_STATUS (*read_key_stroke)(EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This, EFI_INPUT_KEY *Key);
  void* wait_for_key;
};

struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
  EFI_STATUS (*reset)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, int extended_verification);
  EFI_STATUS (*output_string)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, CHAR16 *String);
  void* test_string;
  void* query_mode;
  void* set_mode;
  void* set_attribute;
  EFI_STATUS (*clear_screen)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This);
  void* set_cursor_position;
  void* enable_cursor;
  void* Mode;
};

struct _EFI_GRAPHICS_OUTPUT_PROTOCOL {
  void* query_mode;
  void* set_mode;
  void* blt;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *mode;
};

struct _EFI_BOOT_SERVICES {
  char _pad[24];
  void* raise_tpl;
  void* restore_tpl;
  EFI_STATUS (*allocate_pages)(int, EFI_MEMORY_TYPE, UINTN, uint64_t*);
  EFI_STATUS (*free_pages)(uint64_t, UINTN);
  EFI_STATUS (*get_memory_map)(UINTN*, EFI_MEMORY_DESCRIPTOR*, UINTN*, UINTN*, uint32_t*);
  EFI_STATUS (*allocate_pool)(EFI_MEMORY_TYPE, UINTN, void**);
  EFI_STATUS (*free_pool)(void*);
  char _pad2[88];
  EFI_STATUS (*handle_protocol)(EFI_HANDLE, void*, void**);
  char _pad3[64];
  EFI_STATUS (*locate_protocol)(void*, void*, void**);
  char _pad4[96];
  EFI_STATUS (*exit_boot_services)(EFI_HANDLE, UINTN);
};

struct _EFI_RUNTIME_SERVICES {
  char _pad[24];
  EFI_STATUS (*get_time)(EFI_TIME *Time, void *Capabilities);
  EFI_STATUS (*set_time)(EFI_TIME *Time);
  // Other runtime services...
  char _pad2[200];
};

struct _EFI_SYSTEM_TABLE {
  uint64_t signature;
  uint32_t revision;
  uint32_t header_size;
  uint32_t crc32;
  uint32_t reserved;
  uint16_t firmware_vendor;
  uint32_t firmware_revision;
  EFI_HANDLE console_in_handle;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL *con_in;
  EFI_HANDLE console_out_handle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *con_out;
  EFI_HANDLE standard_error_handle;
  void* std_err;
  EFI_RUNTIME_SERVICES *runtime_services;
  EFI_BOOT_SERVICES *boot_services;
  UINTN number_of_table_entries;
  void* configuration_table;
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
static EFI_SYSTEM_TABLE *g_st = NULL;
static EFI_BOOT_SERVICES *g_bs = NULL;
static EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *g_con_out = NULL;

// EFI File System Structures
typedef struct _EFI_FILE_PROTOCOL EFI_FILE_PROTOCOL;
typedef struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

typedef struct {
  uint64_t Size;
  uint64_t file_size;
  uint64_t physical_size;
  uint64_t create_time[2];
  uint64_t last_access_time[2];
  uint64_t modification_time[2];
  uint64_t attribute;
  CHAR16 file_name[1];
} EFI_FILE_INFO;

#define EFI_FILE_MODE_READ   0x0000000000000001ULL
#define EFI_FILE_MODE_WRITE  0x0000000000000002ULL
#define EFI_FILE_MODE_CREATE 0x8000000000000000ULL

struct _EFI_FILE_PROTOCOL {
  uint64_t Revision;
  EFI_STATUS (*open)(EFI_FILE_PROTOCOL *This, EFI_FILE_PROTOCOL **new_handle, CHAR16 *file_name, uint64_t open_mode, uint64_t attributes);
  EFI_STATUS (*close)(EFI_FILE_PROTOCOL *This);
  EFI_STATUS (*delete)(EFI_FILE_PROTOCOL *This);
  EFI_STATUS (*read)(EFI_FILE_PROTOCOL *This, UINTN *buffer_size, void *buffer);
  EFI_STATUS (*write)(EFI_FILE_PROTOCOL *This, UINTN *buffer_size, void *buffer);
  EFI_STATUS (*get_position)(EFI_FILE_PROTOCOL *This, uint64_t *position);
  EFI_STATUS (*set_position)(EFI_FILE_PROTOCOL *This, uint64_t position);
  EFI_STATUS (*get_info)(EFI_FILE_PROTOCOL *This, void *information_type, UINTN *buffer_size, void *buffer);
  EFI_STATUS (*set_info)(EFI_FILE_PROTOCOL *This, void *information_type, UINTN buffer_size, void *buffer);
  EFI_STATUS (*flush)(EFI_FILE_PROTOCOL *This);
};

struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
  uint64_t Revision;
  EFI_STATUS (*open_volume)(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *This, EFI_FILE_PROTOCOL **root);
};

struct _EFI_LOADED_IMAGE_PROTOCOL {
  uint32_t Revision;
  EFI_HANDLE parent_handle;
  EFI_SYSTEM_TABLE *system_table;
  EFI_HANDLE device_handle;
  void *file_path;
  void *reserved;
  uint32_t load_options_size;
  void *load_options;
  void *image_base;
  uint64_t image_size;
  EFI_MEMORY_TYPE image_code_type;
  EFI_MEMORY_TYPE image_data_type;
  void *unload;
};

