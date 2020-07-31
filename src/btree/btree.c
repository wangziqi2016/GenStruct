
#include "btree.h"

btree_node_t *btree_node_init(int type) {
  assert(type == BTREE_NODE_INNER || type == BTREE_NODE_LEAF);
  int capacity = (type == BTREE_NODE_INNER) ? BTREE_INNER_CAPACITY : BTREE_LEAF_CAPACITY;
  bttree_node_t *node = (btree_node_t *)malloc(sizeof(btree_node_t) + sizeof(btree_kv_t) * capacity);
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