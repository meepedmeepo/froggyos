#include "vmm.h"
#include <stddef.h>
#include "framealloc.h"
#include "heap.h"
#include "../arch/paging/paging.h"
#include "../klibc/string/printf.h"

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

void free_page(void *ptr) {
  
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

void munmap(void *ptr, size_t size) {
  for(uintptr_t addr = (uintptr_t)ptr; addr < (uintptr_t)ptr + size; addr += 0x1000) {
    void *frame = unmap_page((void *) addr);

    if(frame) {
      push_frame_list(vmm.frameAlloc, (uint64_t)frame);
    } else {
      serial_printf("Error: Couldn't Unmap page at address %ulh", (uint64_t)addr);
    }
  }
}
