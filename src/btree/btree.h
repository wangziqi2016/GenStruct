
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

typedef struct {
  int type;
  int count;
  btree_kv_t kv[0];
} btree_node_t;

typedef struct {
  btree_node_t *root;
} btree_t;

btree_node_t *btree_node_init(int type);

#endif