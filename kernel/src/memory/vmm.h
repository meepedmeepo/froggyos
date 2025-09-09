#ifndef _K_VMM_H
#define _K_VMM_H

#include <stdint.h>

void* alloc_page();
void* free_page(void* virtAddress);



#endif
