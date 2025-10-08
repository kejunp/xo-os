#pragma once

#include "helpers/defs.h"

// Test framework globals
static int test_count = 0;
static int test_passed = 0;
static int test_failed = 0;

// Test result codes
#define TEST_SUCCESS 0
#define TEST_FAILURE 1

// Colors for output (if we implement serial output)
#define TEST_GREEN "\033[32m"
#define TEST_RED "\033[31m"
#define TEST_RESET "\033[0m"

// Simple assert macro
#define assert(condition) \
    do { \
        test_count++; \
        if (condition) { \
            test_passed++; \
        } else { \
            test_failed++; \
            test_fail(__FILE__, __LINE__, #condition); \
        } \
    } while(0)

// Test function declarations
void test_start(const char* test_name);
void test_fail(const char* file, int line, const char* condition);
void test_summary(void);
int get_test_result(void);

// Helper functions for test output
void test_print(const char* str);
void test_print_num(uint64_t num);
void test_print_hex(uint64_t num);

