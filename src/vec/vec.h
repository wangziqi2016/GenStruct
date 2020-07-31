
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
void vec_realloc(vec_t *vec, int new_size); // Reallocate the storage while keeping the content (truncate if less)
// Returns address of elements
inline static void **vec_addr(vec_t *vec, int index) {
  assert(index >= 0 && index < vec->count);
  return vec->data + index;
}
inline static void *vec_at(vec_t *vec, int index) { return *vec_addr(vec, index); }

void vec_push(vec_t *vec, void *value);
void *vec_pop(vec_t *vec);
// The following are just short hands
inline static void vec_push_u64(vec_t *vec, uint64_t value) { vec_push(vec, (void *)value); }
inline static uint64_t vec_pop_u64(vec_t *vec) { return (uint64_t)vec_pop(vec); }
inline static void vec_push_32(vec_t *vec, int32_t value) { vec_push(vec, (void *)(uint64_t)value); }
inline static int32_t vec_pop_32(vec_t *vec) { return (int32_t)(uint64_t)vec_pop(vec); }

// Get functions
inline static int vec_get_count(vec_t *vec) { return vec->count; }
inline static int vec_get_capacity(vec_t *vec) { return vec->capacity; }
inline static void **vec_get_data(vec_t *vec) { return vec->data; }

// Iterator
typedef struct {
  vec_t *vec;
  int index;
} vec_it_t;

vec_it_t vec_it_begin(vec_t *vec) { vec_it_t it = {vec, 0}; return it; }
int vec_it_isend(vec_it_t *it) { return it->index == it->vec->count; }
void vec_it_next(vec_it_t *it) { assert(it->index >= 0 && it->index < it->vec->count); it->index++; }
void *vec_it_value(vec_it_t *it) { return it->vec->data[it->index]; }

#endif
