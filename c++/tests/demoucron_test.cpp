#include <gtest/gtest.h>
#include "planarity_testing/graph.hpp"
#include "planarity_testing/demoucron.hpp"


TEST(DemoucronTest, PathsPlanar)
{
    Graph p4(std::set<Graph::Edge>{{1, 2}, {2, 3}, {3, 4}});
    Graph p10(std::set<Graph::Edge>{{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 8}, {8, 9}, {9, 10}});
    ASSERT_TRUE(is_planar_demoucron(p4));
    ASSERT_TRUE(is_planar_demoucron(p10));
}

TEST(DemoucronTest, TreesPlanar)
{
    Graph bin_tree_3(std::set<Graph::Edge>{{1, 2}, {1, 3}, {2, 4}, {2, 5}, {3, 6}, {3, 7}});
    ASSERT_TRUE(is_planar_demoucron(bin_tree_3));
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

TEST(DemoucronTest, SmallCliquesPlanar)
{
    Graph k1(std::set<Graph::Edge>{});
    k1.add_vertex(1);
    Graph k2(
        std::set<Graph::Edge>{
            {1, 2}
        });
    Graph k3(
        std::set<Graph::Edge>{
            {1, 2},
            {1, 3},
            {2, 3}
        });
    Graph k4(
       std::set<Graph::Edge>{
           {1, 2},
           {1, 3},
           {1, 4},
           {2, 3},
           {2, 4},
           {3, 4}
       });
    ASSERT_TRUE(is_planar_demoucron(k1));
    ASSERT_TRUE(is_planar_demoucron(k2));
    ASSERT_TRUE(is_planar_demoucron(k3));
    ASSERT_TRUE(is_planar_demoucron(k4));
}

TEST(DemoucronTest, IncompleteK33Planar)
{
    Graph incomplete_k33(
        std::set<Graph::Edge>{
            {1, 4},
            {1, 5},
            {1, 6},
            {2, 4},
            {2, 5},
            {2, 6},
            {3, 4},
            {3, 5}
        });
    ASSERT_TRUE(is_planar_demoucron(incomplete_k33));
}

TEST(DemoucronTest, K33Nonplanar)
{
    Graph k33(
        std::set<Graph::Edge>{
            {1, 4},
            {1, 5},
            {1, 6},
            {2, 4},
            {2, 5},
            {2, 6},
            {3, 4},
            {3, 5},
            {3, 6}
    });
    ASSERT_FALSE(is_planar_demoucron(k33));
}

TEST(DemoucronTest, PetersenPlanar)
{
    Graph petersen(
        std::set<Graph::Edge>{
            {1, 2},
            {2, 3},
            {3, 4},
            {4, 5},
            {5, 1},
            {1, 6},
            {2, 7},
            {3, 8},
            {4, 9},
            {5, 10},
            {6, 8},
            {8, 10},
            {10, 7},
            {7, 9},
            {9, 6}
        });
    ASSERT_FALSE(is_planar_demoucron(petersen));
}