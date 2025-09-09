#ifndef K_BUCKETALLOC_H
#define K_BUCKETALLOC_H

#include <stdatomic.h>
#include <stdint.h>
#include <stddef.h>

#define PAGESIZE 4096

#define MIN_INITIAL_ALLOC (65536 + sizeof(struct memory_bucket_t) + sizeof(struct memory_pebble_t))

#define KMALLOC_MAGIC_BUCKET 'BUCK'
#define KMALLOC_MAGIC_PEBBLE 'ROCK'

#define PEBBLE_FLAG_FREE (0 << 0) //If bit set, pebble is in use.
#define PEBBLE_FLAG_IN_USE (1 << 0)
#define PEBBLE_FLAG_ALIGNED (1 << 1) //If bit set, pebble is aligned.
#define PEBBLE_FLAG_CLEARED (1 << 2) //If bit set, pebble memory has been zeroed

#define PEBBLE_MIN_ALIGN 64
#define PEBBLE_MIN_SIZE 64

#define IS_PEBBLE_FREE(p) (((p)->lflags & PEBBLE_FLAG_IN_USE) == PEBBLE_FLAG_FREE)

#define UPDATE_NODE(p0, p1)   \
{                             \
    (p0)->next = (p1)->next;  \
    (p1)->next = (p0);        \
    (p0)->prev = (p1);        \
    if ((p0)->next)           \
      (p0)->next->prev = (p0) \
}

//Checks if pebble is big enough to split.
#define SPLIT_PEBBLE(s0, s1) ((((s0) + PEBBLE_MIN_ALIGN - 1) & ~PEBBLE_MIN_SIZE) > ((s1) + sizeof(memory_pebble_t) + PEBBLE_MIN_SIZE))


typedef struct memory_pebble_t memory_pebble_t;

#pragma pack(push, 1)

struct memory_bucket_t {
  uint32_t magic;
  uint32_t lflags;//Local flags.
  size_t largest_free;//Largest free block available in bucket.
  size_t size;//Counts of 4096 pages used for bucket.

  atomic_flag spinlock;
  uint8_t reserved[12];

  memory_pebble_t *first;

  //Double linked list of buckets.
  struct memory_bucket_t *prev;
  struct memory_bucket_t *next;

} typedef memory_bucket_t;

struct memory_pebble_t {
  uint32_t magic;
  uint32_t lflags;//Local flags.
  uint32_t reserved;
  uint32_t alignment;
  size_t size;//Number of bytes requested for pebble.
  uint8_t reserved1[12];

  memory_bucket_t *parent;

  //Double linked list of pebbles in same bucket.
  memory_pebble_t *prev;
  memory_pebble_t *next;
};

#pragma pack(pop)

memory_bucket_t *create_bucket(size_t size, const uint32_t flags);
memory_pebble_t *place_pebble(memory_bucket_t *bucket, memory_pebble_t pebble);
memory_pebble_t *split_pebble(memory_pebble_t *pebble, memory_pebble_t *src);


//Public interface.
// TODO: Considering moving this into a seperate file?
void *kmalloc(size_t size, uint32_t alignment, uint32_t flags);
void *krealloc(void *ptr, size_t size);
void kfree(void * ptr);


void *kmalloc_init(size_t size);



#endif
