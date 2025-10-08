#pragma once

#include "helpers/defs.h"

struct spinlock {
  volatile uint32_t locked;
};

void spin_init(struct spinlock* lk);

void spin_lock(struct spinlock* lk);
void spin_unlock(struct spinlock* lk);

