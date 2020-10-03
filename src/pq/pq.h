
#ifndef _PQ_H
#define _PQ_H

#include "common.h"

// Initially allocate 128-element array
#define PQ_INIT_SIZE       128
#define PQ_ROOT_INDEX      0

// Less than function for comparing elements in the queue; void * are general pointers pointing to the element
// Returns non-zero if a < b; 0 otherwise
// Users should define their own pq_less_cb either in the header or in the source
int (*pq_less_cb_t)(void *a, void *b);

typedef struct {
  void **data;             // An array that might be reallocated
  int size;                // Current number of elements in the array
  int capacity;            // Maximim number of elements in the array
  pq_less_cb_t less_cb;    // Call back function
} pq_t;

pq_t *pq_init_size(int size);
pq_t *pq_init();
void pq_free(pq_t *pq);
void pq_free_all(pq_t *pq, void (*data_free)(void *)); // Also free elements

inline static int pq_lchild_index(pq_t *pq, int index) { return index * 2 + 1; (void)pq; }
inline static int pq_rchild_index(pq_t *pq, int index) { return index * 2 + 2; (void)pq; }
inline static int pq_parent_index(pq_t *pq, int index) { return (index - 1) / 2; (void)pq; }

inline static void *pq_lchild(pq_t *pq, int index) { return pq->data[pq_lchild_index(pq, index)]; }
inline static void *pq_rchild(pq_t *pq, int index) { return pq->data[pq_rchild_index(pq, index)]; }
inline static void *pq_parent(pq_t *pq, int index) { return pq->data[pq_parent_index(pq, index)]; }

inline static void pq_swap(pq_t *pq, int index1, int index2) { 
  void *t = pq->data[index1];
  pq->data[index1] = pq->data[index2];
  pq->data[index2] = t;
  return;
}

// Percolate up, compare with parent and swap if necessary, repeat until parent larger or at root
void pq_up(pq_t *pq, int index);
// Percolate down
void pq_down(pq_t *pq, int index);


#endif