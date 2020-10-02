
#ifndef _PQ_H
#define _PQ_H

#include "common.h"

// Initially allocate 128-element array
#define PQ_INIT_SIZE       128

// Less than function for comparing elements in the queue; void * are general pointers pointing to the element
// Returns 1 if a < b; 0 otherwise
typedef int (*pq_less_cb_t)(void *a, void *b);



#endif