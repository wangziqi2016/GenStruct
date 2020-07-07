
#ifndef _BSTREAM_H
#define _BSTREAM_H

#include <stdint.h>
#include <string.h>
#include "common.h"

//* bstream_t - Bit stream data structure. 
//  Note that bstream does not support random access. Only streaming reads or wrires are supported

#define BSTREAM_INIT_SIZE 256

typedef struct {
  uint8_t *data;         // Buffer data
  int owner;             // Whether the object owns the buffer
  int size;              // Number of bytes in the data buffer
  // Shared for reads and writes
  int bit_pos;        // Bit offset
  int byte_pos;       // Byte offset
} bstream_t;

bstream_t *bstream_init();              // Initialize with default size
bstream_t *bstream_init_size(int size); // Initialize with given size
bstream_t *bstream_init_from(void *data, int size); // Ownership always transferred without copying
bstream_t *bstream_init_copy(void *data, int size); // Ownership transferred by copying
void bstream_free(bstream_t *bstream);
void bstream_realloc(bstream_t *bstream, int size); // Change the size of data array, can expend or shrink

void bstream_advance(bstream_t *bstream, int bits); // Advance from the current pos by given bits



// Returns number of bits
inline static int bstream_get_byte_pos(bstream_t *bstream) { return bstream->byte_pos; }
inline static int bstream_get_bit_pos(bstream_t *bstream) { return bstream->bit_pos; }
inline static int bstream_get_pos(bstream_t *bstream) { return bstream->byte_pos * 8 + bstream->bit_pos; }
inline static int bstream_get_eos_pos(bstream_t *bstream) { return bstream->size * 8; }
// Total number of bits remaining in the buffer
inline static int bstream_get_rem(bstream_t *bstream) { return bstream_get_eos_pos(bstream) - bstream_get_pos(bstream); }
// Number of bits remaining in the current byte
inline static int bstream_get_byte_rem(bstream_t *bstream) { return 8 - bstream->bit_pos; }

void bstream_plan(bstream_t *bstream, int bits, int *head_bits, int *mid_bytes, int *tail_bits);
void bstream_print(bstream_t *bstream);

#endif