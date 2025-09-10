#ifdef _K_SPINLOCK_H
#define _K_SPINLOCK_H

#include <stdatomic.h>

void acquire_spinlock(atomic_flag* mutex);
void acquire_spinlock(atomic_flag* mutex) {
  while(atomic_flag_test_and_set_explicit(mutex, memory_order_acquire)) {
    __builtin_ia32_pause();
  }
}

void release_mutex(atomic_flag* mutex) {
  atomic_flag_clear_explicit(mutex, memory_order_acquire);
}


#endif
