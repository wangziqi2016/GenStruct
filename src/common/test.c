
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

int main() {
  printf("========== main ==========\n");
  test_sysexpect();
  test_dbg_printf();
  printf("==========================\n");
  return 0;
}