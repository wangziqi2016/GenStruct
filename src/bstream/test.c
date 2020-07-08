
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
  int file_rem = fp_rem(fp);
  void *copy_start = bstream_get_data(bstream);
  int copy_size = bstream_get_size(bstream);
  if(file_rem == 0) {
    bstream_invalidate(bstream); // Next access will return zero
    return 1;                    // Signal EOS to caller
  } else if(file_rem < bstream_get_size(bstream)) {
    // Adjust read pointer such that the last byte of the file is the last byte in the buffer
    copy_size = file_rem;
    int copy_offset = bstream_get_size(bstream) - file_rem;
    copy_start = bstream_get_data(bstream) + copy_offset;
    bstream_set_bit_pos(bstream, 0);
    bstream_set_byte_pos(bstream, copy_offset);
  }
  // Note that we read at 1 byte granularity. If EOF is seen then only read till that point
  int ret = fread(copy_start, 1, copy_size, fp);
  assert(ret == copy_size); 
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
  // Invalidate such that next read access triggers read call back
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
  int total_rem = 8 - total_bits % 8; // Number of bits we still have in the buffer (wild bits)
  uint64_t dummy;
  // Read a large range but we can only actually read the last few bits
  int dummy_ret = bstream_read(bstream, &dummy, 1000);
  printf("Reading the last few bits... ret = %d (expecting %d)\n", dummy_ret, total_rem);
  assert(dummy_ret == total_rem);
  dummy_ret = bstream_read(bstream, &dummy, 1000);
  printf("Reading invalidated stream 1... ret = %d (expecting 0)\n", dummy_ret);
  dummy_ret = bstream_read(bstream, &dummy, 1000);
  printf("Reading invalidated stream 2... ret = %d (expecting 0)\n", dummy_ret);
  dummy_ret = bstream_read(bstream, &dummy, 1000);
  printf("Reading invalidated stream 3... ret = %d (expecting 0)\n", dummy_ret);
  assert(dummy_ret == 0);
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