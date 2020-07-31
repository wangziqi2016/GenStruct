
#ifndef _BTREE_H
#define _BTREE_H

#define BTREE_INNER_CAPACITY 64
#define BTREE_LEAF_CAPACITY  64

typedef struct {
  int type;
  int count;
  void *key[BTREE_INNER_CAPACITY];
  void *value[BTREE_INNER_CAPACITY];
} btree_inner_t;

typedef struct {
  int type;
  int count;
  void *key[BTREE_LEAF_CAPACITY];
  void *value[BTREE_LEAF_CAPACITY];
} btree_leaf_t;

typedef struct {

} btree_t;

#endif