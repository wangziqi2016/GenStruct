
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
  assert(bstream_get_byte_pos(bstream) == 13 && bstream_get_bit_pos(bstream) == 1);
  bstream_advance(bstream, 19 * 8 - 1);  // To the very end
  assert(bstream_get_byte_pos(bstream) == 33 && bstream_get_bit_pos(bstream) == 0);
  // This is the pos of the very end of the buffer
  assert(bstream_get_rem(bstream) == 0);
  assert(bstream_get_pos(bstream) == 32 * 8);
  TEST_PASS();
  return;
}

int main() {
  printf("========== main ==========\n");
  test_advance();
  printf("==========================\n");
  return 0;
}