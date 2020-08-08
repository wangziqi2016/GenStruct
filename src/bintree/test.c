
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
  int iter = 100000;

}

int main() {
  test_insert();
  return 0;
}