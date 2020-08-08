
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

int bintree_insert(bintree_t *bintree, void *key, void *value) {
  if(bintree->root == NULL) {
    bintree->root = bintree_node_init();
    bintree->root->key = key;
    bintree->root->value = value;
  } else {
    bintree_node_t *node = bintree->root;
    while(1) {
      bintree_node_t **next = NULL;
      if(BINTREE_KEY_EQ(key, node->key)) {
        return 0;
      } else if(BINTREE_KEY_LESS(key, node->key)) {
        next = &node->left;
      } else {
        next = &node->right;
      }
      if(*next == NULL) {
        *next = bintree_node_init();
        (*next)->key = key;
        (*next)->value = value;
        break;
      } else {
        node = *next;
      }
    }
  }
  return 1;
}

int bintree_search(bintree_t *bintree, void *key, void **value) {
  bintree_node_t *node = bintree->root;
  while(node != NULL) {
    if(BINTREE_KEY_EQ(key, node->key)) {
      *value = node->value;
      return 1;
    } else if(BINTREE_KEY_LESS(key, node->key)) {
      node = node->left;
    } else {
      node = node->right;
    }
  }
  return 0;
}

int bintree_remove(bintree_t *bintree, void *key, void **value) {
  bintree_node_t *node = bintree->root;
  bintree_node_t **parent_link = NULL;
  while(node != NULL) {
    if(BINTREE_KEY_EQ(key, node->key)) {
      // Optionally return the value
      if(value != NULL) *value = node->value;
      break;
    } else if(BINTREE_KEY_LESS(key, node->key)) {
      parent_link = &node->left;
      node = node->left;
    } else {
      parent_link = &node->right;
      node = node->right;
    }
  }
  if(node == NULL) return 0;
  
  // After this point, parent_link points to NULL or the node parent's link to the node
  // We next swap the "removed" node with nodes below it such that binary property is not violated, while
  // the node to be deleted moves down the tree and finally reach leaf level
  while(1) {
    bintree_node_t *removed = node;
    if(node->left != NULL) {
      // Select the biggest node is the left subtree, i.e. go right in the left subtree until NULL
      parent_link = &node->left;
      node = node->left;
      while(node->right != NULL) {
        parent_link = &node->right;
        node = node->right;
      }
    } else if(node->right != NULL) {
      assert(node->left == NULL);
      parent_link = &node->right;
      node = node->right;
      while(node->left != NULL) {
        parent_link = &node->left;
        node = node->left;
      }
    } else {
      // Leaf node is removed directly
      assert(node->left == NULL && node->right == NULL);
      bintree_node_free(node);
      // If removing the root node, then set root to NULL
      if(parent_link == NULL) bintree->root = NULL;
      else *parent_link = NULL;
      break;
    }
    // Swap nodes
    removed->key = node->key;
    removed->value = node->value;
  }
  return 1;
}
