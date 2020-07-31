
#ifndef _VEC_H
#define _VEC_H

#include "common.h"

#define VEC_INIT_CAPACITY   16

typedef struct {
  int count;       // Current number of elements
  int capacity;    // Maximum number of elements
  void **data;     // Data array (could also store uint64_t in-place)
} vec_t;

vec_t *vec_init();              // Initialize using default capacity
vec_t *vec_init_size(int size); // Better control over initial capacity
void vec_free(vec_t *vec);

#endif
