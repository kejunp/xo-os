#include "test.h"
#include "kernel/mm/kalloc.h"
#include "kernel/mm/memlayout.h"

// Test basic allocation functionality
void test_kalloc_basic(void) {
    test_start("test_kalloc_basic");

    void *p1 = kalloc();
    assert(p1 != NULL);

    void *p2 = kalloc();
    assert(p2 != NULL);
    assert(p1 != p2);

    kfree(p1);
    kfree(p2);
}

// Test that freed memory can be reused
void test_kalloc_reuse(void) {
    test_start("test_kalloc_reuse");

    void *p1 = kalloc();
    assert(p1 != NULL);

    kfree(p1);

    void *p2 = kalloc();
    assert(p2 != NULL);

    // Should reuse the same page (LIFO free list)
    assert(p1 == p2);

    kfree(p2);
}

// Test multiple allocations and frees
void test_kalloc_multiple(void) {
    test_start("test_kalloc_multiple");

    void *pages[10];

    // Allocate multiple pages
    for (int i = 0; i < 10; i++) {
        pages[i] = kalloc();
        assert(pages[i] != NULL);

        // Check all pages are different
        for (int j = 0; j < i; j++) {
            assert(pages[i] != pages[j]);
        }
    }

    // Free all pages
    for (int i = 0; i < 10; i++) {
        kfree(pages[i]);
    }

    // Allocate again - should get pages back in LIFO order
    void *p1 = kalloc();
    assert(p1 == pages[9]); // Last freed should be first allocated

    kfree(p1);
}

// Test page alignment
void test_kalloc_alignment(void) {
    test_start("test_kalloc_alignment");

    void *p1 = kalloc();
    assert(p1 != NULL);

    // Check page alignment (should be 4KB aligned)
    uint64_t addr = (uint64_t)p1;
    assert((addr & (PGSIZE - 1)) == 0);

    kfree(p1);
}

// Test that different pages don't overlap
void test_kalloc_no_overlap(void) {
    test_start("test_kalloc_no_overlap");

    void *p1 = kalloc();
    void *p2 = kalloc();

    assert(p1 != NULL);
    assert(p2 != NULL);

    uint64_t addr1 = (uint64_t)p1;
    uint64_t addr2 = (uint64_t)p2;

    // Pages should be at least PGSIZE apart
    uint64_t diff = addr1 > addr2 ? addr1 - addr2 : addr2 - addr1;
    assert(diff >= PGSIZE);

    kfree(p1);
    kfree(p2);
}

// Run all kalloc tests
void run_kalloc_tests(void) {
    test_print("Running kalloc tests...\n");

    test_kalloc_basic();
    test_kalloc_reuse();
    test_kalloc_multiple();
    test_kalloc_alignment();
    test_kalloc_no_overlap();

    test_print("Kalloc tests completed.\n");
}

