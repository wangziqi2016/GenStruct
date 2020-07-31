
#include <time.h>
#include "common.h"

void test_sysexpect() {
  TEST_BEGIN();
  void *p = malloc(1234);
  SYSEXPECT(p != NULL);
  TEST_PASS();
  return;
}

void test_dbg_printf() {
  TEST_BEGIN();
  // Make sure we can always see a prompt, with the status of the NDEBUG flag
#ifdef NDEBUG
  // This should not trigger an assert
  assert(0);
#undef NDEBUG
  printf("Debug print - NDEBUG enabled\n");
#define NDEBUG
#else 
  dbg_printf("Debug print - NDEBUG disabled\n");
#endif
  TEST_PASS();
  return;
}

void test_bitsprintf8() {
  TEST_BEGIN();
  char buf1[9], buf2[9];
  bitsprint8_be(buf1, 0x55);
  bitsprint8_be(buf2, 0xaa);
  printf("0x55 = \"%s\"; 0xaa = \"%s\"\n", buf1, buf2);
  assert(STREQ(buf1, "01010101"));
  assert(STREQ(buf2, "10101010"));
  bitsprint8_be(buf1, 0xa5);
  bitsprint8_be(buf2, 0x5a);
  printf("0xa5 = \"%s\"; 0x5a = \"%s\"\n", buf1, buf2);
  assert(STREQ(buf1, "10100101"));
  assert(STREQ(buf2, "01011010"));
  TEST_PASS();
  return;
}

void test_bit_gen() {
  TEST_BEGIN();
  uint8_t value;
  // Test 8 bit gen
  value = bit8_gen("10101010"); assert(value == 0xaa);
  value = bit8_gen("01010101"); assert(value == 0x55);
  value = bit8_gen("1010"); assert(value == 0x0a);
  value = bit8_gen("0"); assert(value == 0x00);
  value = bit8_gen("1"); assert(value == 0x01);
  value = bit8_gen("11111111"); assert(value == 0xff);
  value = bit8_gen("00000000"); assert(value == 0x00);
  //value = bit8_gen("12345"); // Illegal char
  //value = bit8_gen("101010100"); // Too long
  //value = bit8_gen(""); // Too short
  // Test 64 bit gen
  uint64_t value2;
  value2 = bit64_gen("0"); assert(value2 == 0);
  value2 = bit64_gen("1"); assert(value2 == 1);
  value2 = bit64_gen("1010101010101010010101010101010110101010101010100101010101010101"); 
  assert(value2 == 0xaaaa5555aaaa5555UL);
  // Error cases
  //value2 = bit64_gen("10101010101010100101010101010101101010101010101001010101010101010"); // Too long
  TEST_PASS();
  return;
}

void test_bitcpy() {
  TEST_BEGIN();
  uint8_t from, to, result;
  srand(time(NULL));
  from = rand() ^ rand() ^ rand(); to = rand() ^ rand() ^ rand();
  printf("From = 0x%02X; to = 0x%02X\n", from, to);
  int count = 0;
  for(int from_start = 0; from_start < 8;from_start++) {
    for(int to_start = 0; to_start < 8;to_start++) {
      int max_bits = from_start > to_start ? 8 - from_start : 8 - to_start;
      for(int bits = 0; bits <= max_bits; bits++) {
        result = to;
        uint8_t old_from = from;
        bitcpy8(&result, &from, to_start, from_start, bits);
        // Check result
        //char buf1[9], buf2[9], buf3[9];
        //bitsprint8_be(buf1, from); bitsprint8_be(buf2, to); bitsprint8_be(buf3, result);
        //printf("from %s to %s result %s from_start %d to_start %d bits %d\n", 
        //  buf1, buf2, buf3, from_start, to_start, bits);
        for(int i = 0;i < 8;i++) {
          if(i < to_start || i >= to_start + bits) {
            // Condition 1: Bits in "to" are still preserved
            assert(bit8_test(result, i) == bit8_test(to, i));
          } else {
            // Condition 2: All bits in "from" are moved to "to"
            assert(bit8_test(from, from_start + i - to_start) == bit8_test(result, i));
          }
        }
        // Accidentally changed from (since we passed pointer)
        assert(from == old_from);
        count++;
      }
    }
  }
  printf("Tested all possible %d combinations\n", count);
  TEST_PASS();
  return;
}

void test_1_bit_mask() {
  TEST_BEGIN();
  for(int index = 0;index < 64;index++) {
    uint64_t value1 = MASK64_1(index);
    for(int i = 0;i < 64;i++) {
      if(i == index) assert(bit64_test(value1, i) == 1);
      else assert(bit64_test(value1, i) == 0);
    }
  }
  for(int index = 0;index < 32;index++) {
    uint32_t value1 = MASK32_1(index);
    for(int i = 0;i < 32;i++) {
      if(i == index) assert(bit32_test(value1, i) == 1);
      else assert(bit32_test(value1, i) == 0);
    }
  }
  for(int index = 0;index < 8;index++) {
    uint8_t value1 = MASK8_1(index);
    for(int i = 0;i < 8;i++) {
      if(i == index) assert(bit8_test(value1, i) == 1);
      else assert(bit8_test(value1, i) == 0);
    }
  }
  TEST_PASS();
  return;
}

void test_mask_64() {
  TEST_BEGIN();
  //printf("0x%lX 0x%lX\n", 0x1UL << 64, (0x1UL << 64) - 1);
  // Note that mask can accept 0 - 64, rather than 0 - 63
  for(int bits = 0;bits <= 64;bits++) {
    uint64_t value1 = MASK64_LOW_1(bits);
    uint64_t value2 = MASK64_HIGH_1(bits);
    for(int i = 0;i < 64;i++) {
      if(i < bits) assert(bit64_test(value1, i) == 1);
      else assert(bit64_test(value1, i) == 0);
      if(i >= 64 - bits) assert(bit64_test(value2, i) == 1);
      else assert(bit64_test(value2, i) == 0);
    }
  }
  TEST_PASS();
  return;
}

void test_mask_32() {
  TEST_BEGIN();
  for(int bits = 0;bits <= 32;bits++) {
    uint32_t value1 = MASK32_LOW_1(bits);
    uint32_t value2 = MASK32_HIGH_1(bits);
    for(int i = 0;i < 32;i++) {
      if(i < bits) assert(bit32_test(value1, i) == 1);
      else assert(bit32_test(value1, i) == 0); 
      if(i >= 32 - bits) assert(bit32_test(value2, i) == 1);
      else assert(bit32_test(value2, i) == 0);
    }
  }
  TEST_PASS();
  return;
}

void test_mask_8() {
  TEST_BEGIN();
  for(int bits = 0;bits <= 8;bits++) {
    uint8_t value1 = MASK8_LOW_1(bits);
    uint8_t value2 = MASK8_HIGH_1(bits);
    for(int i = 0;i < 8;i++) {
      if(i < bits) assert(bit8_test(value1, i) == 1);
      else assert(bit8_test(value1, i) == 0);
      if(i >= 8 - bits) assert(bit8_test(value2, i) == 1);
      else assert(bit8_test(value2, i) == 0);
    }
  }
  TEST_PASS();
  return;
}

void test_bit_range_64() {
  TEST_BEGIN();
  for(int start = 0;start < 64;start++) {
    for(int bits = 1; bits <= 64 - start;bits++) {
      uint64_t value1 = MASK64_RANGE_1(start, bits);
      uint64_t value2 = MASK64_RANGE_0(start, bits);
      // Both values must be exact complement to each other
      assert((value1 ^ value2) == -1UL); 
      // Check whether all bits are set or cleared as planned
      for(int i = 0;i < 64;i++) {
        if(i >= start && i < start + bits) {
          assert(bit64_test(value1, i) == 1);
          assert(bit64_test(value2, i) == 0);
        } else {
          assert(bit64_test(value1, i) == 0);
          assert(bit64_test(value2, i) == 1);
        }
      }
    }
  }
  TEST_PASS();
  return;
}

void test_bit_range_32() {
  TEST_BEGIN();
  for(int start = 0;start < 32;start++) {
    for(int bits = 1; bits <= 32 - start;bits++) {
      uint32_t value1 = MASK32_RANGE_1(start, bits);
      uint32_t value2 = MASK32_RANGE_0(start, bits);
      assert((value1 ^ value2) == 0xFFFFFFFF); 
      for(int i = 0;i < 32;i++) {
        if(i >= start && i < start + bits) {
          assert(bit32_test(value1, i) == 1);
          assert(bit32_test(value2, i) == 0);
        } else {
          assert(bit32_test(value1, i) == 0);
          assert(bit32_test(value2, i) == 1);
        }
      }
    }
  }
  TEST_PASS();
  return;
}

void test_bit_range_8() {
  TEST_BEGIN();
  for(int start = 0;start < 8;start++) {
    for(int bits = 1; bits <= 8 - start;bits++) {
      uint8_t value1 = MASK8_RANGE_1(start, bits);
      uint8_t value2 = MASK8_RANGE_0(start, bits);
      assert((value1 ^ value2) == 0xFF); 
      for(int i = 0;i < 8;i++) {
        if(i >= start && i < start + bits) {
          assert(bit8_test(value1, i) == 1);
          assert(bit8_test(value2, i) == 0);
        } else {
          assert(bit8_test(value1, i) == 0);
          assert(bit8_test(value2, i) == 1);
        }
      }
    }
  }
  TEST_PASS();
  return;
}

void test_bit_range_set_clear() {
  TEST_BEGIN();
  srand(time(NULL));
  int count = 0;
  for(int start = 0; start < 64;start++) {
    for(int bits = 1; bits <= 64 - start;bits++) {
      uint64_t before = rand_u64();
      uint64_t after1 = bit64_range_set(before, start, bits);
      uint64_t after2 = bit64_range_clear(before, start, bits);
      count++;
      // Check bits
      for(int i = 0;i < 64;i++) {
        if(i >= start && i < start + bits) {
          assert(bit64_test(after1, i) == 1);
          assert(bit64_test(after2, i) == 0);
        } else {
          assert(bit64_test(after1, i) == bit64_test(before, i));
          assert(bit64_test(after2, i) == bit64_test(before, i));
        }
      }
    }
  }
  printf("Checked %d combinations\n", count);
  TEST_PASS();
  return;
}

void test_popcount() {
  TEST_BEGIN();
  assert(bit64_popcount(0x55aaaa5555aaaa55UL) == 32);
  assert(bit64_popcount(0xffffffffffffffffUL) == 64);
  assert(bit64_popcount(0x0UL) == 0);
  assert(bit64_popcount(0x8000000000000000) == 1);
  assert(bit32_popcount(0x55aaaa55) == 16);
  assert(bit32_popcount(0xffffffff) == 32);
  assert(bit32_popcount(0x0) == 0);
  assert(bit32_popcount(0x80000000) == 1);
  assert(bit8_popcount(0x5a) == 4);
  assert(bit8_popcount(0xff) == 8);
  assert(bit8_popcount(0x00) == 0);
  assert(bit8_popcount(0x80) == 1);
  TEST_PASS();
  return;
}

void test_fp_rem() {
  TEST_BEGIN();
  FILE *fp = fopen("common.h", "r");
  SYSEXPECT(fp != NULL);
  int before, after;
  printf("common.h file size %d\n", before = fp_rem(fp));
  fseek(fp, before / 3, SEEK_SET);
  printf("common.h file rem %d\n", after = fp_rem(fp));
  assert(before - after == before / 3);
  int sz = file_size("common.h");
  printf("common.h file size via syscall %d\n", sz);
  assert(sz == before);
  fclose(fp);
  TEST_PASS();
  return;
}

void test_islog2() {
  TEST_BEGIN();
  assert(islog2_u64(65536UL) == 1);
  assert(islog2_u64(12345UL) == 0);
  assert(islog2_64(12345L) == 0);
  assert(islog2_64(0x8000000000000000) == 1);
  assert(islog2_u32(0xffffffff) == 0);
  assert(islog2_u32(0x1) == 1);
  assert(islog2_32(16384 * 16) == 1);
  assert(islog2_8(0x80) == 1);
  assert(islog2_8(0xaa) == 0);
  assert(islog2_u8(0x80) == 1);
  assert(islog2_u8(0x88) == 0);
  TEST_PASS();
  return;
}

void test_nextlog2() {
  TEST_BEGIN();
  assert(nextlog2_u64(0UL) == 1UL);  // Corner case
  assert(nextlog2_u64(-1UL) == 0UL); // Overflow
  assert(nextlog2_u64(255UL) == 256UL);
  assert(nextlog2_u64(65536UL) == 65536UL);
  assert(nextlog2_u64(0x7FFFFFFFFFFFFFFFUL) == 0x8000000000000000UL);
  assert(nextlog2_u64(0x8000000000000001UL) == 0UL); // Overflow
  // Random number test
  rand_init();
  for(int i = 0;i < 1000;i++) {
    uint64_t num = rand_u64();
    uint64_t log2 = nextlog2_u64(num);
    //printf("num 0x%lX log2 %lX\n", num, log2);
    if(num > 0x8000000000000000) assert(log2 == 0UL);
    else if(num == 0) assert(log2 == 1UL);
    else if(bit64_popcount(num) == 1) assert(log2 == num);
    else assert(bit64_popcount(log2) == 1 && log2 > num && (log2 >> 1) < num);
  }
  TEST_PASS();
  return;
}

void test_rand() {
  TEST_BEGIN();
  int high_bits[256];
  int low_bits[256];
  int mid_bits[256];
  memset(high_bits, 0x00, sizeof(high_bits));
  memset(low_bits, 0x00, sizeof(low_bits));
  memset(mid_bits, 0x00, sizeof(mid_bits));
  rand_init();
  for(int i = 0;i < 1000000;i++) {
    uint64_t num = rand_u64();
    high_bits[num >> 56]++;   // Highest 8 bits
    low_bits[num & 0xFFUL]++; // Lowest 8 bits
    mid_bits[(num >> 36) & 0xFFUL]++; // Middle 8 bits
  }
  int high_total = 0, low_total = 0, mid_total = 0;
  for(int i = 0;i < 256;i++) {
    high_total += high_bits[i];
    low_total += low_bits[i];
    mid_total += mid_bits[i];
  }
  double high_avg = high_total / 256.0;
  double low_avg = low_total / 256.0;
  double mid_avg = mid_total / 256.0;
  double high_dev = 0.0, low_dev = 0.0, mid_dev = 0.0;
  for(int i = 0;i < 256;i++) {
    high_dev += () * ();
  }
  TEST_PASS();
  return;
}

int main() {
  printf("========== main ==========\n");
  test_sysexpect();
  test_dbg_printf();
  test_bitsprintf8();
  test_bit_gen();
  test_bitcpy();
  test_1_bit_mask();
  test_mask_64();
  test_mask_32();
  test_mask_8();
  test_bit_range_64();
  test_bit_range_32();
  test_bit_range_8();
  test_bit_range_set_clear();
  test_popcount();
  test_fp_rem();
  test_islog2();
  test_nextlog2();
  test_rand();
  printf("==========================\n");
  return 0;
}