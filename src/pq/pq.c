
#include "common.h"
#include "pq.h"

pq_t *pq_init_size(pq_less_cb_t less_cb, int size) {
  pq_t *pq = (pq_t *)malloc(sizeof(pq_t));
  SYSEXPECT(pq != NULL);
  memset(pq, 0x00, sizeof(pq_t));
  pq->data = (void **)malloc(sizeof(void *) * size);
  pq->less_cb = less_cb;
  pq->capacity = size;
  SYSEXPECT(pq->data != NULL);
  memset(pq->data, 0x00, sizeof(void *) * size);
  return pq;
}

pq_t *pq_init(pq_less_cb_t less_cb) {
  return pq_init_size(less_cb, PQ_INIT_SIZE);
}

void pq_free(pq_t *pq) {
  free(pq->data);
  free(pq);
  return;
}

void pq_free_all(pq_t *pq, void (*data_free)(void *)) {
  for(int i = 0;i < pq->size;i++) {
    data_free(pq->data[i]);
  }
  pq_free(pq);
  return;
}

// Move the current index element up the heap until it is at root level or the parent is no larger
void pq_up(pq_t *pq, int index) {
  assert(index >= 0 && index < pq->size);
  if(index == PQ_ROOT_INDEX) {
    return;
  }
  // This value does not change since we always use the same element for comparison
  void *const curr = pq->data[index];
  while(pq->less_cb(curr, pq_parent(pq, index))) {
    int parent_index = pq_parent_index(pq, index);
    pq_swap(pq, index, parent_index);
    index = parent_index;
  }
  return;
}

// Move the element down by comparing it with one or two child, or stop if there is none
void pq_down(pq_t *pq, int index) {
  assert(index >= 0 && index < pq->size);
  void *const curr = pq->data[index];
  while(1) {
    // Terminating condition #1: Exit when current element is at the last level
    if(pq_has_child(pq, index) == 0) {
      break;
    }
    // If there is right child then pick the smaller one; Otherwise only pick left child
    int less_child_index = pq_lchild_index(pq, index);
    if(pq_has_rchild(pq, index) == 1) {
      if(pq->less_cb(pq_rchild(pq, index), pq_lchild(pq, index))) {
        less_child_index = pq_rchild_index(pq, index);
      }
    }
    if(pq->less_cb(pq->data[less_child_index], curr)) {
      pq_swap(pq, index, less_child_index);
    } else {
      // Terminate condition #2: Exit loop when both elements are larger
      break;
    }
    // This is the current index of the element under consideration
    index = less_child_index;
  }
  return;
}

// This function only guarantees that the after size is no smaller than new size
void pq_realloc(pq_t *pq, int new_size) {
  assert(pq->size >= 0 && pq->size <= pq->capacity); // == capacity is fine since we need full state
  if(new_size <= pq->capacity) return;
  void **new_data = (void **)malloc(sizeof(void *) * new_size);
  SYSEXPECT(new_data != NULL);
#ifndef NDEBUG
  memset(new_data, 0x00, sizeof(void *) * new_size);
#endif
  void **old_data = pq->data;
  memcpy(new_data, old_data, sizeof(void *) * pq->size);
  pq->data = new_data;
  pq->capacity = new_size;
  free(old_data);
  return;
}

// Insert an element at the end and pq_up
void pq_push(pq_t *pq, void *entry) {
  assert(pq->size >= 0 && pq->size <= pq->capacity);
  if(pq->size == pq->capacity) {
    pq_realloc(pq, pq->capacity * 2);
  }
  assert(pq->size < pq->capacity);
  // The index of the newly inserted element
  int index = pq->size;
  pq->data[index] = entry;
  pq->size++;
  pq_up(pq, index);
  return;
}

void *pq_pop(pq_t *pq) {
  // Special case: empty queue and single element queue, both do not reorganization
  if(pq->size == 0) return NULL;
  else if(pq->size == 1) {
    pq->size = 0;
    return pq->data[0];
  }
  void *ret = pq->data[0];
  // One element was removed; This var now points to the last element in the array
  pq->size--;
  // Move the last element to the root, and percolate down
  pq_swap(pq, pq->size, 0);
#ifndef NDEBUG
  pq->data[pq->size] = NULL;
#endif
  pq_down(pq, 0);
  return ret;
}