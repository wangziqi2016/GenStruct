
#include "btree.h"

btree_node_t *btree_node_init(int type) {
  assert(type == BTREE_NODE_INNER || type == BTREE_NODE_LEAF);
  int capacity = (type == BTREE_NODE_INNER) ? BTREE_INNER_CAPACITY : BTREE_LEAF_CAPACITY;
  btree_node_t *node = (btree_node_t *)malloc(sizeof(btree_node_t) + sizeof(btree_kv_t) * capacity);
  node->type = type;
  node->count = 0;
  SYSEXPECT(node != NULL);
  return node;
}

// This function is recursive. It traverses all nodes below until leaf level is reached, and frees all of them
void btree_node_free(btree_node_t *node) {
  assert(node->type == BTREE_NODE_INNER || node->type == BTREE_NODE_LEAF);
  if(node->type == BTREE_NODE_LEAF) return;
  for(int i = 0;i < node->count;i++) {
    btree_node_free(node->kv[i].value);
  }
  free(node);
  return;
}

btree_t *btree_init() {
  btree_t *btree = (btree_t *)malloc(sizeof(btree_t));
  SYSEXPECT(btree != NULL);
  memset(btree, 0x00, sizeof(btree_t));
  return btree;
}

void btree_free(btree_t *btree) {
  if(btree->root != NULL) btree_node_free(btree->root);
  free(btree);
  return;
}

// If found, return 1, otherwise return 0
// If exact match is found, index is set to the index of the item; Otherwise it is set
// to the next largest item (which could point to the end of the array)
int btree_node_search_u64(btree_node_t *node, void *_key, int *index, btree_key_cmp_func_t key_cmp_func) {
  assert(key_cmp_func == NULL);
  uint64_t key = (uint64_t)_key;
  // Invariant: start < end; Search in [start, end)
  int start = 0, end = node->count;
  while(start < end) {
    int mid = (start + end) / 2;
    if(key == (uint64_t)node->kv[mid].key) {
      *index = mid;
      return 1;
    } else if(key < (uint64_t)node->kv[mid].key) {
      end = mid;
    } else {
      start = mid + 1;
    }
  }
  assert(start == end);
  *index = start;
  return 0;
}

// Generic search function using key call backs
int btree_node_search(btree_node_t *node, void *key, int *index, btree_key_cmp_func_t key_cmp_func) {
  assert(key_cmp_func != NULL);
  int start = 0, end = node->count;
  while(start < end) {
    int mid = (start + end) / 2;
    int cmp = key_cmp_func(key, node->kv[mid].key);
    if(cmp == 0) {
      *index = mid;
      return 1;
    } else if(cmp < 0) {
      end = mid;
    } else {
      assert(cmp > 0);
      start = mid + 1;
    }
  }
  assert(start == end);
  *index = start;
  return 0;
}
