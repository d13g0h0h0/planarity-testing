#include <gtest/gtest.h>
#include "planarity_testing/graph.hpp"
#include "planarity_testing/demoucron.hpp"


TEST(DemoucronTest, PathsPlanar)
{
    Graph p4(std::set<Graph::Edge>{{1, 2}, {2, 3}, {3, 5}});
    ASSERT_TRUE(is_planar_demoucron(p4));
}

TEST(DemoucronTest, IncompleteK5Planar)
{
    Graph incomplete_k5(std::set<Graph::Edge>{{1, 2}, {1, 3}, {1, 4}, {1, 5}, {2, 3}, {2, 4}, {2, 5}, {3, 4}, {3, 5}});
    ASSERT_TRUE(is_planar_demoucron(incomplete_k5));
}

TEST(DemoucronTest, K5Nonplanar)
{
    Graph k5(std::set<Graph::Edge>{{1, 2}, {1, 3}, {1, 4}, {1, 5}, {2, 3}, {2, 4}, {2, 5}, {3, 4}, {3, 5}, {4, 5}});
    ASSERT_FALSE(is_planar_demoucron(k5));
}