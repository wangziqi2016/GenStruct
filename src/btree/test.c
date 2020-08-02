
#include "btree.h"

void test_search() {
  TEST_BEGIN();
  btree_node_t *node = btree_node_init(BTREE_NODE_LEAF);
  assert(node->type == BTREE_NODE_LEAF && node->count == 0);
  rand_init();
  // First test empty node search
  for(int i = 0;i < 1000;i++) {
    int index;
    int found = btree_node_search_u64(node, (void *)rand_u64(), &index, NULL);
    assert(found == 0 && index == 0);
  }
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
  TEST_BEGIN();
  btree_t *btree = btree_init();
  uint64_t keys[BTREE_LEAF_CAPACITY];
  btree_node_t *node = populate_leaf(btree, BTREE_LEAF_CAPACITY, keys);
  node->next = node; // We test the non-trivial case
  btree_node_t *sibling = btree_node_split(node);
  assert(node->count + sibling->count == BTREE_LEAF_CAPACITY);
  assert(node->next == sibling);
  assert(sibling->next == node); // Special arrangement, see above
  assert((uint64_t)node->kv[node->count - 1].key < (uint64_t)sibling->kv[0].key);
  for(int i = 0;i < node->count - 1;i++) assert((uint64_t)node->kv[i].key < (uint64_t)node->kv[i + 1].key);
  for(int i = 0;i < sibling->count - 1;i++) assert((uint64_t)sibling->kv[i].key < (uint64_t)sibling->kv[i + 1].key);
  for(int i = 0;i < BTREE_LEAF_CAPACITY;i++) {
    uint64_t key = keys[i];
    int index1, index2;
    int found1, found2;
    found1 = btree_node_search_u64(node, (void *)key, &index1, NULL); 
    found2 = btree_node_search_u64(sibling, (void *)key, &index2, NULL); 
    assert((found1 ^ found2) == 1);
  }
  btree_node_free(node);
  btree_node_free(sibling);
  btree_free(btree);
  TEST_PASS();
  return;
}

static int cmp_u64(const void *_n1, const void *_n2) { 
  uint64_t n1 = *(uint64_t *)_n1; uint64_t n2 = *(uint64_t *)_n2;
  if(n1 < n2) return -1; 
  else if(n1 == n2) return 0; 
  else return 1; 
}

// Returns a populated tree filled with random values
btree_t *populate_tree(int iter, uint64_t *keys) {
  btree_t *btree = btree_init();
  rand_init();
  int key_index = 0;
  for(int i = 0;i < iter;i++) {
    uint64_t key = rand_u64();
    keys[key_index++] = key;
    int ret = btree_insert(btree, (void *)key, (void *)key);
    assert(ret == 1);
  }
  return btree;
}

void test_btree_insert() {
  TEST_BEGIN();
  const int iter = 1000000;
  uint64_t *keys = malloc(iter * sizeof(uint64_t));
  SYSEXPECT(keys != NULL);
  btree_t *btree = populate_tree(iter, keys);
  for(int i = 0;i < iter;i++) {
    uint64_t value;
    int found = btree_search(btree, (void *)keys[i], (void **)&value);
    assert(found == 1 && value == keys[i]);
  }
  qsort(keys, iter, sizeof(uint64_t), cmp_u64);
  for(int i = 0;i < iter - 1;i++) {
    assert(keys[i] < keys[i + 1]);
  }
  // Check sorted property
  btree_node_t *leaf = btree->first_leaf;
  int leaf_count = 0;
  int element_count = 0;
  int leaf_index = 0;
  for(int i = 0;i < iter;i++) {
    while(leaf_index == leaf->count) {
      leaf = leaf->next;
      leaf_index = 0;
      leaf_count++;
    }
    element_count++;
    assert(leaf != NULL);
    assert(keys[i] == (uint64_t)leaf->kv[leaf_index].key);
    leaf_index++;
  }
  assert(leaf_index == leaf->count && leaf->next == NULL);
  printf("Leafs %d elements %d occupancy %lf (CAP %d)\n", 
    leaf_count, element_count, (double)element_count / leaf_count, BTREE_LEAF_CAPACITY);
  free(keys);
  btree_free(btree);
  TEST_PASS();
  return;
}

static int bin_search(uint64_t *array, int count, uint64_t key) {
  int start = 0, end = count;
  while(start < end) {
    int mid = (start + end) / 2;
    if(key == array[mid]) {
      return mid;
    } else if(key < array[mid]) {
      end = mid;
    } else {
      start = mid + 1;
    }
  }
  assert(start == end);
  return start;
}

void test_btree_it() {
  TEST_BEGIN();
  const int iter = 100000;
  uint64_t *keys = malloc(iter * sizeof(uint64_t));
  SYSEXPECT(keys != NULL);
  btree_t *btree = populate_tree(iter, keys);
  qsort(keys, iter, sizeof(uint64_t), cmp_u64);
  for(int i = 0;i < 100;i++) {
    uint64_t it_key = rand_u64();
    btree_it_t it = btree_it_begin(btree, (void *)it_key);
    int index = bin_search(keys, iter, it_key);
    while(btree_it_isend(&it) == 0) {
      assert(btree_it_key(&it) == btree_it_value(&it));
      assert((uint64_t)btree_it_key(&it) == keys[index]);
      btree_it_next(&it);
      assert(index < iter);
      index++;
    }
    assert(index == iter);
  }
  TEST_PASS();
  return;
}

int main() {
  test_search();
  test_insert();
  test_split();
  test_btree_insert();
  test_btree_it();
  return 0;
}