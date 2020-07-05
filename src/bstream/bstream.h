
#ifndef _BSTREAM_H
#define _BSTREAM_H

#include <stdint.h>
#include "common.h"

typedef struct {
  uint8_t *data;         // Buffer data
  int owner;             // Whether the object owns the buffer
  int size;              // Number of bytes in the data buffer
  int bit_offset;        // Bit offset
  int byte_offset;       // Byte offset
} bstream_t;

#endif