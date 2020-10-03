
#include "pq.h"

typedef struct {
  int key;
  int value;
} pq_test_node_t;

// Note that the value is set to key * 2 + 1
static node_t *pq_test_node_init(int key) {
  pq_test_node_t *node = (pq_test_node_t *)malloc(sizeof(pq_test_node_t));
  SYSEXPECT(node != NULL);
  memset(node, 0x00, sizeof(pq_test_node_t));
  node->key = key;
  node->value = key * 2 + 1;
  return node;
}

// This function takes void * argument for the pq_free_all() call back
static void pq_test_node_free(void *node) {
  free((pq_test_node_t *)node);
  return;
}

static int pq_test_node_less_cb(void *a, void *b) {
  return a->key < b->key;
}

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