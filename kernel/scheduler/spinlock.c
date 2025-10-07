#include "spinlock.h"

void spin_lock(struct spinlock* lk) {
  while (__atomic_exchange_n(&lk->locked, 1, __ATOMIC_ACQUIRE));
}

void spin_unlock(struct spinlock* lk) {
  __atomic_store_n(&lk->locked, 0, __ATOMIC_RELEASE);
}

