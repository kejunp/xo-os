#define PGSIZE 4096

#define PGROUNDUP(x) (((x) + PGSIZE-1) & ~(PGSIZE - 1))
#define PGROUNDDOWN(x) ((x) & ~(PGSIZE-1))

