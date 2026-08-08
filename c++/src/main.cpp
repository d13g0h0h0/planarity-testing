#include <iostream>
#include "../include/planarity_testing/graph.hpp"
#include "../include/planarity_testing/demoucron.hpp"

int main()
{
    Graph g(std::set<Graph::Edge>{{1, 2}, {2, 3}, {1, 3}, {3, 5}});
    g.add_vertex(7);
    std::cout << "Edge count: " << g.get_edge_count() << std::endl;
    std::cout << "Connected component count: " << g.get_connected_components().size() << std::endl;
    std::cout << "Biconnected component count: " << g.get_biconnected_components().size() << std::endl;
    std::cout << std::boolalpha;
    std::cout << "Is it planar?: " << is_planar_demoucron(g) << std::endl;
    Graph h(std::set<Graph::Edge>{{1, 4}, {1, 5}, {1, 6}, {2, 4}, {2, 5}, {2, 6}, {3, 4}, {3, 5}, {3, 6}});
    std::cout << "Is K_{3, 3} planar?: " << is_planar_demoucron(h) << std::endl;
    Graph k(std::set<Graph::Edge>{{1, 2}, {1, 3}, {1, 4}, {1, 5}, {2, 3}, {2, 4}, {2, 5}, {3, 4}, {3, 5}});
    std::cout << "Is K_5 minus one edge planar?: " << is_planar_demoucron(k) << std::endl;
    k.add_edge(4, 5);
    std::cout << "Is K_5 planar?: " << is_planar_demoucron(k) << std::endl;
}
