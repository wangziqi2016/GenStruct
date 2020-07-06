
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

int main() {
  printf("========== main ==========\n");
  test_sysexpect();
  test_dbg_printf();
  test_bitsprintf8();
  printf("==========================\n");
  return 0;
}