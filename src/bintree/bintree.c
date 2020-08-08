
#include "bintree.h"

bintree_node_t *bintree_node_init() {
  bintree_node_t *node = (bintree_node_t *)malloc(sizeof(bintree_node_t));
  SYSEXPECT(node != NULL);
  memset(node, 0x00, sizeof(bintree_node_t));
  return node;
}

