
#include "btree.h"

void test_search() {
  TEST_BEGIN();
  btree_node_t *node = btree_node_init(BTREE_NODE_LEAF);
  assert(node->type == BTREE_NODE_LEAF);
  rand_init();
  for(int i = 0;i < BTREE_LEAF_CAPACITY;i++) {
    node->kv[i].key = node->kv[i].value = (void *)rand_u64();
  }
  node->count = BTREE_LEAF_CAPACITY;
  for(int i = 0;i < BTREE_LEAF_CAPACITY;i++) {
    for(int j = 0;j < BTREE_LEAF_CAPACITY - 1;j++) {
      if((uint64_t)node->kv[j].key > (uint64_t)node->kv[j + 1].key) {
        btree_kv_t t = node->kv[j + 1];
        node->kv[j + 1] = node->kv[j];
        node->kv[j] = t;
      }
    }
  }
  // Make sure ordering is observed
  for(int i = 0;i < BTREE_LEAF_CAPACITY - 1;i++) {
    assert((uint64_t)node->kv[i].key < (uint64_t)node->kv[i + 1].key);
  }
  for(int i = 0;i < BTREE_LEAF_CAPACITY - 1;i++) {
    uint64_t key = (uint64_t)node->kv[i].key;
    int index;
    int found = btree_node_search_u64(node, key, &index);
    assert(found == 1 && index == i);
    assert(key == (uint64_t)node->kv[i].key);
    assert(key == (uint64_t)node->kv[i].value);
  }
  TEST_PASS();
  return;
}

int main() {
  test_search();
  return 0;
}