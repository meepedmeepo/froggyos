#include "heap.h"
#include "../klibc/spinlock.h"
#include "../drivers/uart.h"

static uint32_t kheap_spinlock = 0;
static uintptr_t K_HEAP_END;

extern void acquire_spinlock(uint32_t *spin);
extern void release_spinlock(uint32_t *spin);

void init_heap() {
   K_HEAP_END = KHEAPSTART + (0x1000 * 120);//0x1000 is pagesize
   write_serial_string("Kernel Heap Initialised.\n");
  
}

//Size should be multiple of pagesize.
// TODO: add checks to ensure there is enough phys memory available to
// map new heap space to frames.
void *grow_heap(size_t size) {
   acquire_spinlock(&kheap_spinlock);
   uintptr_t start_region = K_HEAP_END;

   K_HEAP_END += size;

   release_spinlock(&kheap_spinlock);
   return (void *)start_region;
}
