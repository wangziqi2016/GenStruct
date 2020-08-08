
#include "bintree.h"

// Generates random keys, inserts into the bintree and also put them into the array
// Value is always key * 2 + 1
static bintree_t *bintree_populate(int iter, uint64_t **array) {
  *array = (uint64_t *)malloc(sizeof(uint64_t) * iter);
  SYSEXPECT(*array != NULL);
  rand_init();
  bintree_t *bintree = bintree_init();
  int array_index = 0;
  for(int i = 0;i < iter;i++) {
    uint64_t key = rand_u64();
    int ret = bintree_insert(bintree, (void *)key, (void *)(key * 2 + 1));
    assert(ret == 1);
    (*array)[array_index++] = key;
  }
  return bintree;
}

void test_insert() {
  TEST_BEGIN();
  int iter = 1000000;
  uint64_t *array = NULL;
  struct timespec begin_time;
  clock_gettime(CLOCK_MONOTONIC, &begin_time);
  bintree_t *bintree = bintree_populate(iter, &array);
  struct timespec end_time;
  clock_gettime(CLOCK_MONOTONIC, &end_time);
  uint64_t delta_sec = end_time.tv_sec - begin_time.tv_sec;
  uint64_t delta_nsec = end_time.tv_nsec - begin_time.tv_nsec;
  uint64_t nsec = delta_sec * 1000000000UL + delta_nsec;
  printf("Time = %lu ns\n", nsec);
  printf("Insert throughput = %lf Mop/sec\n", (iter / 1000000.0f) / (nsec / 1000000000.0f));
  for(int i = 0;i < iter;i++) {
    uint64_t key = array[i];
    void *value;
    int ret = bintree_search(bintree, (void *)key, &value);
    assert(ret == 1 && (uint64_t)value == key * 2 + 1);
  }
  bintree_free(bintree);
  TEST_PASS();
  return;
}

void test_remove() {
  TEST_BEGIN();
  int iter = 1000000;
  uint64_t *array = NULL;
  bintree_t *bintree = bintree_populate(iter, &array);
  for(int i = 0;i < iter;i++) {
    uint64_t key = array[i];
    void *value;
    int ret = bintree_remove(bintree, (void *)key, &value);
    assert(ret == 1 && (uint64_t)value == key * 2 + 1);
  }
  assert(bintree->root == NULL);
  bintree_free(bintree);
  TEST_PASS();
  return;
}

// Remove keys in the array randomly
void test_random_remove() {
  TEST_BEGIN();
  int iter = 1000000;
  uint64_t *array = NULL;
  bintree_t *bintree = bintree_populate(iter, &array);
  // We reshuffle random array
  for(int i = 0;i < iter;i++) {
    int index = rand() % iter;
    uint64_t t = array[index];
    array[index] = array[i];
    array[i] = t;
  }
  for(int i = 0;i < iter;i++) {
    uint64_t key = array[i];
    void *value;
    int ret = bintree_remove(bintree, (void *)key, &value);
    assert(ret == 1 && (uint64_t)value == key * 2 + 1);
  }
  assert(bintree->root == NULL);
  bintree_free(bintree);
  TEST_PASS();
  return;
}

typedef struct {
  int count;
  int first_key;
  uint64_t prev_key;
  int mode;
} test_traverse_arg_t;

static test_traverse_cb(bintree_node_t *node, void *_arg) {
  test_traverse_arg_t *arg = (test_traverse_arg_t *)arg;
  // Check global key ordering for in-order traversal
  if(arg->mode == BINTREE_TRAVERSE_INORDER && arg->count != 0) {
    assert(arg->prev_key < (uint64_t)node->key);
  }
  // Check ordering of keys for single element
  if(node->left != NULL) assert((uint64_t)node->left->key < (uint64_t)node->key);
  if(node->right != NULL) assert((uint64_t)node->right->key > (uint64_t)node->key);
  arg->prev_key = (uint64_t)node->key;
  arg->count++;
  return;
}

void test_traverse() {
  TEST_BEGIN();

  TEST_PASS();
  return;
}

int main() {
  test_insert();
  test_remove();
  test_random_remove();
  test_traverse();
  return 0;
}