#ifndef K_FRAMEALLOC_H
#define K_FRAMEALLOC_H
#include <stddef.h>
#include <stdint.h>


struct PhysFrame {
  uint64_t physAddress;
};

struct FreeFrameList {
  void *head;
  struct PhysFrame *nextFree;
  uint64_t numFrames;
};

//Creates empty FreeFrameList at address with head initialised,
// then adds provided region of free frames to the list. 
struct FreeFrameList* init_frame_list(void* addr, void* memmap_region);

void add_frame_region(struct FreeFrameList* list, void* memmap_region);

void push_frame_list(struct FreeFrameList *list, uint64_t physAddress);
void *pop_frame_list(struct FreeFrameList *list);

struct FrameBitmap {
  uint8_t * bitmap;
};

struct FrameBitmap* init_frame_bitmap(uint64_t total_memory, void* memmap);






#endif
