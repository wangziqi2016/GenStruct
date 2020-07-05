
#ifndef _BSTREAM_H
#define _BSTREAM_H

#include <stdint.h>
#include <string.h>
#include "common.h"

#define BSTREAM_INIT_SIZE 256

typedef struct {
  uint8_t *data;         // Buffer data
  int owner;             // Whether the object owns the buffer
  int size;              // Number of bytes in the data buffer
  // Shared for reads and writes
  int bit_offset;        // Bit offset
  int byte_offset;       // Byte offset
} bstream_t;

bstream_t *bstream_init();              // Initialize with default size
bstream_t *bstream_init_size(int size); // Initialize with given size
bstream_t *bstream_init_from(void *data, int size); // Ownership always transferred without copying
bstream_t *bstream_init_copy(void *data, int size); // Ownership transferred by copying
void bstream_free(bstream_t *bstream);

void bstream_realloc(bstream_t *bstream, int size); // Change the size of data array, can expend or shrink

#endif