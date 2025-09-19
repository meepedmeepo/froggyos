#ifndef _K_SPINLOCK_H
#define _K_SPINLOCK_H

#include <stdint.h>

void acquire_spinlock(uint32_t *spin); 

void release_spinlock(uint32_t *spin); 


#endif
