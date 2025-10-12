#include "test.h"
#include "helpers/rbtree.h"
#include "kernel/mm/kalloc.h"

struct test_node {
    int key;
    struct rb_node rb;
};

static int test_cmp(const void* key, const struct rb_node* node) {
    const int* k = (const int*)key;
    struct test_node* tn = rb_entry(node, struct test_node, rb);
    return *k - tn->key;
}

static int test_node_cmp(const struct rb_node* a, const struct rb_node* b) {
    struct test_node* ta = rb_entry(a, struct test_node, rb);
    struct test_node* tb = rb_entry(b, struct test_node, rb);
    return ta->key - tb->key;
}

static struct test_node* create_test_node(int key) {
    struct test_node* node = (struct test_node*)kalloc();
    if (node) {
        node->key = key;
        rb_init_node(&node->rb);
    }
    return node;
}

static void insert_test_node(struct rb_root* root, int key) {
    struct test_node* new_node = create_test_node(key);
    if (!new_node) return;
    
    rb_insert(root, &new_node->rb, test_node_cmp);
}

static void test_basic_operations(void) {
    test_start("Red-Black Tree Basic Operations");
    
    RB_ROOT(root);
    assert(rb_empty(&root));
    
    // Test insertion
    insert_test_node(&root, 10);
    assert(!rb_empty(&root));
    assert(rb_is_valid_rbtree(&root));
    
    insert_test_node(&root, 5);
    insert_test_node(&root, 15);
    assert(rb_is_valid_rbtree(&root));
    
    // Test finding
    int key = 10;
    struct rb_node* found = rb_find(&root, &key, test_cmp);
    assert(found != NULL);
    struct test_node* tn = rb_entry(found, struct test_node, rb);
    assert(tn->key == 10);
    
    key = 99;
    found = rb_find(&root, &key, test_cmp);
    assert(found == NULL);
}

static void test_complex_insertion(void) {
    test_start("Red-Black Tree Complex Insertion");
    
    RB_ROOT(root);
    
    // Insert values that will trigger rotations
    int values[] = {10, 5, 15, 2, 7, 12, 20, 1, 3, 6, 8, 11, 13, 17, 25};
    int num_values = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < num_values; i++) {
        insert_test_node(&root, values[i]);
        assert(rb_is_valid_rbtree(&root));
    }
    
    // Verify all values can be found
    for (int i = 0; i < num_values; i++) {
        struct rb_node* found = rb_find(&root, &values[i], test_cmp);
        assert(found != NULL);
        struct test_node* tn = rb_entry(found, struct test_node, rb);
        assert(tn->key == values[i]);
    }
}

static void test_traversal(void) {
    test_start("Red-Black Tree Traversal");
    
    RB_ROOT(root);
    
    int values[] = {10, 5, 15, 2, 7, 12, 20};
    int num_values = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < num_values; i++) {
        insert_test_node(&root, values[i]);
    }
    
    // Test rb_first and rb_last
    struct rb_node* first = rb_first(&root);
    assert(first != NULL);
    struct test_node* first_tn = rb_entry(first, struct test_node, rb);
    assert(first_tn->key == 2); // minimum value
    
    struct rb_node* last = rb_last(&root);
    assert(last != NULL);
    struct test_node* last_tn = rb_entry(last, struct test_node, rb);
    assert(last_tn->key == 20); // maximum value
    
    // Test ordered traversal
    int prev_key = -1;
    struct rb_node* pos;
    rb_for_each(pos, &root) {
        struct test_node* tn = rb_entry(pos, struct test_node, rb);
        assert(tn->key > prev_key); // should be in sorted order
        prev_key = tn->key;
    }
}

static void test_deletion(void) {
    test_start("Red-Black Tree Deletion");
    
    RB_ROOT(root);
    
    int values[] = {10, 5, 15, 2, 7, 12, 20, 1, 3, 6, 8};
    int num_values = sizeof(values) / sizeof(values[0]);
    
    // Insert all values
    for (int i = 0; i < num_values; i++) {
        insert_test_node(&root, values[i]);
    }
    assert(rb_is_valid_rbtree(&root));
    
    // Delete some nodes and verify tree remains valid
    int key = 5;
    struct rb_node* to_delete = rb_find(&root, &key, test_cmp);
    assert(to_delete != NULL);
    rb_erase(&root, to_delete);
    assert(rb_is_valid_rbtree(&root));
    
    // Verify the node is no longer findable
    struct rb_node* not_found = rb_find(&root, &key, test_cmp);
    assert(not_found == NULL);
    
    // Delete root node
    key = 10;
    to_delete = rb_find(&root, &key, test_cmp);
    assert(to_delete != NULL);
    rb_erase(&root, to_delete);
    assert(rb_is_valid_rbtree(&root));
    
    not_found = rb_find(&root, &key, test_cmp);
    assert(not_found == NULL);
}

void test_rbtree_all(void) {
    test_basic_operations();
    test_complex_insertion();
    test_traversal();
    test_deletion();
}