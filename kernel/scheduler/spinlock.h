#include "helpers/defs.h"

struct spinlock {
  volatile uint32_t locked;
};

