
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
  bstream_t *bstream = bstream_init_size(count * sizeof(uint64_t));
  // Generate random numbers and bit lengths
  uint64_t values[4096];
  int bits[4096];
  srand(time(NULL));
  int total_bits = 0;
  for(int i = 0;i < count;i++) {
    values[i] = ((uint64_t)rand() << 32) | (uint64_t)rand();
    bits[i] = rand() % 65; // Can be 0 - 64 bits
    bstream_write(bstream, values + i, bits[i]);
    total_bits += bits[i];
  }
  printf("Written bits %d; Pos %d rem %d\n", total_bits, bstream_get_pos(bstream), bstream_get_rem(bstream));
  // Reset read head
  bstream_reset(bstream);
  for(int i = 0;i < count;i++) {
    for(int j = 0;j < bits[i];j++) {
      // The j-th bit of the word must equal current location on bstream
      assert(bit64_test(values[i], j) == bstream_get_bit(bstream));
      bstream_advance(bstream, 1);
    }
  }
  // Test whole copy using bstream copy
  bstream_t *bstream2 = bstream_init_size(count * sizeof(uint64_t));
  bstream_reset(bstream);
  bstream_copy(bstream2, bstream, total_bits);
  // Reset both to compare
  bstream_reset(bstream);
  bstream_reset(bstream2);
  for(int i = 0;i < total_bits;i++) {
    assert(bstream_get_bit(bstream) == bstream_get_bit(bstream2));
  }
  bstream_free(bstream2);
  bstream_free(bstream);
  TEST_PASS();
  return;
}

static int test_write_cb_write_cb(bstream_t *bstream) {
  FILE *fp = (FILE *)bstream_get_arg(bstream);
  //printf("byte %d bit %d\n", bstream->byte_pos, bstream->bit_pos);
  // Write pos_byte such that even if the buf is not full we can still write correct number
  int ret = fwrite(bstream_get_data(bstream), bstream_get_pos_byte(bstream), 1, fp);
  assert(ret == 1 || ret == 0);
  // Reset for further write
  bstream_reset(bstream);
  return 0;
}

static int test_write_cb_read_cb(bstream_t *bstream) {
  FILE *fp = (FILE *)bstream_get_arg(bstream);
  bstream_reset(bstream);
  // Note that we read at 1 byte granularity. If EOF is seen then only read till that point
  int ret = fread(bstream_get_data(bstream), 1, bstream_get_size(bstream), fp);
  // Terminate read or shift the data to the end of the buffer to trigger a zero read next time and read will return 0
  if(ret < bstream_get_size(bstream)) {
    int byte_pos = bstream_get_size(bstream) - ret;
    memmove(bstream_get_data(bstream) + byte_pos, bstream_get_data(bstream), ret);
    bstream->byte_pos = byte_pos;
  } else if(ret == 0) {
    bstream_invalidate(bstream); // Next access triggers end
    return 1;
  }
  return 0;
}

void test_write_cb() {
  TEST_BEGIN();
  const int count = 4096;
  const char *filename = "test_write_cb.test";
  // Use smaller buffer to trigger read and write cb
  bstream_t *bstream = bstream_init_size(256); 
  FILE *fp = fopen(filename, "wb");
  bstream_set_read_cb(bstream, test_write_cb_read_cb);
  bstream_set_write_cb(bstream, test_write_cb_write_cb);
  bstream_set_arg(bstream, fp);
  // Generate random numbers and bit lengths
  uint64_t values[4096];
  int bits[4096];
  srand(time(NULL));
  int total_bits = 0;
  for(int i = 0;i < count;i++) {
    values[i] = ((uint64_t)rand() << 32) | (uint64_t)rand();
    bits[i] = rand() % 65; // Can be 0 - 64 bits
    bstream_write(bstream, values + i, bits[i]);
    total_bits += bits[i];
  }
  // Write back the rest data
  if(bstream_get_pos(bstream) != 0) test_write_cb_write_cb(bstream);
  printf("Written bits %d; Pos %d rem %d\n", total_bits, bstream_get_pos(bstream), bstream_get_rem(bstream));
  fclose(fp);
  fp = fopen(filename, "rb");
  bstream_set_arg(bstream, fp);
  // Invalidate such that next read access triggers
  bstream_invalidate(bstream);
  for(int i = 0;i < count;i++) {
    uint64_t value; // Read into this var from the stream
    int ret = bstream_read(bstream, &value, bits[i]);
    assert(ret == bits[i]);
    for(int j = 0;j < bits[i];j++) {
      // The j-th bit of the word must equal current location on bstream
      assert(bit64_test(values[i], j) == bit64_test(value, j));
    }
  }
  // Must be already at the end
  //assert(bstream_read(bstream, NULL, 1000) == 0);
  fclose(fp);
  bstream_free(bstream);
  TEST_PASS();
  return;
}

int main() {
  printf("========== main ==========\n");
  test_advance();
  test_write_no_cb();
  test_write_cb();
  printf("==========================\n");
  return 0;
}