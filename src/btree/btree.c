
#include "btree.h"

btree_node_t *btree_node_init(int type) {
  assert(type == BTREE_NODE_INNER || type == BTREE_NODE_LEAF);
  int capacity = (type == BTREE_NODE_INNER) ? BTREE_INNER_CAPACITY : BTREE_LEAF_CAPACITY;
  btree_node_t *node = (btree_node_t *)malloc(sizeof(btree_node_t) + sizeof(btree_kv_t) * capacity);
  node->type = type;
  node->count = 0;
  node->next = NULL;
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
  btree->key_cmp_func = NULL;
  btree->search_func = btree_node_search_u64;
  // Use an empty leaf node as initial state
  btree->root = btree->first_leaf = btree_node_init(BTREE_NODE_LEAF);
  return btree;
}

btree_t *btree_init_custom_key(btree_key_cmp_func_t key_cmp_func) {
  if(key_cmp_func == NULL) error_exit("The key comparison function must not be NULL\n");
  btree_t *btree = btree_init();
  btree->key_cmp_func = key_cmp_func;
  btree->search_func = btree_node_search;
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
  // Inner nodes do not use the first element, since there is no fixed low key
  int start = (node->type == BTREE_NODE_INNER) ? 1 : 0, end = node->count;
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
  assert(start == end || (start == 1 && end == 0));
  *index = end;
  return 0;
}

// Generic search function using key call backs
int btree_node_search(btree_node_t *node, void *key, int *index, btree_key_cmp_func_t key_cmp_func) {
  assert(key_cmp_func != NULL);
  if(node->count == 0) { *index = 0; return 0; }
  int start = (node->type == BTREE_NODE_INNER) ? 1 : 0, end = node->count;
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
  assert(start == end || (start == 1 && end == 0));
  *index = end;
  return 0;
}

// Returns 1 if insertion succeeds; returns 0 if key already exists
// Only supports non-full node insert
int btree_node_insert(btree_t *btree, btree_node_t *node, void *key, void *value) {
  assert((node->type == BTREE_NODE_INNER && node->count < BTREE_INNER_CAPACITY) || \
         (node->type == BTREE_NODE_LEAF && node->count < BTREE_LEAF_CAPACITY));
  int index;
  int found = btree->search_func(node, key, &index, btree->key_cmp_func);
  if(found == 1) return 0;
  assert(found == 0 && index <= node->count && index >= 0);
  // Easy case: just append at the end
  if(index < node->count) {
    memmove(node->kv + index + 1, node->kv + index, (node->count - index) * sizeof(btree_kv_t));  
  }
  node->kv[index].key = key;
  node->kv[index].value = value; 
  node->count++;
  return 1;
}

// The sibling node contains half of the larger key array in the original node
// The next pointer field of the sibling and the original node is updated accordingly
btree_node_t *btree_node_split(btree_node_t *node) {
  assert(node->count > 1);
  // Split node into [0, mid) and [mid, count)
  int mid = node->count / 2;
  btree_node_t *sibling = btree_node_init(node->type);
  assert(sibling->type == node->type);
  memcpy(sibling->kv, node->kv + mid, (node->count - mid) * sizeof(btree_kv_t));
  // Update element count
  sibling->count = node->count - mid;
  node->count = mid;
  // Update next pointer field
  sibling->next = node->next;
  node->next = sibling;
  return sibling;
}

// This function assumes that the given inner node is already the correct node to be used
btree_node_t *btree_next_level(btree_t *btree, btree_node_t *const node, void *key) {
  assert(node->type == BTREE_NODE_INNER);
  int index;
  int found = btree->search_func(node, key, &index, btree->key_cmp_func);
  btree_node_t *child;
  if(found == 1) {
    assert(index >= 0 && index < node->count);
    child = (btree_node_t *)node->kv[index].value;
  } else {
    // Note that in this case it is impossible to get the first element
    assert(index > 0 && index <= node->count);
    child = (btree_node_t *)node->kv[index - 1].value;
  }
  // Then check B-Link property to see whether there is an unfinished node split
  assert(child->next == NULL || child->next->count > 0); // If not NULL then must be > 0
  int b_link = 0;
  if(child->next != NULL) {
    void *child_sibling_key = child->next->kv[0].key;
    // There is indeed an unfinished node split, finish it by inserting into the current level
    if(btree_key_less(btree, key, child_sibling_key) == 0) {
      b_link = 1;
      // If the current node is not large enough, then split it and that's all
      // The B-Link design ensures that the next thread seeing this will help along
      if(node->count < BTREE_INNER_CAPACITY) {
        btree_node_insert(btree, node, child_sibling_key, child->next);
      } else {
        btree_node_t *sibling = btree_node_split(node);
        if(btree_key_less(btree, child_sibling_key, sibling->kv[0].key)) {
          btree_node_insert(btree, node, child_sibling_key, child->next);
        } else {
          btree_node_insert(btree, sibling, child_sibling_key, child->next);
        }
        // Root must not observe B-Links
        if(btree->root == node) {
          btree->root = btree_node_init(BTREE_NODE_INNER);
          btree_node_insert(btree, btree->root, node->kv[0].key, node);
          btree_node_insert(btree, btree->root, sibling->kv[0].key, sibling);
        }
      } 
    }
  }
  return b_link ? child->next : child;
}

btree_node_t *btree_traverse(btree_t *btree, void *key) {
  btree_node_t *curr = btree->root;
  while(curr->type == BTREE_NODE_INNER) {
    curr = btree_next_level(btree, curr, key);
  }
  assert(curr->type == BTREE_NODE_LEAF);
  // Note: The low key of the first leaf is -inf, so we should not check this
  //assert(btree_key_less(btree, key, curr->kv[0].key) == 0);
  assert(curr->next == NULL || curr->next->count == 0 || btree_key_less(btree, key, curr->next->kv[0].key));
  return curr;
}

// Returns 1 if insertion succeeds, 0 otherwise
int btree_insert(btree_t *btree, void *key, void *value) {
  btree_node_t *node = btree_traverse(btree, key);
  assert(node->type == BTREE_NODE_LEAF && node->count <= BTREE_LEAF_CAPACITY);
  // Easy case: Just insert when the leaf is not full
  if(node->count != BTREE_LEAF_CAPACITY) {
    return btree_node_insert(btree, node, key, value);
  }
  btree_node_t *sibling = btree_node_split(node);
  int ret;
  if(btree_key_less(btree, key, sibling->kv[0].key)) {
    ret = btree_node_insert(btree, node, key, value);
  } else {
    ret = btree_node_insert(btree, sibling, key, value);
  }
  if(btree->root == node) {
    btree->root = btree_node_init(BTREE_NODE_INNER);
    btree_node_insert(btree, btree->root, node->kv[0].key, node);
    btree_node_insert(btree, btree->root, sibling->kv[0].key, sibling);
  }
  return ret;
}

int btree_search(btree_t *btree, void *key, void **value) {
  btree_node_t *node = btree_traverse(btree, key);
  assert(node->type == BTREE_NODE_LEAF && node->count <= BTREE_LEAF_CAPACITY);
  int index;
  int found = btree->search_func(node, key, &index, btree->key_cmp_func);
  if(found == 0) return 0;
  *value = node->kv[index].value;
  return 1;
}
