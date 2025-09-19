#include "heap.h"
#include "../klibc/spinlock.h"
#include "../drivers/uart.h"
#include "../klibc/string/printf.h"
uint32_t kheap_spinlock = 0;
uintptr_t K_HEAP_END;

extern void acquire_spinlock(uint32_t *spin);
extern void release_spinlock(uint32_t *spin);

void init_heap() {
   K_HEAP_END = KHEAPSTART;//0x1000 is pagesize
   serial_printf("heap start %p, heap end: %p", KHEAPSTART, K_HEAP_END);
   write_serial_string("Kernel Heap Initialised.\n");
  
}

//Size should be multiple of pagesize.
// TODO: add checks to ensure there is enough phys memory available to
// map new heap space to frames.
void *grow_heap(size_t size) {
   serial_printf("heap\n");
   acquire_spinlock(&kheap_spinlock);
   serial_printf("got lock.\n");
   uintptr_t start_region = K_HEAP_END;
   serial_printf("Heap end region: %ulh \n", (uint64_t)start_region);
   K_HEAP_END += (size * 0x1000) ;

   release_spinlock(&kheap_spinlock);
   serial_printf("ptr: %ulh",(uint64_t) KHEAPSTART);
   serial_printf("\n heap lock release\n");
   return (void *)start_region;
}
//SEAL F R O G E
