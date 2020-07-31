

#include "vec.h"

vec_t *vec_init() {
  return vec_init_size(VEC_INIT_CAPACITY);
}

vec_t *vec_init_size(int size) {
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