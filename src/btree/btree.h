
#ifndef _BTREE_H
#define _BTREE_H

#include "common.h"

#define BTREE_INNER_CAPACITY 64
#define BTREE_LEAF_CAPACITY  64

#define BTREE_NODE_INNER     0
#define BTREE_NODE_LEAF      1

typedef struct {
  void *key;
  void *value;
} btree_kv_t;

typedef struct btree_node_struct_t {
  int type;
  int count;
  struct btree_node_struct_t *next; // We use the b-link design
  btree_kv_t kv[0];
} btree_node_t;

// Call back interface
typedef int (*btree_key_cmp_func_t)(void *key1, void *key2);
typedef int (*btree_node_search_func_t)(btree_node_t *node, void *key, int *index, btree_key_cmp_func_t key_cmp_func);

typedef struct {
  btree_node_t *root;
  btree_key_cmp_func_t key_cmp_func;    // Set to NULL if using uint64_t as key
  btree_node_search_func_t search_func; // Set to either uint64_t search or customized key search
} btree_t;

btree_node_t *btree_node_init(int type);
void btree_node_free(btree_node_t *node); // Recursively frees all nodes
btree_t *btree_init();                    // By default use uint64_t as key type
btree_t *btree_init_custom_key(btree_key_cmp_func_t key_cmp_func);
void btree_free(btree_t *btree);

// Returns the index found, or the next larger element
int btree_node_search_u64(btree_node_t *node, void *key, int *index, btree_key_cmp_func_t key_cmp_func);
int btree_node_search(btree_node_t *node, void *key, int *index, btree_key_cmp_func_t key_cmp_func);

int btree_node_insert(btree_t *btree, btree_node_t *node, void *key, void *value);
btree_node_t *btree_node_split(btree_node_t *node);

// Traverse to the next level, given an inner node
btree_node_t *btree_next_level(btree_t *btree, btree_node_t *const node, void *key);

#endif