#ifndef _K_SPINLOCK_H
#define _K_SPINLOCK_H

#include <stdint.h>

extern void _acquire_spinlock(uint32_t *spin);
extern void _release_spinlock(uint32_t *spin);

void acquire_spinlock(uint32_t *spin); 

void release_spinlock(uint32_t *spin); 


#endif
