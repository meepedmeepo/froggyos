#include "spinlock.h"


void acquire_spinlock(uint32_t *spin) {
  _acquire_spinlock(spin);
}

void release_spinlock(uint32_t *spin) {
  _release_spinlock(spin);
}
