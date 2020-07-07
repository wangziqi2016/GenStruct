
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

bstream_t *bstream_init_local(bstream_t *bstream, void *data, int size) {
  memset(bstream, 0x00, sizeof(bstream_t));
  bstream->local = 1;
  bstream->owner = 0;
  bstream->data = data;
  bstream->size = size;
  return bstream;
}

void bstream_free(bstream_t *bstream) {
  if(bstream->local == 1) error_exit("Could not free local bstream instance\n");
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

// Returns actual number of bits written; Report error if write beyond EOS and the write error flag is on
void bstream_write(bstream_t *bstream, void *p, int bits) {
  // Create a local object as wrapper
  bstream src_, *src = &src_;
  // Size of the local buffer is rounded up to the nearest 8 byte, since it is guaranteed that we at least have
  // an entire byte even if "bits" is not multiple of 8
  bstream_init_local(src, p, (bits + 7) / 8);
  while(1) {
    int rem = bstream_get_rem(bstream);
    int copy_bits = bits;
    if(rem < bits) {
      if(bstream->read_cb == NULL) {
        error_exit("Bits remaining (%d) smaller than write amount (%d)\n", rem, bits);
      }
      // Only copy to the rest of the buffer, call the write cb, and loop back
      copy_bits = rem;
    }
    bstream_copy(bstream, src, copy_bits);
    bits -= copy_bits;
    if(bits != 0) {
      // This function is expected to empty the buffer and reset it
      bstream->write_cb(bstream);
    } else {
      break;
    }
  }
  return;
}

// Low-level function. There will not be out-of-range read or write
// Copy from current pos from src to current pos to dest; Guaranteed copy "bits" bits
// This function will move dest and src pointers
void bstream_copy(bstream_t *dest, bstream_t *src, int bits) {
  assert(bits <= bstream_get_rem(dest));
  assert(bits <= bstream_get_rem(src));
  while(bits != 0) {
    int dest_rem = bstream_get_byte_rem(dest);
    int src_rem = bstream_get_byte_rem(dest);
    int copy_bits = (dest_rem < src_rem) ? dest_rem : src_rem;
    if(copy_bits > bits) copy_bits = bits;
    bitcpy8(dest->data + dest->byte_pos, src->data + src->byte_pos, dest->bit_pos, src->bit_pos, copy_bits);
    bstream_advance(dest, copy_bits);
    bstream_advance(src, copy_bits);
    bits -= copy_bits;
    assert(bits >= 0);
  }
  return;
}

void bstream_print(bstream_t *bstream) {
  printf("[bstream] size %d pos byte %d bit %d own %d\n", 
    bstream->size, bstream->byte_pos, bstream->bit_pos, bstream->owner);
  return;
}
