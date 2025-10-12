#include "rbtree.h"

void rb_init_node(struct rb_node* node) {
  node->parent = NULL;
  node->left = NULL;
  node->right = NULL;
  node->color = RB_RED;
}

static void rb_rotate_left(struct rb_root* root, struct rb_node* node) {
  struct rb_node* right = node->right;
  struct rb_node* parent = rb_parent(node);

  node->right = right->left;
  if (right->left != NULL)
    rb_set_parent(right->left, node);

  right->left = node;
  rb_set_parent(right, parent);

  if (parent != NULL) {
    if (node == parent->left)
      parent->left = right;
    else
      parent->right = right;
  } else {
    root->node = right;
  }
  rb_set_parent(node, right);
}

static void rb_rotate_right(struct rb_root* root, struct rb_node* node) {
  struct rb_node* left = node->left;
  struct rb_node* parent = rb_parent(node);

  node->left = left->right;
  if (left->right != NULL)
    rb_set_parent(left->right, node);

  left->right = node;
  rb_set_parent(left, parent);

  if (parent != NULL) {
    if (node == parent->right)
      parent->right = left;
    else
      parent->left = left;
  } else {
    root->node = left;
  }
  rb_set_parent(node, left);
}

void rb_link_node(struct rb_node* node, struct rb_node* parent, struct rb_node** rb_link) {
  node->parent = parent;
  node->color = RB_RED;
  node->left = node->right = NULL;
  *rb_link = node;
}

void rb_insert_color(struct rb_root* root, struct rb_node* node) {
  struct rb_node* parent, *gparent;

  while ((parent = rb_parent(node)) && rb_is_red(parent)) {
    gparent = rb_parent(parent);

    if (parent == gparent->left) {
      struct rb_node* uncle = gparent->right;
      if (uncle && rb_is_red(uncle)) {
        rb_set_black(uncle);
        rb_set_black(parent);
        rb_set_red(gparent);
        node = gparent;
        continue;
      }

      if (parent->right == node) {
        struct rb_node* tmp;
        rb_rotate_left(root, parent);
        tmp = parent;
        parent = node;
        node = tmp;
      }

      rb_set_black(parent);
      rb_set_red(gparent);
      rb_rotate_right(root, gparent);
    } else {
      struct rb_node* uncle = gparent->left;
      if (uncle && rb_is_red(uncle)) {
        rb_set_black(uncle);
        rb_set_black(parent);
        rb_set_red(gparent);
        node = gparent;
        continue;
      }

      if (parent->left == node) {
        struct rb_node* tmp;
        rb_rotate_right(root, parent);
        tmp = parent;
        parent = node;
        node = tmp;
      }

      rb_set_black(parent);
      rb_set_red(gparent);
      rb_rotate_left(root, gparent);
    }
  }

  rb_set_black(root->node);
}

void rb_insert_fixup(struct rb_root* root, struct rb_node* node) {
  rb_insert_color(root, node);
}

static void rb_erase_color(struct rb_root* root, struct rb_node* node, struct rb_node* parent) {
  struct rb_node* other;

  while ((!node || rb_is_black(node)) && node != root->node) {
    if (parent->left == node) {
      other = parent->right;
      if (rb_is_red(other)) {
        rb_set_black(other);
        rb_set_red(parent);
        rb_rotate_left(root, parent);
        other = parent->right;
      }
      if ((!other->left || rb_is_black(other->left)) &&
          (!other->right || rb_is_black(other->right))) {
        rb_set_red(other);
        node = parent;
        parent = rb_parent(node);
      } else {
        if (!other->right || rb_is_black(other->right)) {
          rb_set_black(other->left);
          rb_set_red(other);
          rb_rotate_right(root, other);
          other = parent->right;
        }
        rb_set_color(other, rb_color(parent));
        rb_set_black(parent);
        rb_set_black(other->right);
        rb_rotate_left(root, parent);
        node = root->node;
        break;
      }
    } else {
      other = parent->left;
      if (rb_is_red(other)) {
        rb_set_black(other);
        rb_set_red(parent);
        rb_rotate_right(root, parent);
        other = parent->left;
      }
      if ((!other->left || rb_is_black(other->left)) &&
          (!other->right || rb_is_black(other->right))) {
        rb_set_red(other);
        node = parent;
        parent = rb_parent(node);
      } else {
        if (!other->left || rb_is_black(other->left)) {
          rb_set_black(other->right);
          rb_set_red(other);
          rb_rotate_left(root, other);
          other = parent->left;
        }
        rb_set_color(other, rb_color(parent));
        rb_set_black(parent);
        rb_set_black(other->left);
        rb_rotate_right(root, parent);
        node = root->node;
        break;
      }
    }
  }
  if (node)
    rb_set_black(node);
}

void rb_erase(struct rb_root* root, struct rb_node* node) {
  struct rb_node* child, *parent;
  enum rb_color color;

  if (!node->left)
    child = node->right;
  else if (!node->right)
    child = node->left;
  else {
    struct rb_node* old = node, *left;

    node = node->right;
    while ((left = node->left) != NULL)
      node = left;

    if (rb_parent(old)) {
      if (rb_parent(old)->left == old)
        rb_parent(old)->left = node;
      else
        rb_parent(old)->right = node;
    } else
      root->node = node;

    child = node->right;
    parent = rb_parent(node);
    color = rb_color(node);

    if (parent == old) {
      parent = node;
    } else {
      if (child)
        rb_set_parent(child, parent);
      parent->left = child;

      node->right = old->right;
      rb_set_parent(old->right, node);
    }

    node->parent = old->parent;
    node->color = old->color;
    node->left = old->left;
    rb_set_parent(old->left, node);

    goto color_fixup;
  }

  parent = rb_parent(node);
  color = rb_color(node);

  if (child)
    rb_set_parent(child, parent);

  if (parent) {
    if (parent->left == node)
      parent->left = child;
    else
      parent->right = child;
  } else
    root->node = child;

color_fixup:
  if (color == RB_BLACK)
    rb_erase_color(root, child, parent);
}

void rb_replace_node(struct rb_root* root, struct rb_node* victim, struct rb_node* new) {
  struct rb_node* parent = rb_parent(victim);

  if (parent) {
    if (victim == parent->left)
      parent->left = new;
    else
      parent->right = new;
  } else {
    root->node = new;
  }

  if (victim->left)
    rb_set_parent(victim->left, new);
  if (victim->right)
    rb_set_parent(victim->right, new);

  *new = *victim;
}

struct rb_node* rb_first(const struct rb_root* root) {
  struct rb_node* n;

  n = root->node;
  if (!n)
    return NULL;
  while (n->left)
    n = n->left;
  return n;
}

struct rb_node* rb_last(const struct rb_root* root) {
  struct rb_node* n;

  n = root->node;
  if (!n)
    return NULL;
  while (n->right)
    n = n->right;
  return n;
}

struct rb_node* rb_next(const struct rb_node* node) {
  struct rb_node* parent;

  if (rb_parent(node) == node)
    return NULL;

  if (node->right) {
    node = node->right;
    while (node->left)
      node = node->left;
    return (struct rb_node*)node;
  }

  while ((parent = rb_parent(node)) && node == parent->right)
    node = parent;

  return parent;
}

struct rb_node* rb_prev(const struct rb_node* node) {
  struct rb_node* parent;

  if (rb_parent(node) == node)
    return NULL;

  if (node->left) {
    node = node->left;
    while (node->right)
      node = node->right;
    return (struct rb_node*)node;
  }

  while ((parent = rb_parent(node)) && node == parent->left)
    node = parent;

  return parent;
}

struct rb_node* rb_find(struct rb_root* root, const void* key, 
                       int (*cmp)(const void* key, const struct rb_node* node)) {
  struct rb_node* node = root->node;

  while (node) {
    int result = cmp(key, node);

    if (result < 0)
      node = node->left;
    else if (result > 0)
      node = node->right;
    else
      return node;
  }

  return NULL;
}

struct rb_node* rb_find_first(struct rb_root* root, const void* key,
                             int (*cmp)(const void* key, const struct rb_node* node)) {
  struct rb_node* node = root->node;
  struct rb_node* match = NULL;

  while (node) {
    int result = cmp(key, node);

    if (result <= 0) {
      if (result == 0)
        match = node;
      node = node->left;
    } else {
      node = node->right;
    }
  }

  return match;
}

void rb_augment_path(struct rb_node* node, void (*augment)(struct rb_node*)) {
  while (node) {
    augment(node);
    node = rb_parent(node);
  }
}

static int rb_is_valid_rbtree_helper(struct rb_node* node, int* black_height) {
  if (!node) {
    *black_height = 1;
    return 1;
  }

  if (rb_is_red(node)) {
    if ((node->left && rb_is_red(node->left)) ||
        (node->right && rb_is_red(node->right))) {
      return 0;
    }
  }

  int left_black_height, right_black_height;
  if (!rb_is_valid_rbtree_helper(node->left, &left_black_height) ||
      !rb_is_valid_rbtree_helper(node->right, &right_black_height)) {
    return 0;
  }

  if (left_black_height != right_black_height) {
    return 0;
  }

  *black_height = left_black_height + (rb_is_black(node) ? 1 : 0);
  return 1;
}

int rb_is_valid_rbtree(struct rb_root* root) {
  if (!root->node) {
    return 1;
  }

  if (rb_is_red(root->node)) {
    return 0;
  }

  int black_height;
  return rb_is_valid_rbtree_helper(root->node, &black_height);
}