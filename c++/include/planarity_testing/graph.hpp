#ifndef PLANARITY_TESTING_GRAPH_HPP
#define PLANARITY_TESTING_GRAPH_HPP

#include <vector>
#include <set>
#include <map>
#include <deque>
#include <stack>
#include <iterator>

class Graph
{
public:
    using Vertex = size_t;
    using Edge = std::pair<Vertex, Vertex>;
private:
    mutable size_t vertex_count{};
    mutable size_t edge_count{};
    mutable Vertex max_vertex{};
    std::set<Vertex> vertex_set;
    mutable bool dirty_bit{};

    using AdjMap = std::map<Vertex, std::set<Vertex>>;
    AdjMap adj_list;

    struct StackFrame
    {
        Vertex current;
        Vertex parent;
        size_t child_index;
        size_t children_count;
        bool is_root;
    };

    struct BiCompContext
    {
        std::map<Vertex, bool> visited;
        std::stack<Edge> edge_stack;
        std::vector<std::set<Vertex>> result;
    };

    void dfs_iterative(Vertex start, BiCompContext& context) const;
    void calc_params() const;
public:
    explicit Graph(const std::set<Edge>& pairs);
    explicit Graph(const std::vector<Vertex>& vertices, const AdjMap& edges);
    explicit Graph(const std::set<Vertex>& vertices, const AdjMap& edges);

    static Graph get_cycle(const std::vector<Vertex>& cycle);

    void add_vertex(Vertex v);

    void add_edge(Vertex u, Vertex v);

    [[nodiscard]] size_t get_edge_count() const;

    [[nodiscard]] size_t get_vertex_count() const;

    [[nodiscard]] const std::set<Vertex>& vertices() const;

    [[nodiscard]] const AdjMap& edges() const;

    [[nodiscard]] std::set<Edge> edges_iterator() const;

    [[nodiscard]] bool contains(Vertex v) const;

    [[nodiscard]] std::vector<Graph> get_connected_components();

    ///
    /// Returns a @verbatim std::vector @endverbatim of biconnected components.
    /// @return a vector of biconnected components
    [[nodiscard]] std::vector<Graph> get_biconnected_components() const;
};

#endif //PLANARITY_TESTING_GRAPH_HPP