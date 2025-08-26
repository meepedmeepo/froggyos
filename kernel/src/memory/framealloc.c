#include "framealloc.h"
#include <stdint.h>
#include <stddef.h>
#include "../limine.h"
#include "../drivers/uart.h"
extern uint64_t PHYSICAL_ADDRESS_OFFSET;
extern char* ultoa(uint64_t value, char* result, int base);

struct FrameBitmap* init_frame_bitmap(uint64_t total_memory, void* memmap) {
  size_t required_size = (total_memory/4096) / 8;
  
  
  return NULL;
}


struct FreeFrameList* init_frame_list(void* addr, void *memmap_region) {
  struct FreeFrameList* list = (struct FreeFrameList*) addr;

  list->nextFree = NULL;
  list->head = addr + sizeof(struct FreeFrameList);
  list->numFrames = 0;
  
  write_serial_string("frame list inialised");

  add_frame_region(list, memmap_region);

  return list;
}

void push_frame_list(struct FreeFrameList *list, uint64_t physAddress) {
  list->nextFree = (struct PhysFrame*)list->head;
  list->nextFree->physAddress = physAddress;
  list->numFrames += 1;
  
  list->head = (void*)list->nextFree + sizeof(struct PhysFrame);
}

void *pop_frame_list(struct FreeFrameList *list) {

  if (list->nextFree == NULL) {
    return NULL;
  }

  void *address = (void*)list->nextFree->physAddress + PHYSICAL_ADDRESS_OFFSET;

  if (list->nextFree - 1 != NULL) {
    list->nextFree -= 1;
  } else {
    list->nextFree = NULL;
  }

  list->numFrames --;
  list->head -= sizeof(struct PhysFrame);
  
  return address;
}

void add_frame_region(struct FreeFrameList * list, void *memmap_region) {
  struct limine_memmap_entry* region = (struct limine_memmap_entry*)memmap_region;

  uint64_t physAddress = region->base;
  for (uint64_t x = 0; x < region->length/4096; x++) {
    char res[60];
    write_serial_string(ultoa(x,res,10));
    write_serial_string("\n");
    push_frame_list(list, physAddress);
    physAddress += 4096;//TODO: change this into PAGE_SIZE macro or someshit

    if (x >= 30000-1) {
      break;
    }
   }
}


