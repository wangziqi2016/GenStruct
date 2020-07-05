
#include "error.h"

void test_sysexpect() {
  printf("========== %s ==========\n", __func__);
  void *p = malloc(1234);
  SYSEXPECT(p != NULL);
  printf("Pass!\n");
  return;
}

void test_dbg_printf() {
  printf("========== %s ==========\n", __func__);
  void *p = malloc(1234);
  SYSEXPECT(p != NULL);
  printf("Pass!\n");
}

int main() {
  test_sysexpect();

}