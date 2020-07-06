
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

void test_bit8_gen() {
  TEST_BEGIN();
  uint8_t value;
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
  TEST_PASS();
  return;
}

void test_bitcpy() {
  TEST_BEGIN();
  uint8_t from, to;
  from = 0x55; to = 0xaa;
  for(int from_start = 0; from_start < 8;from_start++) {
    for(int to_start = 0; to_start < 8;to_start++) {
      for(int bits = 0; bits <= 8; bits++) {
        result = to;
        uint8_t old_from = from;
        result = bitcpy8(&result, &from, to_start, from_start, bits);
        // Check result
        for(int i = 0;i < 8;i++) {
          if(i < to_start || i >= to_start + bits) {
            // Condition 1: Bits in "to" are still preserved
            assert(bit8_test(result, i) == bit8_test(to, i));
          } else {
            // Condition 2: All bits in "from" are moved to "to"
            assert(bit8_test(from, from_start + to_start - i) == bit8_test(result, i));
          }
        }
        // Accidentally changed from (since we passed pointer)
        assert(from == old_from);
      }
    }
  }
  TEST_PASS();
  return;
}

int main() {
  printf("========== main ==========\n");
  test_sysexpect();
  test_dbg_printf();
  test_bitsprintf8();
  test_bit8_gen();
  test_bitcpy();
  printf("==========================\n");
  return 0;
}