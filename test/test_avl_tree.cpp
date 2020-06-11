#include "../avl_tree.h"
#include "test_common.h"

using tree_t = cym::avl_tree<int>;
using node_t = tree_t::avl_node;
using dir_t = tree_t::iteration_direction;

void test_avl_node() {
    node_t root(10);
    EXPECT_EQ(root.height(), 0)
    node_t left(5);
    node_t left_left(1);
    node_t left_right(7);
    left.append_left(&left_left);
    left.append(&left_right, dir_t::Right);
    dir_t dir = cym::avl_tree<int>::iteration_direction::Left;
    root.append(&left, dir);
    EXPECT_EQ(left.balance_factor(), 0)
    EXPECT_EQ(root.balance_factor(), -2)
    EXPECT_EQ(root.need_rotation(), true)
    EXPECT_EQ(left.height(), 1);
    EXPECT_EQ(root.height(), 2);
}

void test_avl_tree_insert() {
    tree_t tree;
    tree.insert(1);
    EXPECT_EQ(tree.height(), 1);
    tree.insert(2);
    EXPECT_EQ(tree.height(), 2);
}

TEST_MAIN(test_avl_node(); test_avl_tree_insert();)
