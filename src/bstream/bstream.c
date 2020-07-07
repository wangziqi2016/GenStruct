
#include "bstream.h"

bstream_t *bstream_init() { return bstream_init_size(BSTREAM_INIT_SIZE); }
bstream_t *bstream_init_size(int size) {
  bstream_t *bstream = (bstream_t *)malloc(sizeof(bstream_t));
  SYSEXPECT(bstream != NULL);
  memset(bstream, 0x00, sizeof(bstream_t));
  bstream->size = size;
  bstream->data = (uint8_t *)malloc(size);
  SYSEXPECT(bstream->data != NULL);
  bstream->owner = 1;
  return bstream;
}

bstream_t *bstream_init_from(void *data, int size) {
  // Allocating 0 byte may return NULL or valid data
  bstream_t *bstream = bstream_init_size(0);
  if(bstream->data != NULL) {
    free(bstream->data);
  }
  bstream->data = (uint8_t *)data;
  bstream->size = size;
  bstream->owner = 0;
  return bstream;
}

bstream_t *bstream_init_copy(void *data, int size) {
  void *p = malloc(size);
  SYSEXPECT(p != NULL);
  memcpy(p, data, size);
  bstream_t *bstream = bstream_init_from(p, size);
  bstream->owner = 1;
  return bstream;
}

void bstream_free(bstream_t *bstream) {
  if(bstream->owner == 1) free(bstream->data);
  free(bstream);
  return;
}

// If the bstream has ownership then simply does a realloc; If not owner then will become owner by copying the content
// Data will be truncated if shirinking, or filled with random pattern if expanding
void bstream_realloc(bstream_t *bstream, int size) {
  if(size <= 0) error_exit("Invalid size argument: %d\n", size);
  uint8_t *new_data = (uint8_t *)malloc(size);
  SYSEXPECT(new_data != NULL);
  int copy_size = size < bstream->size ? size : bstream->size;
  memcpy(new_data, bstream->data, copy_size);
  free(bstream->data);
  bstream->data = new_data;
  bstream->size = size;
  // Always owner after this function
  if(bstream->owner == 0) bstream->owner = 1;
  return;
}

// Report error if advance past end of buffer
void bstream_advance(bstream_t *bstream, int bits) {
  if(bits > bstream_get_rem(bstream)) {
    error_exit("Advance beyond the end of the current buffer (rem %d, bits %d)\n", 
      bstream_get_rem(bstream), bits);
  }
  bstream->byte_pos += (bits / 8);
  int bits_rem = bits % 8;                      // Number of bits remains to be advanced
  int byte_rem = bstream_get_byte_rem(bstream); // Number of bits in the current byte
  if(byte_rem > bits_rem) {
    bstream->bit_pos += bits_rem; // Easy case: Still within the same byte
  } else {
    bstream->byte_pos++;
    bstream->bit_pos = bits_rem - byte_rem; // Hard case: Advance to next byte
  }
  assert(bstream_get_rem(bstream) >= 0);
  assert(bstream->bit_pos >= 0 && bstream->bit_pos < 8);
  return;
}

void bstream_set_read_eos_error(bstream_t *bstream, int value) {
  if(value != 0 && value != 1) error_exit("readd_eos_error can only be 0 or 1 (see %d)\n", value);
  bstream->read_eos_error = value;
  return;
}
void bstream_set_write_eos_error(bstream_t *bstream, int value) {
  if(value != 0 && value != 1) error_exit("readd_eos_error can only be 0 or 1 (see %d)\n", value);
  bstream->write_eos_error = value;
  return;
}

// Computes a plan for read/write bits from/to the current bit offset
// bits - Number of bits to read or write. Must be between 0 and remaining bits.
// head_bits - Output variable. Number of bits in the current byte. Can be zero.
// mid_bytes - Output variable. Number of aligned bytes that can be copied directly
// tail_bits - Output variable. Number of bits in the last byte. Can be zero.
void bstream_plan(bstream_t *bstream, int bits, int *head_bits, int *mid_bytes, int *tail_bits) {
  assert(bits > 0 && bits <= bstream_get_rem(bstream));
  assert(bstream->bit_pos >= 0 && bstream->bit_pos < 8);
  *head_bits = (8 - bstream->bit_pos) & 0x00000007; // Set to 0 if bit_pos is 1
  bits -= *head_bits;
  *mid_bytes = bits / 8;
  *tail_bits = bits % 8;
  assert(*head_bits < 8 && *tail_bits < 8);
  assert(bits == (*head_bits + *tail_bits + *mid_bytes * 8));
  return;
}

// Returns actual number of bits written; Report error if write beyond EOS and the write error flag is on
int bstream_write(bstream_t *bstream, void *p, int bits) {
  int rem = bstream_get_rem(bstream);
  if(rem < bits) {
    if(bstream->write_eos_error == 0) {
      bits = rem;
    } else {
      error_exit("Bits remaining (%d) smaller than write amount (%d)\n", rem, bits);
    }
  }
  const int ret = bits; // Save for return value
  int head_bits, mid_bytes, tail_bits;
  bstream_plan(bstream, bits, &head_bits, &mid_bytes, &tail_bits);
  // Optimization: If aligned then we do a fast memcpy of mid_bytes
  uint8_t *input = (uint8_t *)p;
  if(head_bits == 0) {
    assert(bstream->bit_pos == 0);
    memcpy(bstream->data + bstream->byte_pos, input, mid_bytes);
    bstream->byte_pos += mid_bytes;
    input += mid_bytes;
    // Finally add tail bits
    bitcpy8(bstream->data + bstream->byte_pos, input, 0, 0, tail_bits);
    return ret;
  }
  // In the general case we copy 8 bits from input
  while(bits >= 8) {
    bitcpy8(bstream->data + bstream->byte_pos, input, bstream->bit_pos, 0, 8 - bstream->bit_pos);
    bstream->byte_pos++;
    bitcpy8(bstream->data + bstream->byte_pos, input, 0, 8 - bstream->bit_pos, bstream->bit_pos);
    input++;
    bits -= 8;
  }
  if(bits != 0) {
    if(bits >= 8 - bstream->bit_pos) {
      // Hard case: Cross boundary again
      bitcpy8(bstream->data + bstream->byte_pos, input, bstream->bit_pos, 0, 8 - bstream->bit_pos);
      // After this the stream is at bit pos 0, and input is at bit pos (8 - bstream->bit_pos)
      bstream->byte_pos++;
      bits -= (8 - bstream->bit_pos);
      bitcpy8(bstream->data + bstream->byte_pos, input, 0, 8 - bstream->bit_pos, bits);
      bstream->bit_pos = bits;
    } else {
      // Easy case: Just add the remaining "bits" bits without crossing boundary
      bitcpy8(bstream->data + bstream->byte_pos, input, bstream->bit_pos, 0, bits);
      bstream->bit_pos += bits;
    }
  }
  return ret;
}

// Low-level function. There will not be out-of-range read or write
// Copy from current pos from src to current pos to dest
int bstream_copy(bstream_t *dest, bstream_t *src, int bits) {
  assert(bits <= bstream_get_rem(dest));
  assert(bits <= bstream_get_rem(src));
  while(bits != 0) {

  }
  return 0;
}

void bstream_print(bstream_t *bstream) {
  printf("[bstream] size %d pos byte %d bit %d own %d\n", 
    bstream->size, bstream->byte_pos, bstream->bit_pos, bstream->owner);
  return;
}
