#include "definitions/c++/graph.hpp"
#include <iostream>

int main()
{
    Graph g(std::set<Graph::Edge>{{1, 2}, {2, 3}, {1, 3}, {3, 5}});
    std::cout << "Edge count: " << g.get_edge_count() << std::endl;
    std::cout << "Connected component count: " << g.get_connected_components().size() << std::endl;
    std::cout << "Biconnected component count: " << g.get_biconnected_components().size() << std::endl;
}