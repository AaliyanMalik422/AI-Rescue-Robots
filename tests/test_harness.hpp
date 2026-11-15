#pragma once

#include <iostream>

// Minimal test helper shared by all test files (no external test library).
// CHECK prints PASS/FAIL and counts failures; main returns the failure count.
static int test_failures = 0;
static int test_count = 0;

#define CHECK(condition)                                                        \
    do {                                                                        \
        ++test_count;                                                           \
        if (condition) {                                                        \
            std::cout << "[PASS] " << #condition << '\n';                       \
        } else {                                                                \
            std::cout << "[FAIL] " << #condition << "  (line " << __LINE__      \
                      << ")\n";                                                 \
            ++test_failures;                                                    \
        }                                                                       \
    } while (0)

// REQUIRE: like CHECK, but leaves the current test function when it fails, so
// later lines never dereference something that was not built.
#define REQUIRE(condition)                                                      \
    do {                                                                        \
        ++test_count;                                                           \
        if (condition) {                                                        \
            std::cout << "[PASS] " << #condition << '\n';                       \
        } else {                                                                \
            std::cout << "[FAIL] " << #condition << "  (line " << __LINE__      \
                      << ", test stopped)\n";                                   \
            ++test_failures;                                                    \
            return;                                                             \
        }                                                                       \
    } while (0)

static int finish_tests(const char* suite_name) {
    std::cout << suite_name << ": " << (test_count - test_failures) << "/"
              << test_count << " passed\n";
    return test_failures == 0 ? 0 : 1;
}
