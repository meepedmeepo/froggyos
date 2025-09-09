#ifndef _K_HEAP_H
#define _K_HEAP_H

#include <stddef.h>

#define KHEAPSTART 0xffffffff0000000UL


void init_heap();

void *grow_heap(size_t size);


#endif
