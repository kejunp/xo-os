#include "vma.h"
#include "kalloc.h"

// Red-black tree comparison function for VMAs
static int vma_cmp(const struct rb_node* a, const struct rb_node* b) {
    struct vma* vma_a = rb_entry(a, struct vma, rb);
    struct vma* vma_b = rb_entry(b, struct vma, rb);
    
    if (vma_a->start < vma_b->start) return -1;
    if (vma_a->start > vma_b->start) return 1;
    return 0;
}

// Find comparison function for red-black tree
static int vma_find_cmp(const void* key, const struct rb_node* node) {
    uintptr_t addr = *(const uintptr_t*)key;
    struct vma* vma = rb_entry(node, struct vma, rb);
    
    if (addr < vma->start) return -1;
    if (addr >= vma->end) return 1;
    return 0;
}

// Allocate and initialize a new VMA
struct vma* vma_alloc(void) {
    struct vma* vma = (struct vma*)kalloc();
    if (!vma) return NULL;
    
    vma->start = 0;
    vma->end = 0;
    vma->prot = 0;
    vma->flags = 0;
    vma->offset = 0;
    vma->file = NULL;
    vma->prev = NULL;
    vma->next = NULL;
    rb_init_node(&vma->rb);
    
    return vma;
}

// Free a VMA structure
void vma_free(struct vma* vma) {
    if (vma) {
        kfree(vma);
    }
}

// Insert VMA into both tree and list
void vma_insert(struct mm* mm, struct vma* vma) {
    // Insert into red-black tree
    rb_insert(&mm->vma_tree, &vma->rb, vma_cmp);
    
    // Insert into sorted linked list
    struct vma* curr = mm->vma_list;
    struct vma* prev = NULL;
    
    // Find insertion point
    while (curr && curr->start < vma->start) {
        prev = curr;
        curr = curr->next;
    }
    
    // Insert into list
    vma->prev = prev;
    vma->next = curr;
    
    if (prev) {
        prev->next = vma;
    } else {
        mm->vma_list = vma;
    }
    
    if (curr) {
        curr->prev = vma;
    }
}

// Remove VMA from both tree and list
void vma_remove(struct mm* mm, struct vma* vma) {
    // Remove from red-black tree
    rb_erase(&mm->vma_tree, &vma->rb);
    
    // Remove from linked list
    if (vma->prev) {
        vma->prev->next = vma->next;
    } else {
        mm->vma_list = vma->next;
    }
    
    if (vma->next) {
        vma->next->prev = vma->prev;
    }
}

// Find VMA containing a specific address
struct vma* vma_find(struct mm* mm, uintptr_t addr) {
    struct rb_node* node = rb_find(&mm->vma_tree, &addr, vma_find_cmp);
    if (node) {
        return rb_entry(node, struct vma, rb);
    }
    return NULL;
}

// Find VMA before a given address
struct vma* vma_find_prev(struct mm* mm, uintptr_t addr) {
    struct vma* vma = mm->vma_list;
    struct vma* prev = NULL;
    
    while (vma && vma->start < addr) {
        prev = vma;
        vma = vma->next;
    }
    
    return prev;
}

// Split a VMA at a given address
int vma_split(struct mm* mm, struct vma* vma, uintptr_t addr) {
    if (addr <= vma->start || addr >= vma->end) return -1;
    
    // Allocate new VMA for the second part
    struct vma* new_vma = vma_alloc();
    if (!new_vma) return -1;
    
    // Set up the new VMA
    new_vma->start = addr;
    new_vma->end = vma->end;
    new_vma->prot = vma->prot;
    new_vma->flags = vma->flags;
    new_vma->file = vma->file;
    new_vma->offset = vma->offset + (addr - vma->start);
    
    // Adjust original VMA
    vma->end = addr;
    
    // Insert new VMA
    vma_insert(mm, new_vma);
    
    return 0;
}

// Initialize memory manager
void mm_init(struct mm* mm) {
    mm->vma_tree = (struct rb_root)RB_ROOT_INIT;
    mm->vma_list = NULL;
}