#include "definitions/c++/graph.hpp"
#include <iostream>

int main()
{
    Graph g(std::set<std::pair<size_t, size_t>>{{1, 2}, {2, 3}, {3, 5}});
    std::cout << "Edge count: " << g.edges() << std::endl;
    std::cout << "Connected component count: " << g.get_connected_components().size() << std::endl;
}