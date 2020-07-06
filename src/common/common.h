#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <error.h>

// Error reporting and system call assertion
#define SYSEXPECT(expr) do { if(!(expr)) { perror(__func__); exit(1); } } while(0)
#define error_exit(fmt, ...) do { fprintf(stderr, "%s error: " fmt, __func__, ##__VA_ARGS__); exit(1); } while(0);
#ifndef NDEBUG
#define dbg_printf(fmt, ...) do { fprintf(stderr, fmt, ##__VA_ARGS__); } while(0);
#else
#define dbg_printf(fmt, ...) do {} while(0);
#endif

// Bit masks
#define MASK64_LOW_1(num)  ((1UL << num) - 1)
#define MASK64_HIGH_1(num) (~(MASK64_LOW_1(64 - num)))
#define MASK64_LOW_0(num)  (~MASK64_LOW_1(num))
#define MASK64_HIGH_0(num) (~MASK64_HIGH_1(num))

// From 0 to 8, indexed by the number of bits
extern uint8_t mask8_low_1[9];  
extern uint8_t mask8_high_1[9];

#define MASK8_LOW_1(num)  (mask8_low_1[num])
#define MASK8_HIGH_1(num) (mask8_high_1[num])
#define MASK8_LOW_0(num)  (~MASK8_LOW_1(num))
#define MASK8_HIGH_0(num) (~MASK8_HIGH_1(num))

// Copies bit range in one byte to another byte
// starts are inclusive; ends are non-inclusive
inline static bitcpy8(uint8_t *to, uint8_t *from, int to_start, int from_start, int bits) {
  assert(bits >= 0 && bits <= 8);
  assert(to_start >= 0 && to_start + bits <= 8);
  assert(from_start >= 0 && from_start + bits <= 8);
  uint8_t from_mask_1 = MASK8_LOW_1(bits) << from_start;
  uint8_t to_mask_1 = MASK8_LOW_1(bits) << to_start;
  uint8_t from_bits = *from & from_mask_1;
  *to &= ~to_mask_1; // Clear bits in the destination

}

// Testing function print name and pass
#define TEST_BEGIN() do { printf("========== %s ==========\n", __func__); } while(0);
#define TEST_PASS() do { printf("Pass!\n"); } while(0);

#endif