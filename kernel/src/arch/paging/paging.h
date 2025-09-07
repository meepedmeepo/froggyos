#ifndef __K_PAGING_H
#define __K_PAGING_H

#define PAGE_ADDR_MASK 0x000FFFFFFFFFF000ULL
#define PAGE_SIZE 4096UL

#include <stdint.h>
#include "../../memory/framealloc.h"
#include "stdbool.h"
#include "../../drivers/uart.h"

extern uint64_t PHYSICAL_ADDRESS_OFFSET;
extern void *memset(void *s, int c, size_t n);
extern char *ultoa(uint64_t value, char* result, int  base);

typedef uint64_t* PhysAddress;
typedef uint64_t* VirtAddress;

void init_paging(uint64_t offset);

typedef enum
{
    PAGE_PRESENT = (1ULL << 0),
    PAGE_RW = (1ULL << 1),
    PAGE_USER = (1ULL << 2),
    PAGE_PWT = (1ULL << 3),
    PAGE_PCD = (1ULL << 4),
    PAGE_ACCESSED = (1ULL << 5),
    PAGE_DIRTY = (1ULL << 6),
    PAGE_PAT = (1ULL << 7),
    PAGE_GLOBAL = (1ULL << 8),
    PAGE_NX = (1ULL << 63), // Only if EFER.NXE is enabled
} PageAttributes;


inline void* phys_to_virt_translation(void * addr) {
  uintptr_t val = (uintptr_t) addr + (uintptr_t)PHYSICAL_ADDRESS_OFFSET;
  void* res = (void*)val;
  return res;
}

static inline uint64_t read_cr3(void){
  uint64_t val;
  asm volatile ("mov %%cr3, %0" : "=r"(val));
  //read_cr3() & ~0xFFFULL this is how to get phys address of current pml4
  return val;
}


struct PageTable {
    uint64_t  entries[512];
};

PhysAddress create_new_page_table(struct FreeFrameList *frameAlloc);

//Creates new mapping of frame to a page.
//Will allocate new frames for missing page tables.
// pml4VirtAddress only needs to be set to valid value while limine page tables are in use.
VirtAddress map_to_page(VirtAddress address, PhysAddress frame, struct FreeFrameList *frameAlloc, VirtAddress pml4VirtAddress);
VirtAddress identity_map_page(PhysAddress frame, struct FreeFrameList* frameAlloc, VirtAddress pml4VirtAddress);
PhysAddress translate_virt_address(VirtAddress address);

bool inline is_page_table_present(uint64_t pt_entry) {
  return pt_entry & 0x1;
}

uint64_t inline page_table_phys_address(uint64_t pt_entry)
{
  return pt_entry & ~((1ull<<12)-1) & ((1ull<<51)-1);
}

static inline uint64_t* active_pml4_table() {
  uint64_t addr = (uint64_t)read_cr3() & ~0xFFFULL;
  addr += PHYSICAL_ADDRESS_OFFSET;

  return (uint64_t *)addr;
}

uint64_t inline create_basic_page_attributes(){
  uint64_t attr = 0;
  attr |= PAGE_PRESENT;
  attr |= PAGE_RW;

  return attr;
}

PhysAddress create_kernel_page_tables(struct FreeFrameList* frameAlloc);

inline void print_ptr_address(void *addr){
  char res[30];
  write_serial_string("0x");
  write_serial_string(ultoa((uint64_t)addr, res, 16));
  write_serial_string("\n");
}
static inline void __native_flush_tlb_single(unsigned long addr) {
   asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}

#endif
