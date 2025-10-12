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

#define rb_parent(node) ((node)->parent)
#define rb_color(node) ((node)->color)
#define rb_is_red(node) ((node) && (node)->color == RB_RED)
#define rb_is_black(node) (!(node) || (node)->color == RB_BLACK)
#define rb_set_red(node) do { (node)->color = RB_RED; } while(0)
#define rb_set_black(node) do { (node)->color = RB_BLACK; } while(0)
#define rb_set_parent(node, p) do { (node)->parent = (p); } while(0)
#define rb_set_color(node, c) do { (node)->color = (c); } while(0)

void rb_init_node(struct rb_node* node);
void rb_link_node(struct rb_node* node, struct rb_node* parent, struct rb_node** rb_link);
void rb_insert_fixup(struct rb_root* root, struct rb_node* node);
void rb_insert_color(struct rb_root* root, struct rb_node* node);
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

#define rb_empty(root) ((root)->node == NULL)

