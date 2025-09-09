#include "bucketalloc.h"

memory_bucket_t *create_bucket(size_t size, uint32_t flags) {
  memory_bucket_t *first;

  if (size < MIN_INITIAL_ALLOC) {
    size = MIN_INITIAL_ALLOC;
  }

  size = (size + (PAGESIZE - 1) & ~(PAGESIZE-1));//Ensure alloc size is a multiple of pagesize.
  


  return first;
}
