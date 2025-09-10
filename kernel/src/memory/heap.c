#include "heap.h"
#include <stdatomic.h>

static atomic_flag kheap_mutex = ATOMIC_FLAG_INIT;
static uintptr_t K_HEAP_END;


void init_heap() {
   K_HEAP_END = KHEAPSTART + (0x1000 * 120);//0x1000 is pagesize
  
}

void *grow_heap(size_t size) {
   acquire_spinlock(&kheap_mutex);

   return NULL;
}
