#include "kalloc.h"
#include "memlayout.h"
#include "kernel/scheduler/spinlock.h"

struct run {
  struct run* next;
};

static struct run*      freelist;
static struct spinlock kalloc_lk;


extern char end[];

void kinit(struct xo_boot_info* boot_info) {
  uint64_t kernel_end = PGROUNDUP((uint64_t)end);

  spin_init(&kalloc_lk);
  freelist = NULL;

  for (uint32_t i = 0; i < boot_info->memory_map_entries; ++i) {
    struct xo_memory_entry* e = &boot_info->memory_map[i];
    if (e->type != XO_MEMORY_AVAILABLE)
      continue;

    uint64_t start = PGROUNDUP(e->base_address);
    uint64_t limit = PGROUNDDOWN(e->base_address + e->length);

    if (limit <= kernel_end)
      continue;
    if (start < kernel_end)
      start = kernel_end;

    for (uint64_t p = start; p + PGSIZE <= limit; p += PGSIZE)
      kfree((void *)p);
  }
}

void* kalloc(void) {
  spin_lock(&kalloc_lk);
  struct run* r = freelist;

  if (r)
    freelist = r->next;
  spin_unlock(&kalloc_lk);

  if (r)
    memset((char*)r, 5, 4096);
  return (void*)r;
}

void kfree(void* p) {
  struct run* r = (struct run*)p;

  memset(p, 1, 4096); // fill with junk

  spin_lock(&kalloc_lk);
  r->next = freelist;
  freelist = r;
  spin_unlock(&kalloc_lk);
}

