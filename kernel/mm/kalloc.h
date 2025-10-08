#pragma once

#include "boot/boot.h"

void* kalloc(void);
void  kfree(void* p);
void  kinit(struct xo_boot_info* boot_info);

