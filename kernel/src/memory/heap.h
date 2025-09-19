#ifndef _K_HEAP_H
#define _K_HEAP_H
#include <stdint.h>
#include <stddef.h>

extern char kEnd;

 static uintptr_t KHEAPSTART = (uintptr_t)&kEnd;


void init_heap();

void *grow_heap(size_t size);


#endif
