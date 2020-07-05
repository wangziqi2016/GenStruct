
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
  bstream->data = data;
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