
#include "pq.h"

typedef struct {
  int key;
  int value;
} pq_test_node_t;



void test_pq_push() {
  TEST_BEGIN();

  TEST_PASS();
  return;
}


int main() {
  printf("========== pq ==========\n");
  test_pq_push();
  printf("========================\n");
  return 0;
}