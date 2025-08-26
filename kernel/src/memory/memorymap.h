#ifndef __K_MEMORYMAP_H
#define __K_MEMORYMAP_H

#define MEM_TYPE_FREE 0x1;
#define MEM_TYPE_USED 0x0;

#include <stdint.h>
#include <stdbool.h>

struct MemoryMapEntry {
  uint64_t base_addr;
  uint64_t length;
  uint64_t memorytype;
};


struct MemoryMap {
  struct MemoryMapEntry** entries;
  uint64_t entry_count;
};

struct MemoryBitmap {
  uint64_t map[64];
};

bool get_frame_status(struct MemoryBitmap* map, uint64_t index);

void *read_memory_map(void * memmap);
void print_memmap_entry(void * entry, int index);

void create_usable_frame_list(void * memmap);

struct UsableMemory {
  uint64_t phys_addr;
  uint64_t length;
};

#endif
