#ifndef PLANARITY_TESTING_GRAPH_HPP
#define PLANARITY_TESTING_GRAPH_HPP

#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <iterator>
#include <deque>
#include <numeric>
#include <stdexcept>

class Graph
{
    using Vertex = size_t;
    using Edge = std::pair<Vertex, Vertex>;
    size_t vertex_count;
    size_t edge_count;
    Vertex max_vertex;
    std::set<Vertex> vertex_set;

    using AdjMap = std::map<Vertex, std::set<Vertex>>;
    AdjMap adj_list;
public:
    explicit Graph(const std::set<Edge>& pairs)
    {
        Vertex max = 0;
        vertex_set = {};
        for (auto [fst, snd] : pairs)
        {
            max = std::max({max, fst, snd});
            adj_list[fst].emplace(snd);
            adj_list[snd].emplace(fst);
            vertex_set.emplace(fst);
            vertex_set.emplace(snd);
        }
        max_vertex = max;
        vertex_count = vertex_set.size();
        edge_count = std::accumulate(
            adj_list.begin(),
            adj_list.end(),
            size_t{},
            [](const size_t acc, const auto& pair)
            {
                auto& [key, value] = pair;
                return acc + value.size();
            }) / 2;
    }

    explicit Graph(const std::set<Vertex>& vertices, const AdjMap& edges)
    {
        vertex_set = vertices;
        vertex_count = vertices.size();
        max_vertex = *vertices.rbegin();
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
        edge_count = std::accumulate(
            adj_list.begin(),
            adj_list.end(),
            size_t{},
            [](const size_t acc, const auto& pair)
            {
                auto& [key, value] = pair;
                return acc + value.size();
            }) / 2;
    }

    [[nodiscard]] size_t edges() const
    {
        return edge_count;
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
};

#endif //PLANARITY_TESTING_GRAPH_HPP