
#include "bstream.h"

bsteram_t *bstream_init() { return bstream_init_size(BSTREAM_INIT_SIZE); }
bstream_t *bstream_init_size(int size) {
  bstream_t *bstream = (bstream_t *)malloc(sizeof(bstream_t));
  SYSEXPECT(bstream != NULL);
  memset(bstream, 0x00, sizeof(bstream_t));
  bstream->size = size;
  bstream->data = (char *)malloc(size);
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

  bstream->owner = 1;
  return bstream;
}

void bstream_free(bstream_t *bstream);