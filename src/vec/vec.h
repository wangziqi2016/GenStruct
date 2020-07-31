
#ifndef _VEC_H
#define _VEC_H

#include "common.h"

#define VEC_INIT_CAPACITY   16

typedef struct {
  int count;       // Current number of elements
  int capacity;    // Maximum number of elements
  void **data;     // Data array (could also store uint64_t in-place)
} vec_t;

vec_t *vec_init();                      // Initialize using default capacity
vec_t *vec_init_capacity(int capacity); // Better control over initial capacity
void vec_free(vec_t *vec);
void vec_realloc(vec_t *vec, int new_capacity); // Reallocate the storage while keeping the content (truncate if less)
// Returns address of elements
inline static void **vec_addr(vec_t *vec, int index) {
  assert(index >= 0 && index < vec->count);
  return vec->data + index;
}
inline static uint64_t *vec_addr_u64(vec_t *vec, int index) { return (uint64_t *)vec_addr(vec, index); }
inline static int32_t *vec_addr_32(vec_t *vec, int index) { return (int32_t *)vec_addr(vec, index); }

inline static void *vec_at(vec_t *vec, int index) { assert(index >= 0 && index < vec->count); return *vec_addr(vec, index); }
inline static uint64_t vec_at_u64(vec_t *vec, int index) { return (uint64_t)vec_at(vec, index); }
inline static int32_t vec_at_32(vec_t *vec, int index) { return (int32_t)(uint64_t)vec_at(vec, index); }

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

// Set functions
void vec_set_count(vec_t *vec, int new_count); // Set a count for random access; Data might be uninitialized

// Iterator
typedef struct {
  vec_t *vec;
  int index;
} vec_it_t;

inline static vec_it_t vec_it_begin(vec_t *vec) { vec_it_t it = {vec, 0}; return it; }
inline static int vec_it_isend(vec_it_t *it) { return it->index == it->vec->count; }
inline static void vec_it_next(vec_it_t *it) { assert(it->index >= 0 && it->index < it->vec->count); it->index++; }
inline static void *vec_it_value(vec_it_t *it) { return it->vec->data[it->index]; }
inline static uint64_t vec_it_value_u64(vec_it_t *it) { return (uint64_t)vec_it_value(it); }
inline static int32_t vec_it_value_32(vec_it_t *it) { return (int32_t)(uint64_t)vec_it_value(it); }
inline static void **vec_it_addr(vec_it_t *it) { return &it->vec->data[it->index]; } // Returns the address of the value
inline static uint64_t *vec_it_addr_u64(vec_it_t *it) { return (uint64_t *)&it->vec->data[it->index]; }
inline static int32_t *vec_it_addr_32(vec_it_t *it) { return (int32_t *)&it->vec->data[it->index]; }

#endif
