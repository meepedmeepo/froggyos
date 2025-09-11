#include "vmm.h"
#include <stddef.h>
#include "framealloc.h"
#include "heap.h"
#include "../arch/paging/paging.h"

static struct VirtualMemoryManager vmm;

void init_vmm(struct FreeFrameList *pmm) {
  vmm.frameAlloc = pmm;
  write_serial_string("VMM Initialised.\n");
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

  if (!frame) return NULL;//Frame list empty so operation fails.
  //TODO: implement debugging messages for when this occurs.
  
  if (!addr) {
    //No address chosen, address to map to selected randomly-ish.
    // TODO: implement selecting new address.
    return NULL;
  }

  void *res = map_to_page(addr, frame, vmm.frameAlloc, NULL);

  
  return res;
}

void *mmap(size_t  size) {
  uintptr_t region_start = (uintptr_t)grow_heap(size);

  if (!region_start) {
    return NULL;
  }

  for (uintptr_t addr = region_start; addr < region_start + size; addr += 0x1000) {
    alloc_page((void *)addr);
  }

  return (void *)region_start;
}
