
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

void test_realloc() {
  TEST_BEGIN();
  vec_t *vec = vec_init_capacity(1);
  int new_cap = 100;
  vec_push_32(vec, 123456);
  vec_realloc(vec, new_cap);
  assert(vec_get_count(vec) == 1 && vec_get_capacity(vec) == 100);
  assert(vec_pop_32(vec) == 123456);
  assert(vec_get_count(vec) == 0 && vec_get_capacity(vec) == 100);
  // Write all elements to make sure the memory block is indeed that big
  //void **data = vec_get_data(vec);
  for(int i = 0;i < new_cap;i++) {
    vec_push_32(vec, i);
  }
  vec_realloc(vec, new_cap / 3);
  for(int i = 0;i < new_cap / 3;i++) {
    assert(vec_pop_32(vec) == new_cap / 3 - i - 1);
  }
  vec_free(vec);
  TEST_PASS();
  return;
}

int main() {
  test_init();
  test_realloc();
  return 0;
}