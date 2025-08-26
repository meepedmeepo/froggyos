#ifndef __K_PAGING_H
#define __K_PAGING_H

#include <stdint.h>


extern uint64_t PHYSICAL_ADDRESS_OFFSET;

void init_paging(uint64_t offset);


  struct PageTableEntry {
  uint64_t value;
};

struct PageTable {
  struct PageTableEntry entries[512];
} __attribute__ ((aligned(4096)));



#endif
