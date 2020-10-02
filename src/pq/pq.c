
#include "common.h"
#include "pq.h"

pq_t *pq_init_size(int size) {
  pt_t *pq = (pq_t *)malloc(sizeof(pq_t));
  SYSEXPECT(pq != NULL);
  memset(pq, 0x00, sizeof(pq_t));
  pq->data = (void **)malloc(sizeof(void *) * size);
  SYSEXPECT(pq->data != NULL);
  memset(pq->data, 0x00, sizeof(void *) * size);
  return pq;
}

pq_t *pq_init();
void pq_free(pq_t *pq);