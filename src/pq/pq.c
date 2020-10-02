
#include "common.h"
#include "pq.h"

pq_t *pq_init_size(int size) {
  pq_t *pq = (pq_t *)malloc(sizeof(pq_t));
  SYSEXPECT(pq != NULL);
  memset(pq, 0x00, sizeof(pq_t));
  pq->data = (void **)malloc(sizeof(void *) * size);
  SYSEXPECT(pq->data != NULL);
  memset(pq->data, 0x00, sizeof(void *) * size);
  return pq;
}

pq_t *pq_init() {
  return pq_init_size(PQ_INIT_SIZE);
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