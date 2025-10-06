#include "helpers/defs.h"
#include "spinlock.h"

struct spinlock {
  volatile uint32_t locked;
};

void spin_lock(spinlock_t* lk) {
  while (__atomic_exchange_n(&lk->locked, 1, __ATOMIC_ACQUIRE));
}

void spin_unlock(spinlock_t* lk) {
  __atomic_store_n(&lk->locked, 0, __ATOMIC_RELEASE);
}

