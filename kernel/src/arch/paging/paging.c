#include "paging.h"
#include "../../drivers/uart.h"
#include <stdbool.h>
#include <stdint.h>

uint64_t PHYSICAL_ADDRESS_OFFSET = 0;

void init_paging(uint64_t offset) {
  PHYSICAL_ADDRESS_OFFSET = offset;
}

PhysAddress create_new_page_table(struct FreeFrameList *frameAlloc) {

  PhysAddress addr = (PhysAddress)pop_frame_list(frameAlloc);

  //If no usable frame available indicate failed operation.
  if (addr == NULL) {
    return NULL;
  }

  print_ptr_address(addr);
  
  void* virtAddr = phys_to_virt_translation(addr);
  
  struct PageTable *pt = (struct PageTable *)virtAddr;

  write_serial_string("created page table \n");
  print_ptr_address(pt);

  //PhysAddress actualPhysAddr = translate_virt_address((VirtAddress)pt);
  
  //Zero out memory for new page table.
  memset(pt, 0, sizeof(struct PageTable));

  pt->entries[1] = 0;
  //write_serial_string("Memset");
  
 return addr; 
}

VirtAddress map_to_page(VirtAddress address, PhysAddress frame, struct FreeFrameList *frameAlloc, VirtAddress pml4VirtAddress) {

  uint64_t pgd = ((uint64_t)address >> 39) & 0x1ff;//Level 4 index
  uint64_t pud = ((uint64_t)address >> 30) & 0x1ff;//Level 3 index
  uint64_t pmd = ((uint64_t)address >> 21) & 0x1ff;//Level 2 index
  uint64_t pt = ((uint64_t)address >> 12) & 0x1ff;//Level 1 index

  //write_serial_string("indices set\n");
  uint64_t *virtPgdAddress;
  //reads physical address of pml4 and translates to virtual if pml4 address is null.
  if (pml4VirtAddress == NULL) {
    
    uint64_t pgdAddress = read_cr3();
    //extract actual phys address from cr3 register value.
    uint64_t* pgdPtr =(uint64_t *)(pgdAddress & ~0xFFFULL);

 
    
    virtPgdAddress = phys_to_virt_translation((uint64_t *)pgdAddress);
  } else {
    virtPgdAddress = pml4VirtAddress;
  }
  //print_ptr_address(virtPgdAddress);

  uint64_t vals = (uint64_t)virtPgdAddress;

  
  
  if (virtPgdAddress == NULL) return NULL;
  
  //write_serial_string("pgd address translated\n");
  //char r[40];
  //write_serial_string(ultoa(pgd, r, 10));
  //write_serial_string("\n");
  uint64_t pgdEntry = *(virtPgdAddress + pgd);

  //write_serial_string("pgd checked");
  
  VirtAddress virtPudAddress = NULL;
  //Creates new pud if table doesn't already exist.
  if (!is_page_table_present(pgdEntry)) {
    PhysAddress newPudPhysAddr = create_new_page_table(frameAlloc);
    //write_serial_string("New pud table.");
    if (newPudPhysAddr == NULL)
    {
      write_serial_string("Can't allocate new PUD page table");
      return NULL;
    }

    *(virtPgdAddress + pgd) = (uint64_t)newPudPhysAddr & PAGE_ADDR_MASK | create_basic_page_attributes();

    virtPudAddress = phys_to_virt_translation(newPudPhysAddr);
  } else {
    //pud table exists so just read PhysAddr from table instead.
    PhysAddress newPudPhysAddr = (PhysAddress)page_table_phys_address(pgdEntry);
    virtPudAddress = phys_to_virt_translation(newPudPhysAddr);
  }
  
  //Check pud entry has mapped pmd table.
  uint64_t pudEntry = *(virtPudAddress + pud);
  
  VirtAddress virtPmdAddress = NULL;
  if (!is_page_table_present(pudEntry)) {
    //create new pmd table.
    PhysAddress newPmdPhysAddr = create_new_page_table(frameAlloc);
    if (newPmdPhysAddr == NULL) {
      write_serial_string("Can't allocate new PMD page table");
      return NULL;
    }

    *(virtPudAddress + pud) = (uint64_t)newPmdPhysAddr & PAGE_ADDR_MASK | create_basic_page_attributes();
    virtPmdAddress = phys_to_virt_translation(newPmdPhysAddr);  
  } else {
    //PMD table exists so just read PhysAddr from table.
    PhysAddress newPmdPhysAddr = (PhysAddress)page_table_phys_address(pudEntry);
    virtPmdAddress = phys_to_virt_translation(newPmdPhysAddr);
  }

  //Check PMD table has mapped PT.
  uint64_t pmdEntry = *(virtPmdAddress + pmd);
  
  VirtAddress virtPtAddress = NULL;
  if (!is_page_table_present(pmdEntry)) {
    //Create new PT.
    PhysAddress newPtPhysAddr = create_new_page_table(frameAlloc);
    if (newPtPhysAddr == NULL) {
      write_serial_string("Couldn't Allocate new PT.");
      return NULL;
    }

    *(virtPmdAddress + pmd) = (uint64_t)newPtPhysAddr & PAGE_ADDR_MASK | create_basic_page_attributes();
    virtPtAddress = phys_to_virt_translation(newPtPhysAddr);  
  } else {
    PhysAddress newPtPhysAddr = (PhysAddress)page_table_phys_address(pmdEntry);
    virtPtAddress = phys_to_virt_translation(newPtPhysAddr);
  }

  *(virtPtAddress + pt) = (uint64_t)frame & PAGE_ADDR_MASK | create_basic_page_attributes();


  return address;
}

PhysAddress create_kernel_page_tables(struct FreeFrameList *frameAlloc) {
  PhysAddress pml4PhysAddr = create_new_page_table(frameAlloc);
  write_serial_string("created pml4 \n");
  struct PageTable *pml4 = (struct PageTable *)phys_to_virt_translation(pml4PhysAddr);
  write_serial_string("pml4 pointer \n");
  uint64_t lowMemoryEnd = (1024 * 1024) * 2;
  //Identity map first 2mb of memory.
  for(uint64_t i = 0x1000; i < lowMemoryEnd; i += PAGE_SIZE)
  {
    identity_map_page((uint64_t *)i, frameAlloc, (VirtAddress)pml4);
  }

  
  return pml4PhysAddr;
}

VirtAddress identity_map_page(PhysAddress frame, struct FreeFrameList *frameAlloc, VirtAddress pml4VirtAddress) {
  write_serial_string("identity map \n");  
  VirtAddress addr = map_to_page(frame, frame,frameAlloc, pml4VirtAddress);
  return addr;
}

struct pt_indices_t calculate_pt_indices(void *addr) {
  
  uint64_t pgd = ((uint64_t)addr >> 39) & 0x1ff;//Level 4 index
  uint64_t pud = ((uint64_t)addr >> 30) & 0x1ff;//Level 3 index
  uint64_t pmd = ((uint64_t)addr >> 21) & 0x1ff;//Level 2 index
  uint64_t pt = ((uint64_t)addr >> 12) & 0x1ff;//Level 1 index

  struct pt_indices_t res = {pgd,pud,pmd, pt};
  return res;
}

void *get_pt_entry_frame(void *address, uint64_t entry) {
  

  return NULL;
}

void *unmap_page(void *addr) {
  uint64_t *pml4 = active_pml4_table();

  struct pt_indices_t indices = calculate_pt_indices(pml4);

  struct PageTable *pt4 = (struct PageTable *)pml4;

  uint64_t *pgd = (uint64_t *)page_table_phys_address(pt4->entries[indices.pgd]);

  struct PageTable *pgdS = 
}
