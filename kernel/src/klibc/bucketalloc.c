#include "bucketalloc.h"
#include "../memory/vmm.h"
#include "spinlock.h"
#include "../memory/heap.h"
#include <stddef.h>
#include <stdint.h>
#include "./string/printf.h"

extern void *memcpy(void *restrict, const void *restrict src, size_t n);
extern void *memset(void *s, int c, size_t n);

void *kmalloc_init(size_t size) {
  serial_printf("\n kmalloc init start.");
  memory_bucket_t *bucket = create_bucket(size, 0 | BUCKET_FLAG_FIRST);

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

void insert_bucket(memory_bucket_t *bucket, void *destination) {
  memory_bucket_t *dest = (memory_bucket_t *)destination;

  if(bucket && dest) {
    acquire_spinlock(&dest->spinlock);

    UPDATE_NODE(bucket, dest);

    release_spinlock(&dest->spinlock);
  }
}


void remove_bucket(memory_bucket_t *bucket) {
  //Don't free initial bucket.
  if (bucket && (bucket != (void*)KHEAPSTART)) {
    if (bucket->prev) {
      bucket->prev->next = bucket->next;
    }
    if (bucket->next) {
      bucket->next->prev = bucket->prev;
    }

    //TODO: ensure safety of this function.
    munmap(bucket, bucket->size);  
  }
}

size_t bucket_update_largest(memory_bucket_t *bucket) {
  memory_pebble_t *p = bucket->first;
  size_t ret = 0;

  while(p != NULL) {
    if(p->size > ret)
      ret = p->size;

    p = p->next;
  }

  bucket->largest_free = ret;
  return ret;
}

memory_pebble_t *place_pebble(memory_bucket_t *bucket, memory_pebble_t *pebble) {
  memory_pebble_t *start = bucket->first;
  memory_pebble_t *best = NULL;
  memory_pebble_t *ret = NULL;

  size_t needed_size, best_size = -1;

  //TODO implement alignment.
  needed_size = pebble->size;

  //BEST_FIT method: check all pebbles and find the smallest sized pebble
  // that can fit this pebble in.
  if(bucket->lflags & BUCKET_FLAG_BEST) {
    while (start != NULL) {
      if (IS_PEBBLE_FREE(start) && (start->size >= needed_size)) {
        if (start->size < best_size) {
          best = start;
          best_size = start->size;
        }
      }

      start = start->next; 
    }

    if (best != NULL) {
      best = split_pebble(best, pebble);
      best->lflags = pebble->lflags;

    ret = best;
    }
  } else {
    //FIRST_FOUND method.
    while (start != NULL) {
      if (IS_PEBBLE_FREE(start) && (start->size >= needed_size)) {
        start = split_pebble(start, pebble);
        start->lflags = pebble->lflags;

        ret = start;
        break;
      }

      start = start->next;
    }
  }

  return ret;
}

memory_pebble_t *split_pebble(memory_pebble_t *this_pebble, memory_pebble_t *src) {
  memory_pebble_t *dummy, *new_pebble;
  size_t new_size, org_size;
  void *pos;

  //TODO add alignment.
  if (SPLIT_PEBBLE(this_pebble->size, src->size)) {
    new_size = (src->size + (PEBBLE_MIN_ALIGN - 1) &~(PEBBLE_MIN_ALIGN - 1));
    new_pebble = (memory_pebble_t *)((uint8_t *)this_pebble + sizeof(memory_pebble_t) + new_size);
    memcpy(new_pebble, this_pebble, sizeof(memory_pebble_t));

    new_pebble->size = this_pebble->size - new_size - sizeof(memory_pebble_t);
    new_pebble->alignment = 1;
    new_pebble->prev = this_pebble;
    if (this_pebble->next)
      this_pebble->next->prev = new_pebble;
    this_pebble-> size = new_size;
    this_pebble->next = new_pebble;
  }

  return this_pebble;
}


memory_pebble_t *absorb_next(memory_pebble_t *pebble) {
  if ((pebble && pebble->next) && (IS_PEBBLE_FREE(pebble) && IS_PEBBLE_FREE(pebble->next))) {
    if (pebble->parent->first == pebble->next) //Don't delete first pebble in bucket before updating it.
      pebble->parent->first = pebble;

    pebble->size += pebble->next->size + sizeof(memory_pebble_t);
    pebble->next = pebble->next->next;
    if (pebble->next)
      pebble->next->prev = pebble;

    bucket_update_largest(pebble->parent);
  }

  return pebble;
}

memory_pebble_t *melt_prev(memory_pebble_t *pebble) {
  if ((pebble && pebble->prev) && (IS_PEBBLE_FREE(pebble) && IS_PEBBLE_FREE(pebble->prev))) {
    if (pebble->parent->first == pebble)
      pebble->parent->first = pebble->prev;

    pebble->prev->size += pebble->size + sizeof(memory_pebble_t);
    pebble->prev->next = pebble->next;
    if (pebble->next)
      pebble->next->prev = pebble->prev;

    pebble = pebble->prev;
    bucket_update_largest(pebble->parent);
  }
  
  return pebble;
}

memory_pebble_t *shrink_pebble(memory_pebble_t *pebble, size_t size) {
  memory_pebble_t *ret = NULL;
  memory_pebble_t dummy;

  //Dummy needed to place if pebble is split.
  dummy.magic = KMALLOC_MAGIC_PEBBLE;
  dummy.lflags = PEBBLE_FLAG_FREE;
  dummy.alignment = 1;
  dummy.size = size;
  dummy.parent = pebble->parent;

  if (pebble) {
    split_pebble(pebble, &dummy);
    ret = pebble;
  }

  return ret;
}

void *kmalloc(size_t size) {
  void *ret = NULL;
  
  if (size < PEBBLE_MIN_SIZE)
    size = PEBBLE_MIN_SIZE;

  memory_pebble_t pebble;
  pebble.magic = KMALLOC_MAGIC_PEBBLE;
  pebble.lflags = 0 | PEBBLE_FLAG_IN_USE;
  pebble.alignment = PEBBLE_MIN_ALIGN;
  pebble.size = (size + (PEBBLE_MIN_ALIGN -1)) & ~(PAGESIZE -1);

  memory_bucket_t *bucket = (memory_bucket_t *)KHEAPSTART;
  serial_printf(" \nLargest bucket free pebble size %ul \n", bucket->largest_free);

  
  //Try with currently allocated heap space.
  for (; bucket != NULL; bucket = bucket->next) {
    acquire_spinlock(&bucket->spinlock);


    if (bucket->largest_free >= pebble.size) {
      pebble.parent = bucket;
      ret = place_pebble(bucket, &pebble);

      if (ret != NULL) {
        bucket_update_largest(bucket);
        ret = (uint8_t *)ret + sizeof(memory_pebble_t);
        release_spinlock(&bucket->spinlock);
        break;
      }
    }

    release_spinlock(&bucket->spinlock);
  }

  if (ret == NULL) {
    size_t new_size = size + (sizeof(memory_bucket_t) + sizeof(memory_pebble_t));

    bucket = create_bucket(size, 0);

    if (bucket) {
      insert_bucket(bucket, (void *)KHEAPSTART);
      acquire_spinlock(&bucket->spinlock);

      pebble.parent = bucket;
      ret = place_pebble(bucket, &pebble);
      bucket_update_largest(bucket);
      release_spinlock(&bucket->spinlock);
      if (ret != NULL)
        ret = (uint8_t *)ret + sizeof(memory_pebble_t);
    }
  }

  if (ret)
    memset(ret, 0, size);

  return ret;
}

void  *krealloc(void *ptr, size_t size) {
  memory_pebble_t *pebble;
  void *ret = NULL;

  if (size == 0) {
    kfree(ptr);
    return NULL;
  }

  if (ptr == NULL)
    return kmalloc(size);

  pebble = (memory_pebble_t *)((uint8_t *)ptr - sizeof(memory_pebble_t));
  if (pebble->magic != KMALLOC_MAGIC_PEBBLE)
    return NULL;

  //If requested size is smaller than current size shrink pebble.
  if (size <= pebble->size) {
    acquire_spinlock(&pebble->parent->spinlock);
    ret = (void *)((uint8_t *) shrink_pebble(pebble, size) + sizeof(memory_pebble_t));
    release_spinlock(&pebble->parent->spinlock);
  } else {
    //If requested size bigger allocate a new space for pebble.
    ret = kmalloc(size);

    if (ret)
      memcpy(ret, ptr, size);
    kfree(ptr);
  }

  return ret;
}

void kfree(void *ptr) {

  if (ptr == NULL)
    return;

  memory_pebble_t *pebble = (memory_pebble_t *)((uint8_t *) ptr - sizeof(memory_pebble_t));

  if (pebble->magic != KMALLOC_MAGIC_PEBBLE)
    return;

  acquire_spinlock(&pebble->parent->spinlock);

  pebble->lflags = PEBBLE_FLAG_FREE;
  pebble->alignment = 1;

  pebble = melt_prev(pebble);
  absorb_next(pebble);

  release_spinlock(&pebble->parent->spinlock);

  memory_bucket_t *bucket = pebble->parent;

  if (IS_PEBBLE_FREE(bucket->first) && (bucket->first->prev == NULL) && (bucket->first->next == NULL))
    remove_bucket(bucket);
  else
    bucket_update_largest(bucket);
}
