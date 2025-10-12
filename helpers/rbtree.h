#pragma once

#include "defs.h"

enum rb_color { RB_RED, RB_BLACK };

struct rb_node {
  struct rb_node* parent;
  struct rb_node* left;
  struct rb_node* right;
  enum rb_color   color;
};

struct rb_root {
  struct rb_node* node;
};

#define RB_ROOT_INIT { NULL }
#define RB_ROOT(name) struct rb_root name = RB_ROOT_INIT

#define rb_entry(ptr, type, member) \
  ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

static inline int rb_empty(const struct rb_root* root) {
  return root->node == NULL;
}

void rb_init_node(struct rb_node* node);
void rb_insert(struct rb_root* root, struct rb_node* node,
              int (*cmp)(const struct rb_node* a, const struct rb_node* b));
void rb_erase(struct rb_root* root, struct rb_node* node);
void rb_replace_node(struct rb_root* root, struct rb_node* victim, struct rb_node* new);

struct rb_node* rb_first(const struct rb_root* root);
struct rb_node* rb_last(const struct rb_root* root);
struct rb_node* rb_next(const struct rb_node* node);
struct rb_node* rb_prev(const struct rb_node* node);

struct rb_node* rb_find(struct rb_root* root, const void* key, 
                       int (*cmp)(const void* key, const struct rb_node* node));
struct rb_node* rb_find_first(struct rb_root* root, const void* key,
                             int (*cmp)(const void* key, const struct rb_node* node));

void rb_augment_path(struct rb_node* node, void (*augment)(struct rb_node*));
int rb_is_valid_rbtree(struct rb_root* root);

#define rb_for_each(pos, root) \
  for (pos = rb_first(root); pos; pos = rb_next(pos))

#define rb_for_each_safe(pos, tmp, root) \
  for (pos = rb_first(root); pos && (tmp = rb_next(pos), 1); pos = tmp)

