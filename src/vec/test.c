
#include "vec.h"

void test_init() {
  TEST_BEGIN();
  vec_t *vec = vec_init_capacity(1);
  assert(vec_get_count(vec) == 0);
  assert(vec_get_capacity(vec) == 1);
  vec_free(vec);
  vec = vec_init();
  assert(vec_get_count(vec) == 0);
  assert(vec_get_capacity(vec) == VEC_INIT_CAPACITY);
  vec_free(vec);
  TEST_PASS();
  return;
}

int main() {
  test_init();
  return 0;
}