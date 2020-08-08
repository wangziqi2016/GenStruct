
#ifndef _BINTREE_H
#define _BINTREE_H

#include "common.h"

// The following must be overridden if the default key comparison function is not
// built-in comparison operation
// If you want to use customized comparison function, define the same macto in the wrapper file

#ifndef BINTREE_KEY_LESS
#define BINTREE_KEY_LESS(x, y) ((uint64_t)x < (uint64_t)y)
#endif

#ifndef BINTREE_KEY_EQ
#define BINTREE_KEY_EQ(x, y) ((uint64_t)x == (uint64_t)y)
#endif

typedef struct bintree_node_struct_t {
  void *key;
  void *value;
  struct bintree_node_struct_t *left;
  struct bintree_node_struct_t *right;
} bintree_node_t;

typedef struct {
  bintree_node_t *root;
} bintree_t;

bintree_node_t *bintree_node_init();
void bintree_node_free(bintree_node_t *node); // This function recursively frees all child nodes below

bintree_t *bintree_init();
void bintree_free(bintree_t *bintree);

// Returns 1 if insert succeeds; 0 if fails
int bintree_insert(bintree_t *bintree, void *key, void *value);
// Returns 1 if search succeeds and last argument contains the value
int bintree_search(bintree_t *bintree, void *key, void **value);
// Optionally return the removed value in the last argument; ignored if it is set to NULL 
int bintree_remove(bintree_t *bintree, void *key, void **value);

#endif