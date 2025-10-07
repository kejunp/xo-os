#include <stdint.h>
#include <stddef.h>

// Boot info structure definition (must match bootloader's)
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

// Simple framebuffer operations
static void plot_pixel(struct xo_graphics_info *gfx, uint32_t x, uint32_t y, uint32_t color) {
  if (!gfx->framebuffer_address || x >= gfx->framebuffer_width || y >= gfx->framebuffer_height) {
    return;
  }

  uint32_t *framebuffer = (uint32_t*)gfx->framebuffer_address;
  uint32_t offset = y * (gfx->framebuffer_pitch / 4) + x;
  framebuffer[offset] = color;
}

static void clear_screen(struct xo_graphics_info *gfx, uint32_t color) {
  if (!gfx->framebuffer_address) {
    return;
  }

  uint32_t *framebuffer = (uint32_t*)gfx->framebuffer_address;
  uint32_t pixels_per_line = gfx->framebuffer_pitch / 4;

  for (uint32_t y = 0; y < gfx->framebuffer_height; y++) {
    for (uint32_t x = 0; x < gfx->framebuffer_width; x++) {
      framebuffer[y * pixels_per_line + x] = color;
    }
  }
}

static void draw_test_pattern(struct xo_graphics_info *gfx) {
  if (!gfx->framebuffer_address) {
    return;
  }

  // Clear to black
  clear_screen(gfx, 0x00000000);

  // Draw some colorful rectangles
  for (uint32_t y = 50; y < 150; y++) {
    for (uint32_t x = 50; x < 150; x++) {
      plot_pixel(gfx, x, y, 0x00FF0000); // Red
    }
  }

  for (uint32_t y = 200; y < 300; y++) {
    for (uint32_t x = 200; x < 300; x++) {
      plot_pixel(gfx, x, y, 0x0000FF00); // Green
    }
  }

  for (uint32_t y = 350; y < 450; y++) {
    for (uint32_t x = 350; x < 450; x++) {
      plot_pixel(gfx, x, y, 0x000000FF); // Blue
    }
  }
}

// Kernel entry point
// This function is called by the bootloader after loading the kernel
void kernel_main(struct xo_boot_info *boot_info) {
  // Verify boot info magic
  if (!boot_info || boot_info->magic != XO_BOOT_INFO_MAGIC) {
    // Invalid boot info - halt
    while (1) {
      __asm__ volatile ("hlt");
    }
  }

  // If we have a framebuffer, draw a test pattern
  if (boot_info->graphics.framebuffer_address) {
    draw_test_pattern(&boot_info->graphics);
  }

  // Simple infinite loop - kernel is running!
  // In a real kernel, this is where you'd:
  // - Set up memory management
  // - Initialize interrupt handlers
  // - Start the scheduler
  // - Launch init process
  // etc.
  
  while (1) {
    // Halt until next interrupt
    __asm__ volatile ("hlt");
  }
}