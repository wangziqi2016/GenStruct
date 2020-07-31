

#include "vec.h"

vec_t *vec_init() {
  return vec_init_size(VEC_INIT_CAPACITY);
}

vec_t *vec_init_size(int size) {
  if(size <= 0) error_exit("Initial size of vec_t must be positive (see %d)\n", size);
  vec_t *vec = (vec_t *)malloc(sizeof(vec_t));
  SYSEXPECT(vec != NULL);
  memset(vec, 0x00, sizeof(vec_t));
  vec->data = (void **)malloc(sizeof(void *) * size);
  SYSEXPECT(vec->data != NULL);
  vec->capacity = size;
  return vec;
}

void vec_free(vec_t *vec) {
  free(vec->data);
  free(vec);
  return;
}

void vec_realloc(vec_t *vec, int new_size) {
  if(size <= 0) error_exit("New size of vec_t must be positive (see %d)\n", new_size);
  void **old_data = vec->data;
  vec->capacity = new_size;
  vec->data = (void **)malloc(sizeof(void *) * new_size);
  SYSEXPECT(vec->data != NULL);
  if(new_size < vec->count) vec->count = new_size; // Count also changes since less elements are addressable
  memcpy(vec->data, old_data, sizeof(void *) * vec->count);
  free(old_data);
  return;
}


