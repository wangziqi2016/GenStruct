
#include "common.h"

typedef struct {
  int count;       // Current number of elements
  int capacity;    // Maximum number of elements
  void *data;      // Data array (could also store uint64_t in-place)
} vec_t;