
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

void test_push_pop() {
  TEST_BEGIN();
  vec_t *vec = vec_init_capacity(1);
  const int count = 5000;
  for(int i = 0;i < count;i++) {
    vec_push_32(vec, i);
  }
  assert(vec_get_count(vec) == count);
  assert(vec_get_capacity(vec) == nextlog2_32(count));
  vec_free(vec);
  TEST_PASS();
  return;
}

void test_addr() {
  TEST_BEGIN();
  vec_t *vec = vec_init_capacity(1);
  const int count = 5000;
  for(int i = 0;i < count;i++) vec_push_32(vec, i);
  for(int i = 0;i < count;i++) {
    int32_t *p = vec_addr_32(vec, i);
    assert(p[0] == i);
    p[0] = i * 2;
    p[1] = i * 2 + 1;
  }
  // Read as 64 bit
  for(int i = 0;i < count;i++) {
    uint64_t value = *vec_addr_u64(vec, i);
    assert((int)(value & 0xFFFFFFFFUL) == i * 2);
    assert((int)((value >> 32) & 0xFFFFFFFFUL) == i * 2 + 1);
  }
  vec_free(vec);
  TEST_PASS();
  return;
}

int main() {
  test_init();
  test_realloc();
  test_push_pop(); // This also tests automatic realloc for push()
  test_addr();
  return 0;
}