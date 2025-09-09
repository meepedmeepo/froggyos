#include "vmm.h"
#include <stddef.h>
#include "framealloc.h"
#include "heap.h"
#include "../arch/paging/paging.h"

static struct VirtualMemoryManager vmm;

void init_vmm(struct FreeFrameList *pmm) {
  vmm.frameAlloc = pmm;
}


void *get_physical_frame_addr() {
  void *frame = pop_frame_list(vmm.frameAlloc);

  if (!frame) {
    //TODO: log failure
  }

  return frame;
}


void *alloc_page(void *addr) {
  void *frame = get_physical_frame_addr();
  
  if (addr) {
    //Map to requested address
    
  } else
  {
    //Choose address to map to.
  }

  void *res = map_to_page(addr, frame, vmm.frameAlloc, NULL);

  
  return res;
}

void *mmap(size_t size) {
  uintptr_t region_start = (uintptr_t)grow_heap(size);

  if (!region_start) {
    return NULL;
  }

  for (uintptr_t addr = region_start; addr < region_start + size; addr += 0x1000) {
    alloc_page((void *)addr);
  }

  return (void *)region_start;
}
