
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