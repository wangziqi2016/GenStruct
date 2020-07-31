#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <error.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


// Error reporting and system call assertion
#define SYSEXPECT(expr) do { if(!(expr)) { perror(__func__); exit(1); } } while(0)
#define error_exit(fmt, ...) do { fprintf(stderr, "%s error: " fmt, __func__, ##__VA_ARGS__); exit(1); } while(0);
#ifndef NDEBUG
#define dbg_printf(fmt, ...) do { fprintf(stderr, fmt, ##__VA_ARGS__); } while(0);
#else
#define dbg_printf(fmt, ...) do {} while(0);
#endif

// Bit masks
extern uint64_t mask64_1[65];

#define MASK64_1(index)    (mask64_1[index])    // Single bit 1 mask; Index of bit given in arg
#define MASK64_LOW_1(num)  (MASK64_1(num) - 1)  // All-one mask in lower bits; Number of 1 given in arg
#define MASK64_HIGH_1(num) (~(MASK64_LOW_1(64 - num)))
#define MASK64_LOW_0(num)  (~MASK64_LOW_1(num))
#define MASK64_HIGH_0(num) (~MASK64_HIGH_1(num))
#define MASK64_RANGE_1(start, bits) (MASK64_LOW_1(bits) << start)  // [start, start + bits) are 1, otherwise 0
#define MASK64_RANGE_0(start, bits) (~MASK64_RANGE_1(start, bits)) // [start, start + bits) are 0, otherwise 1

#define MASK32_1(index)    ((uint32_t)mask64_1[index])  
#define MASK32_LOW_1(num)  (MASK32_1(num) - 1)           
#define MASK32_HIGH_1(num) (~(MASK32_LOW_1(32 - num)))
#define MASK32_LOW_0(num)  (~MASK32_LOW_1(num))
#define MASK32_HIGH_0(num) (~MASK32_HIGH_1(num))
#define MASK32_RANGE_1(start, bits) (MASK32_LOW_1(bits) << start) 
#define MASK32_RANGE_0(start, bits) (~MASK32_RANGE_1(start, bits))

#define MASK8_1(index)    ((uint8_t)mask64_1[index])   
#define MASK8_LOW_1(num)  (MASK8_1(num) - 1)           
#define MASK8_HIGH_1(num) (~(MASK8_LOW_1(8 - num)))
#define MASK8_LOW_0(num)  (~MASK8_LOW_1(num))
#define MASK8_HIGH_0(num) (~MASK8_HIGH_1(num))
#define MASK8_RANGE_1(start, bits) (MASK8_LOW_1(bits) << start) // [start, start + bits) are 1, otherwise 0
#define MASK8_RANGE_0(start, bits) (~MASK8_RANGE_1(start, bits))

inline static int bit64_popcount(uint64_t value) { return __builtin_popcountl(value); }
inline static int bit32_popcount(uint32_t value) { return __builtin_popcount(value); }
inline static int bit8_popcount(uint8_t value) { return __builtin_popcount((uint32_t)value); }

inline static int islog2_u64(uint64_t value) { return bit64_popcount(value) == 1; }
inline static int islog2_64(int64_t value) { return bit64_popcount((uint64_t)value) == 1; }
inline static int islog2_u32(uint32_t value) { return bit32_popcount(value) == 1; }
inline static int islog2_32(int32_t value) { return bit32_popcount((uint32_t)value) == 1; }
inline static int islog2_u8(uint8_t value) { return bit8_popcount(value) == 1; }
inline static int islog2_8(int8_t value) { return bit8_popcount((uint8_t)value) == 1; }

// Round up to the next log2; Not changed if already log2; Returns 0 if overflows
inline static uint64_t nextlog2_u64(uint64_t value) { return value ? MASK64_1(64 - __builtin_clzl(value)) : 0UL; }
int nextlog2_32(int32_t value);

// Extracts the bit 
inline static int bit64_test(uint64_t value, int index) { return (value >> index) & 0x1UL; }
inline static int bit32_test(uint32_t value, int index) { return (value >> index) & 0x1; }
inline static int bit8_test(uint8_t value, int index) { return (value >> index) & 0x1; }

inline static uint64_t bit64_range_set(uint64_t value, int start, int bits) { return value | MASK64_RANGE_1(start, bits); }
inline static uint8_t bit8_range_set(uint8_t value, int start, int bits) { return value | MASK8_RANGE_1(start, bits); }

inline static uint64_t bit64_range_clear(uint64_t value, int start, int bits) { return value & MASK64_RANGE_0(start, bits); }
inline static uint8_t bit8_range_clear(uint8_t value, int start, int bits) { return value & MASK8_RANGE_0(start, bits); }

inline static uint8_t rand_u8() { return (uint8_t)rand(); }
// Note that rand() may return a number less than 64 bits, but most likely it will be at least 15 bits
inline static uint64_t rand_u64() { 
  return (uint64_t)rand() ^ ((uint64_t)rand() << 15) ^ ((uint64_t)rand() << 30) ^ \
         ((uint64_t)rand() << 45) ^ ((uint64_t)rand() << 60); 
}

uint64_t bit_gen(const char *s, int bits); // Generate 64 bit string using "1" and "0", MSB first
inline static uint8_t bit8_gen(const char *s) { return (uint8_t)bit_gen(s, 8); }
inline static uint64_t bit64_gen(const char *s) { return bit_gen(s, 64); }

// Copies bit range in one byte to another byte
// starts are inclusive; ends are non-inclusive
inline static void bitcpy8(uint8_t *to, uint8_t *from, int to_start, int from_start, int bits) {
  assert(bits >= 0 && bits <= 8);
  assert(to_start >= 0 && to_start + bits <= 8);
  assert(from_start >= 0 && from_start + bits <= 8);
  uint8_t from_mask_1 = MASK8_LOW_1(bits) << from_start;
  uint8_t to_mask_1 = MASK8_LOW_1(bits) << to_start;
  uint8_t from_bits = *from & from_mask_1;
  uint8_t to_bits = *to & ~to_mask_1;
  if(from_start > to_start) from_bits >>= (from_start - to_start);
  else if(from_start < to_start) from_bits <<= (to_start - from_start);
  to_bits |= from_bits;
  *to = to_bits;
  return;
}

#define BITSPRINT_BE 0 // Big-endian print, i.e. high bits are printed first
#define BITSPRINT_LE 1 // Little-endian print, i.e. low bits are printed first

void bitsprint8(char *buf, uint8_t value, int dir); // Print 8 bit value to a buffer in binary form; dir is direction
inline static void bitsprint8_be(char *buf, uint8_t value) { bitsprint8(buf, value, BITSPRINT_BE); }
inline static void bitsprint8_le(char *buf, uint8_t value) { bitsprint8(buf, value, BITSPRINT_LE); }

// String macros
#define STREQ(a, b) (strcmp(a, b) == 0)

// File macros
int fp_rem(FILE *fp); // Remaining number of bytes in the file
int file_size(const char *name); // Get size using a name

// Testing function print name and pass
#define TEST_BEGIN() do { printf("========== %s ==========\n", __func__); } while(0);
#define TEST_PASS() do { printf("Pass!\n"); } while(0);

#endif