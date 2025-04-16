#include <gtest/gtest.h>
#include <string>
#include "VecTree.h"

TEST(TreeTest, Insert)
{
    VecTree<std::string> tree;
    
    tree.insert_as_root("0");

    EXPECT_TRUE(tree.contains("0"));
    EXPECT_EQ(tree.size(), 1);

    //EXPECT_TRUE(false);
}