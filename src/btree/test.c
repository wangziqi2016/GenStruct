
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
  // Test existing keys
  for(int i = 0;i < BTREE_LEAF_CAPACITY - 1;i++) {
    uint64_t key = (uint64_t)node->kv[i].key;
    int index;
    int found = btree_node_search_u64(node, key, &index);
    assert(found == 1 && index == i);
    assert(key == (uint64_t)node->kv[i].key);
    assert(key == (uint64_t)node->kv[i].value);
  }
  // Test random keys (most likely not found)
  int exist_count = 0;
  const rand_iter = 100000;
  for(int i = 0;i < rand_iter;i++) {
    uint64_t key = rand_u64();
    int exist = 0;
    int exist_index = -1;
    for(int j = 0;j < BTREE_LEAF_CAPACITY;j++) {
      if(key == (uint64_t)node->kv[j].key) {
        exist = 1;
        exist_index = j;
        break;
      } 
    }
    int index;
    int found = btree_node_search_u64(node, key, &index);
    if(exist == 1) {
      assert(found == 1 && exist_index == index);
      exist_count++;
    } else {
      assert(found == 0);
      // Index could equal BTREE_LEAF_CAPACITY
      assert(index >= 0 && index <= BTREE_LEAF_CAPACITY);
      // Key should be between kv[index - 1] and kv[index]
      if(index > 0) assert((uint64_t)node->kv[index - 1].key < key);
      if(index < BTREE_LEAF_CAPACITY) assert(key < (uint64_t)node->kv[index].key);
    }
  }
  printf("Found %d Not Found %d\n", exist, rand_iter - exist);
  TEST_PASS();
  return;
}

int main() {
  test_search();
  return 0;
}