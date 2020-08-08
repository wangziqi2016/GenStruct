
#include "bintree.h"

bintree_node_t *bintree_node_init() {
  bintree_node_t *node = (bintree_node_t *)malloc(sizeof(bintree_node_t));
  SYSEXPECT(node != NULL);
  memset(node, 0x00, sizeof(bintree_node_t));
  return node;
}

// This function recursively frees node. If the height is too large it will fail (modern 64 bit arch has really big 
// per-thread stacks, so it should not be an issue in practice)
void bintree_node_free(bintree_node_t *node) {
  if(node == NULL) return;
  bintree_node_free(node->left);
  bintree_node_free(node->right);
  free(node);
  return;
}

bintree_t *bintree_init() {
  bintree_t *bintree = (bintree_t *)malloc(sizeof(bintree_t));
  SYSEXPECT(bintree != NULL);
  memset(bintree, 0x00, sizeof(bintree_t));
  return bintree;
}

void bintree_free(bintree_t *bintree) {
  if(bintree->root != NULL) bintree_node_free(bintree->root);
  free(bintree);
  return;
}
