#include "bucketalloc.h"
#include "../memory/vmm.h"


void *kmalloc_init(size_t size) {
  memory_bucket_t *bucket = create_bucket(size, 0);

  return (void *)bucket;
}

memory_bucket_t *create_bucket(size_t size, uint32_t flags) {
  memory_pebble_t *first;

  if (size < MIN_INITIAL_ALLOC) {
    size = MIN_INITIAL_ALLOC;
  }

  size = (size + (PAGESIZE - 1) & ~(PAGESIZE-1));//Ensure alloc size is a multiple of pagesize.
  
  memory_bucket_t *bucket = (memory_bucket_t *) mmap(size / PAGESIZE);

  if (bucket != NULL) {
    bucket->magic = KMALLOC_MAGIC_BUCKET;
    bucket->lflags = (flags << 8) | BUCKET_FLAG_FIRST;
    bucket->size = size/PAGESIZE;//Number of pages assigned to bucket.
    bucket->largest_free = size - sizeof(memory_bucket_t) - sizeof(memory_pebble_t);
    bucket->prev = NULL;
    bucket->next = NULL;

    
    bucket->spinlock = 0;
      
    //First element will be place just after the bucket header.
    first = (memory_pebble_t *)((uint8_t *) bucket + sizeof(memory_bucket_t));
    bucket->first = first;

    first->magic = KMALLOC_MAGIC_PEBBLE;
    first->lflags = 0 | PEBBLE_FLAG_FREE;
    first->alignment = 1;//TODO: implement alignment.
    first->size = bucket->largest_free;

    first->parent = bucket;
    first->prev = NULL;
    first->next = NULL;
  }

  return bucket;
}

