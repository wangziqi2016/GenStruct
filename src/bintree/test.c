
#include "bintree.h"

// Generates random keys, inserts into the bintree and also put them into the array
// Value is always key * 2 + 1
static bintree_t *bintree_populate(int iter, uint64_t *array) {
  rand_init();
  bintree_t *bintree = bintree_init();
  int array_index = 0;
  for(int i = 0;i < iter;i++) {
    uint64_t key = rand_u64();
    int ret = bintree_insert(bintree, (void *)key, (void *)(key * 2 + 1));
    assert(ret == 1);
    array[array_index++] = key;
  }
  return bintree;
}

void test_insert() {
  TEST_BEGIN();
  int iter = 100000;
  uint64_t *array = (uint64_t *)malloc(sizeof(uint64_t) * iter);
  SYSEXPECT(arraay != NULL);
  bintree_t *bintree = bintree_populate(iter, array);
  for(int i = 0;i < iter;i++) {
    uint64_t key = array[i];
    void *value;
    int ret = bintree_search(bintree, (void *)key, &value);
    assert(ret == 1 && (uint64_t)value == key * 2 + 1);
  }
  TEST_PASS();
  return;
}

int main() {
  test_insert();
  return 0;
}