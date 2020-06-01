#include "../avl_tree.h"
#include "test_common.h"

using node_t = cym::avl_tree<int>::avl_node;

void test_avl_node() {
    node_t root(10);
    EXPECT_EQ(root.height(), 0)
    node_t left(5);
    node_t left_left(1);
    node_t left_right(7);
    left.append_left(&left_left);
    left.append_right(&left_right);
    root.append_left(&left);
    EXPECT_EQ(left.balance_factor(), 0)
    EXPECT_EQ(root.balance_factor(), -2)
    EXPECT_EQ(root.need_rotation(), true)
    EXPECT_EQ(left.height(), 1);
    EXPECT_EQ(root.height(), 2);
}

TEST_MAIN(test_avl_node())
