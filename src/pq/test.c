
#include "pq.h"

typedef struct {
  int key;
  int value;
  int index;     // The index of the key in the array
} pq_test_node_t;

// Note that the value is set to key * 2 + 1
static pq_test_node_t *pq_test_node_init(int key) {
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
  return ((pq_test_node_t *)a)->key < ((pq_test_node_t *)b)->key;
}

static pq_t *build_pq(int test_size, int init_size, int **array) {
  pq_t *pq = pq_init_size(pq_test_node_less_cb, init_size);
  int index = 0;
  *array = (int *)malloc(sizeof(int) * test_size);
  SYSEXPECT(*array != NULL);
  srand(time(NULL));
  for(int i = 0;i < test_size;i++) {
    int key = rand();
    // Add key to the array
    (*array)[index++] = key;
    // Push the node into the pq
    pq_test_node_t *node = pq_test_node_init(key);
    // Note that index has been incremented
    node->index = index - 1;
    pq_push(pq, node);
  }
  assert(test_size == index);
  return pq;
}

void test_pq_push() {
  TEST_BEGIN();
  const int test_size = 1000;
  const int init_size = 3;
  int *array = NULL;
  // This inserts into the pq
  pq_t *pq = build_pq(test_size, init_size, &array);
  pq_it_t it = pq_begin(pq);
  while(!pq_it_isend(&it)) {
    pq_test_node_t *node = pq_it_data(&it);
    assert(node->key == array[node->index]);
    assert(node->value == node->key * 2 + 1);
    pq_it_next(&it);
  }
  pq_free_all(pq, pq_test_node_free);
  TEST_PASS();
  return;
}


int main() {
  printf("========== pq ==========\n");
  test_pq_push();
  printf("========================\n");
  return 0;
}