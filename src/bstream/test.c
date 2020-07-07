
#include <time.h>
#include "bstream.h"

void test_advance() {
  TEST_BEGIN();
  bstream_t *bstream = bstream_init_size(32); // Buffer size is 32 bytes
  bstream_advance(bstream, 0); // Corner case
  assert(bstream_get_byte_pos(bstream) == 0 && bstream_get_bit_pos(bstream) == 0);
  bstream_advance(bstream, 8); // Single byte
  assert(bstream_get_byte_pos(bstream) == 1 && bstream_get_bit_pos(bstream) == 0);
  bstream_advance(bstream, 64); // Multiple bytes
  assert(bstream_get_byte_pos(bstream) == 9 && bstream_get_bit_pos(bstream) == 0);
  bstream_advance(bstream, 5);  // Not crossing
  assert(bstream_get_byte_pos(bstream) == 9 && bstream_get_bit_pos(bstream) == 5);
  bstream_advance(bstream, 3);  // Stop at boundary
  assert(bstream_get_byte_pos(bstream) == 10 && bstream_get_bit_pos(bstream) == 0);
  bstream_advance(bstream, 10);  // Crossing next boundary
  assert(bstream_get_byte_pos(bstream) == 11 && bstream_get_bit_pos(bstream) == 2);
  bstream_advance(bstream, 10);  // Noe crossing next boundary after byte advance
  assert(bstream_get_byte_pos(bstream) == 12 && bstream_get_bit_pos(bstream) == 4);
  bstream_advance(bstream, 13);  // Crossing next boundary after byte advance
  assert(bstream_get_byte_pos(bstream) == 14 && bstream_get_bit_pos(bstream) == 1);
  bstream_advance(bstream, 18 * 8 - 1);  // To the very end
  assert(bstream_get_byte_pos(bstream) == 32 && bstream_get_bit_pos(bstream) == 0);
  // This is the pos of the very end of the buffer
  assert(bstream_get_rem(bstream) == 0);
  assert(bstream_get_pos(bstream) == 32 * 8);
  bstream_free(bstream);
  TEST_PASS();
  return;
}

void test_write_no_cb() {
  TEST_BEGIN();
  const int count = 4096;
  // Make sure it never overflows
  bstream_t *bstream = bstream_init_size(count);
  // Generate random numbers and bit lengths
  uint64_t values[4096];
  int bits[4096];
  srand(time(NULL));
  for(int i = 0;i < count;i++) {
    values[i] = ((uint64_t)rand() << 32) | (uint64_t)rand();
    bits[i] = rand() % 65; // Can be 0 - 64 bits
    bstream_write(bstream, values + i, bits[i]);
  }
  int pos = 0;
  // Reset read head
  bstream_reset(bstream);
  for(int i = 0;i < count;i++) {
    for(int j = 0;j < bits[i];j++) {
      // The j-th bit of the word must equal current location on bstream
      assert(bit64_test(values, j) == bstream_get_bit(bstream));
      bstream_advance(bstream, 1);
    }
  }
  bstream_free(bstream);
  TEST_PASS();
  return;
}

int main() {
  printf("========== main ==========\n");
  test_advance();
  test_write_no_cb();
  printf("==========================\n");
  return 0;
}