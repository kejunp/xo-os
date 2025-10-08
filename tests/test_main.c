#include "test.h"
#include "kernel/mm/kalloc.h"
#include "boot/boot.h"

// Forward declaration of test functions
void run_kalloc_tests(void);

// Simple test output functions (we'll use serial or basic console)
void test_print(const char* str) {
    // For now, just a stub - could implement serial output later
    (void)str;
}

void test_print_num(uint64_t num) {
    (void)num;
}

void test_print_hex(uint64_t num) {
    (void)num;
}

// Test framework implementation
void test_start(const char* test_name) {
    (void)test_name;
    // Could print test name if we had output
}

void test_fail(const char* file, int line, const char* condition) {
    (void)file;
    (void)line;
    (void)condition;
    // Could print failure details if we had output
}

void test_summary(void) {
    // For now, just track the counts
    // Could print summary if we had output
}

int get_test_result(void) {
    return (test_failed == 0) ? TEST_SUCCESS : TEST_FAILURE;
}

// Create a minimal boot info for testing
struct xo_boot_info* create_test_boot_info(void) {
    static struct xo_boot_info boot_info = {0};

    boot_info.magic = XO_BOOT_INFO_MAGIC;
    boot_info.version = 1;
    boot_info.size = sizeof(struct xo_boot_info);

    // Create a simple memory map with one large available region
    boot_info.memory_map_entries = 1;
    boot_info.memory_map[0].base_address = 0x200000;  // 2MB
    boot_info.memory_map[0].length = 0x1E00000;       // 30MB
    boot_info.memory_map[0].type = XO_MEMORY_AVAILABLE;
    boot_info.memory_map[0].attributes = 0;

    boot_info.total_memory = boot_info.memory_map[0].length;
    boot_info.available_memory = boot_info.memory_map[0].length;

    return &boot_info;
}

// Test kernel main function
void kernel_main(struct xo_boot_info *boot_info) {
    // If no boot info provided, create a test one
    if (!boot_info) {
        boot_info = create_test_boot_info();
    }

    // Verify boot info magic
    if (boot_info->magic != XO_BOOT_INFO_MAGIC) {
        // Test setup failed
        while (1) {
            __asm__ volatile ("hlt");
        }
    }

    // Initialize memory management
    kinit(boot_info);

    // Run all tests
    run_kalloc_tests();

    // Print test summary
    test_summary();

    // Exit based on test results
    int result = get_test_result();

    if (result == TEST_SUCCESS) {
        // All tests passed - could signal success somehow
        // For now, just halt
        while (1) {
            __asm__ volatile ("hlt");
        }
    } else {
        // Tests failed - could signal failure somehow
        // For now, just halt
        while (1) {
            __asm__ volatile ("hlt");
        }
    }
}

// Entry point that matches linker expectations
void _start(void) {
    kernel_main(NULL);
}

