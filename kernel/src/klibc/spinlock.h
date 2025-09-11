#ifdef _K_SPINLOCK_H
#define _K_SPINLOCK_H


extern void _acquire_spinlock(uint32_t *spin);
extern void _release_spinlock(uint32_t *spin);

void acquire_spinlock(uint32_t *spin) {
  _acquire_spinlock(spin);
}

void release_spinlock(uint32_t *spin) {
  _release_spinlock(spin);
}


#endif
