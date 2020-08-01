
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
    int found = btree_node_search_u64(node, (void *)key, &index, NULL);
    assert(found == 1 && index == i);
    assert(key == (uint64_t)node->kv[i].key);
    assert(key == (uint64_t)node->kv[i].value);
  }
  // Test random keys (most likely not found)
  int exist_count = 0;
  const int rand_iter = 100000;
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
    int found = btree_node_search_u64(node, (void *)key, &index, NULL);
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
  printf("Found %d Not Found %d\n", exist_count, rand_iter - exist_count);
  btree_node_free(node);
  TEST_PASS();
  return;
}

// Returns a populated leaf node with given number of random uint64_t key and value (key == value)
// The optional keys array holds unsorted keys
btree_node_t *populate_leaf(btree_t *btree, int count, uint64_t *keys) {
  if(btree->key_cmp_func != NULL) error_exit("Only supports uint64_t keys\n");
  btree_node_t *node = btree_node_init(BTREE_NODE_LEAF);
  int key_index = 0;
  rand_init();
  for(int i = 0;i < count;i++) {
    uint64_t key = rand_u64();
    if(keys != NULL) keys[key_index++] = key;
    btree_node_insert(btree, node, (void *)key, (void *)key);
  }
  assert(node->count == count);
  return node;
}

void test_insert() {
  TEST_BEGIN();
  btree_t *btree = btree_init();
  uint64_t keys[BTREE_LEAF_CAPACITY];
  btree_node_t *node = populate_leaf(btree, BTREE_LEAF_CAPACITY, keys);
  // Check that all values are in the node
  for(int i = 0;i < BTREE_LEAF_CAPACITY;i++) {
    int index;
    int found = btree_node_search_u64(node, (void *)keys[i], &index, NULL);
    assert(found == 1 && index >= 0 && index < node->count);
  }
  // Check that the node is ordered
  for(int i = 0;i < BTREE_LEAF_CAPACITY - 1;i++) {
    assert((uint64_t)node->kv[i].key < (uint64_t)node->kv[i + 1].key);
  }
  btree_node_free(node);
  btree_free(btree);
  TEST_PASS();
  return;
}

void test_split() {
  btree_t *btree = btree_init();
  btree_node_t *node = populate_leaf(btree, BTREE_LEAF_CAPACITY, NULL);
  btree_node_t *sibling = btree_node_split(node);

  btree_node_free(node);
  btree_node_free(sibling);
  btree_free(btree);
  return;
}

int main() {
  test_search();
  test_insert();
  test_split();
  return 0;
}