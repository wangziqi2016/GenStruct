
#ifndef _BSTREAM_H
#define _BSTREAM_H

#include <stdint.h>
#include <string.h>
#include "common.h"

//* bstream_t - Bit stream data structure. 
//  Note that bstream does not support random access. Only streaming reads or wrires are supported

#define BSTREAM_INIT_SIZE 256

struct bstream_struct_t;

typedef void (*bstream_rw_cb_t)(struct bstream_struct_t *bstream);

typedef struct bstream_struct_t {
  uint8_t *data;         // Buffer data
  int owner;             // Whether the object owns the buffer
  int local;             // Set if local; We could not free local
  int size;              // Number of bytes in the data buffer
  // Shared for reads and writes
  int bit_pos;        // Bit offset
  int byte_pos;       // Byte offset
  // Read/write call backs; If NULL just report error
  bstream_rw_cb_t read_cb;  // Called when read request reaches end of buffer
  bstream_rw_cb_t write_cb; // Called when write request reaches end of buffer
} bstream_t;

bstream_t *bstream_init();              // Initialize with default size
bstream_t *bstream_init_size(int size); // Initialize with given size
bstream_t *bstream_init_from(void *data, int size); // Ownership always transferred without copying
bstream_t *bstream_init_copy(void *data, int size); // Ownership transferred by copying
bstream_t *bstream_init_local(bstream_t *bstream, void *data, int size); // Local var on the stack
void bstream_free(bstream_t *bstream);
void bstream_realloc(bstream_t *bstream, int size); // Change the size of data array, can expend or shrink

void bstream_advance(bstream_t *bstream, int bits); // Advance from the current pos by given bits

// Read and write call backs; If not set default to NULL
inline static void bstream_set_read_cb(bstream_t *bstream, bstream_rw_cb_t read_cb) { bstream->read_cb = read_cb; }
inline static void bstream_set_write_cb(bstream_t *bstream, bstream_rw_cb_t write_cb) { bstream->write_cb = write_cb; }

// Returns number of bits
inline static int bstream_get_byte_pos(bstream_t *bstream) { return bstream->byte_pos; }
inline static int bstream_get_bit_pos(bstream_t *bstream) { return bstream->bit_pos; }
inline static int bstream_get_pos(bstream_t *bstream) { return bstream->byte_pos * 8 + bstream->bit_pos; }
inline static int bstream_get_eos_pos(bstream_t *bstream) { return bstream->size * 8; }
// Total number of bits remaining in the buffer
inline static int bstream_get_rem(bstream_t *bstream) { return bstream_get_eos_pos(bstream) - bstream_get_pos(bstream); }
// Number of bits remaining in the current byte
inline static int bstream_get_byte_rem(bstream_t *bstream) { return 8 - bstream->bit_pos; }
// Return current bit location
inline static int bstream_get_bit(bstream_t *bstream) { 
  return bit8_test(bstream->data[bstream->byte_pos], bstream->bit_pos); 
}
inline static void bstream_reset(bstream_t *bstream) { bstream->byte_pos = bstream->bit_pos = 0; }

void bstream_copy(bstream_t *dest, bstream_t *src, int bits);
void bstream_write(bstream_t *bstream, void *p, int bits);

void bstream_print(bstream_t *bstream);

#endif