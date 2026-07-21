#ifndef PLANARITY_TESTING_GRAPH_HPP
#define PLANARITY_TESTING_GRAPH_HPP

#include <vector>
#include <set>
#include <map>
#include <deque>
#include <stack>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <stdexcept>

class Graph
{
public:
    using Vertex = size_t;
    using Edge = std::pair<Vertex, Vertex>;
private:
    size_t vertex_count;
    size_t edge_count;
    Vertex max_vertex;
    std::set<Vertex> vertex_set;
    bool dirty_bit;

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

    void dfs_iterative(Vertex start, BiCompContext& context)
    {
        size_t timer = 0;
        std::map<Vertex, size_t> disc, low_point;
        std::stack<StackFrame> call_stack;

        call_stack.push({start, 0, 0, 0, true});
        context.visited[start] = true;
        disc[start] = low_point[start] = ++timer;

        while (!call_stack.empty())
        {
            StackFrame &f = call_stack.top();
            Vertex u = f.current;
            if (f.child_index < adj_list[u].size())
            {
                auto it = adj_list[u].begin();
                std::advance(it, f.child_index);
                Vertex v = *it;
                f.child_index++;

                if (v == f.parent) continue;
                if (!context.visited[v])
                {
                    f.children_count++;
                    context.edge_stack.push({u, v});
                    context.visited[v] = true;
                    disc[v] = low_point[v] = ++timer;
                    call_stack.push({v, u, 0, 0, false});
                }
                else if (disc[v] < disc[u])
                {
                    context.edge_stack.push({u, v});
                    low_point[u] = std::min(low_point[u], disc[v]);
                }
            }
            else
            {
                call_stack.pop();
                if (!call_stack.empty())
                {
                    StackFrame &parent_f = call_stack.top();
                    Vertex parent_u = parent_f.current;
                    low_point[parent_u] = std::min(low_point[parent_u], low_point[u]);

                    if ((parent_f.is_root && parent_f.children_count > 1) ||
                        (!parent_f.is_root && low_point[u] >= disc[parent_u]))
                    {
                        std::set<Vertex> component;
                        while (!context.edge_stack.empty() &&
                            !(context.edge_stack.top().first == parent_u && context.edge_stack.top().second == u))
                        {
                            component.insert(context.edge_stack.top().first);
                            component.insert(context.edge_stack.top().second);
                            context.edge_stack.pop();
                        }
                        component.insert(context.edge_stack.top().first);
                        component.insert(context.edge_stack.top().second);
                        context.edge_stack.pop();
                        context.result.push_back(component);
                    }
                }
            }
        }
    }

    void calc_params()
    {
        vertex_count = vertex_set.size();
        max_vertex = vertex_set.empty() ? 0 : *vertex_set.rbegin();
        edge_count = std::accumulate(
            adj_list.begin(),
            adj_list.end(),
            size_t{},
            [](const size_t acc, const auto& pair)
            {
                auto& [key, value] = pair;
                return acc + value.size();
            }) / 2;
        dirty_bit = false;
    }

public:
    explicit Graph(const std::set<Edge>& pairs)
    {
        vertex_set = {};
        for (auto [fst, snd] : pairs)
        {
            adj_list[fst].emplace(snd);
            adj_list[snd].emplace(fst);
            vertex_set.emplace(fst);
            vertex_set.emplace(snd);
        }
        calc_params();
    }

    explicit Graph(const std::set<Vertex>& vertices, const AdjMap& edges)
    {
        vertex_set = vertices;
        adj_list = {};
        for (auto vertex : vertices)
        {
            try
            {
                std::ranges::set_intersection(vertices, edges.at(vertex),
                                          std::inserter(adj_list[vertex], adj_list[vertex].begin()));
            }
            catch ([[maybe_unused]] const std::out_of_range& err)
            {
                adj_list[vertex] = {};
            }
        }
        calc_params();
    }

    void addEdge(Vertex u, Vertex v)
    {
        vertex_set.insert(u);
        vertex_set.insert(v);
        adj_list[u].insert(v);
        adj_list[v].insert(u);
        dirty_bit = true;
    }

    [[nodiscard]] size_t get_edge_count()
    {
        if (dirty_bit)
        {
            calc_params();
        }
        return edge_count;
    }

    [[nodiscard]] size_t get_vertex_count()
    {
        if (dirty_bit)
        {
            calc_params();
        }
        return vertex_count;
    }

    [[nodiscard]] std::vector<Graph> get_connected_components()
    {
        std::vector<Graph> result{};

        std::set<Vertex> vertices = vertex_set;
        std::deque<Vertex> vertex_queue{};
        std::map<Vertex, bool> visited;
        while (!vertices.empty())
        {
            Vertex current = *vertices.begin();
            std::set component = {current};
            visited[current] = true;
            vertices.erase(current);
            vertex_queue.emplace_back(current);
            while (!vertex_queue.empty())
            {
                current = vertex_queue.front();
                vertex_queue.pop_front();
                for (Vertex neighbor : adj_list[current])
                {
                    if (!visited[neighbor])
                    {
                        component.emplace(neighbor);
                        visited[neighbor] = true;
                        vertices.erase(neighbor);
                        vertex_queue.emplace_back(neighbor);
                    }
                }
            }
            result.push_back(Graph(component, adj_list));
        }
        return result;
    }

    ///
    /// Returns an @verbatim std::vector @endverbatim of biconnected components.
    /// @return a vector of biconnected components
    [[nodiscard]] std::vector<Graph> get_biconnected_components()
    {
        std::vector<Graph> result{};
        BiCompContext context{};
        for (Vertex v : vertex_set)
        {
            if (!context.visited[v])
            {
                dfs_iterative(v, context);
                if (!context.edge_stack.empty())
                {
                    std::set<Vertex> component;
                    while (!context.edge_stack.empty())
                    {
                        component.insert(context.edge_stack.top().first);
                        component.insert(context.edge_stack.top().second);
                        context.edge_stack.pop();
                    }
                    context.result.push_back(component);
                }
            }
        }
        std::ranges::transform(context.result,
                               std::back_insert_iterator<std::vector<Graph>>(result),
                               [this](const std::set<Vertex>& set)
                               {
                                   return Graph(set, this->adj_list);
                               });


        return result;
    }
};

#endif //PLANARITY_TESTING_GRAPH_HPP