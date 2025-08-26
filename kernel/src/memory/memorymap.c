#include "memorymap.h"
#include "../limine.h"
#include "../drivers/uart.h"
#include <stddef.h>

extern char* itoa(int value, char* result, int base);
extern char* ultoa(int value, char* result, int base);

void *read_memory_map(void * memmap) {
  uint64_t total_usable = 0;
  uint64_t total = 0;
  struct limine_memmap_response* map = (struct limine_memmap_response*)memmap;

  bool isFirst = true;
  void *frameRegion = NULL;
  
  for (int i = 0; i < map->entry_count; i++) {
    uint64_t value = map->entries[i]->length;

    if (map->entries[i]->type == LIMINE_MEMMAP_USABLE) {
      total_usable += value;
      print_memmap_entry(map->entries[i], i);

      if (isFirst) {
        isFirst = false;
      } else if (value/4096 > 500 && frameRegion == NULL) {
        frameRegion = map->entries[i];
      }
    }

    total += value;
  }

  write_serial_string("Total Usable Memory: ");
  char result[80];
  write_serial_string(ultoa(total_usable, result, 10));
  write_serial_string("KiB / ");
  char res[80];
  write_serial_string(ultoa(total, res, 10));
  write_serial_string(" KiB \n");

  return frameRegion;
}

void print_memmap_entry(void * entry, int index) {
  struct limine_memmap_entry* mm_entry = (struct limine_memmap_entry*)entry;

  write_serial_string("Region: #");
  char r[40];
  write_serial_string(itoa(index, r, 10));
  write_serial_string("\n");
  char result[40];
  write_serial_string(ultoa(mm_entry->base, result, 16));
  write_serial_string("\nLength: ");
  char res[40];
  write_serial_string(ultoa(mm_entry->length/4096, res, 10));
  write_serial_string("\n");
}

bool get_frame_status(struct MemoryBitmap* map, uint64_t index) {
  uint64_t indice = index % 64;
  
  uint64_t mask = 1 >> indice;
  uint8_t value = map->map[indice] & mask;

  bool res = value > 0 ? true : false;

  return res;
}

