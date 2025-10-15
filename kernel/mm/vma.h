#pragma once

#include "helpers/defs.h"
#include "helpers/rbtree.h"

// Memory protection flags
#define PROT_NONE  0x0
#define PROT_READ  0x1
#define PROT_WRITE 0x2
#define PROT_EXEC  0x4

// Memory mapping flags
#define MAP_PRIVATE   0x02
#define MAP_SHARED    0x01
#define MAP_ANONYMOUS 0x20
#define MAP_FIXED     0x10

// VMA specific flags
#define VM_READ     0x00000001
#define VM_WRITE    0x00000002
#define VM_EXEC     0x00000004
#define VM_SHARED   0x00000008
#define VM_GROWSDOWN 0x00000100
#define VM_GROWSUP   0x00000200

struct vma {
  uintptr_t start;
  uintptr_t end;
  uint32_t  prot;
  uint32_t  flags;
  off_t     offset;
  void*     file;

  // linked list
  struct vma* prev;
  struct vma* next;

  // red black tree
  struct rb_node rb;
};

struct mm {
  struct rb_root vma_tree;
  struct vma*    vma_list;
};

// Memory manager initialization
void mm_init(struct mm* mm);

// Basic VMA operations
struct vma* vma_alloc(void);
void vma_free(struct vma* vma);
void vma_insert(struct mm* mm, struct vma* vma);
void vma_remove(struct mm* mm, struct vma* vma);
struct vma* vma_find(struct mm* mm, uintptr_t addr);
struct vma* vma_find_prev(struct mm* mm, uintptr_t addr);
int vma_split(struct mm* mm, struct vma* vma, uintptr_t addr);

