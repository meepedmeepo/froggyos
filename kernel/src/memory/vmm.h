#ifndef _K_VMM_H
#define _K_VMM_H

#include <stddef.h>
#include <stdint.h>
#include "framealloc.h"

struct VirtualMemoryManager {
  struct FreeFrameList *frameAlloc;
};

void init_vmm(struct FreeFrameList *pmm);

//If addr is NULL then one will be assigned.
void *alloc_page(void *addr);
void *free_page(void* virtAddress);

void *mmap(size_t size);

#endif
