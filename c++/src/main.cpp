#include <iostream>
#include "planarity_testing/graph.hpp"
#include "planarity_testing/demoucron.hpp"

int main()
{
    std::cout << std::boolalpha;
    Graph k5_1(std::set<Graph::Edge>{{1, 2}, {1, 3}, {1, 4}, {1, 5}, {2, 3}, {2, 4}, {2, 5}, {3, 4}, {3, 5}});
    std::cout << "Is K_5 minus one edge planar?: " << is_planar_demoucron(k5_1) << std::endl;
}
