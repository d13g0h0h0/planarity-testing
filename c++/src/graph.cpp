#include "planarity_testing/graph.hpp"

#include <vector>
#include <set>
#include <map>
#include <deque>
#include <stack>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <stdexcept>

void Graph::dfs_iterative(Vertex start, BiCompContext& context) const
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
        if (f.child_index < adj_list.at(u).size())
        {
            auto it = adj_list.at(u).begin();
            std::advance(it, f.child_index);
            Vertex v = *it;
            f.child_index++;

            if (v == f.parent) continue;
            if (!context.visited[v])
            {
                f.children_count++;
                context.edge_stack.emplace(u, v);
                context.visited[v] = true;
                disc[v] = low_point[v] = ++timer;
                call_stack.push({v, u, 0, 0, false});
            }
            else if (disc[v] < disc[u])
            {
                context.edge_stack.emplace(u, v);
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

void Graph::calc_params() const
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

Graph::Graph(const std::set<Edge>& pairs)
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

Graph::Graph(const std::vector<Vertex>& vertices, const AdjMap& edges) : Graph(
    std::set<Vertex>{vertices.begin(), vertices.end()},
    edges){}

Graph::Graph(const std::set<Vertex>& vertices, const AdjMap& edges)
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

Graph Graph::get_cycle(const std::vector<Vertex>& cycle)
{
    std::set<Edge> edges;
    const Vertex first = cycle.front();
    Vertex u = first;
    size_t i = 1;
    const size_t size = cycle.size();
    Vertex v = cycle.at(i++);
    edges.insert({u, v});
    while (i < size - 1)
    {
        Vertex temp = v;
        v = cycle.at(i++);
        u = temp;
        edges.insert({u, v});
    }
    edges.insert({v, first});
    return Graph{edges};
}

void Graph::add_vertex(Vertex v)
{
    vertex_set.insert(v);
    if (!adj_list.contains(v))
    {
        adj_list[v] = {};
    }
}

void Graph::add_edge(Vertex u, Vertex v)
{
    vertex_set.insert(u);
    vertex_set.insert(v);
    adj_list[u].insert(v);
    adj_list[v].insert(u);
    dirty_bit = true;
}

size_t Graph::get_edge_count() const
{
    if (dirty_bit)
    {
        calc_params();
    }
    return edge_count;
}

size_t Graph::get_vertex_count() const
{
    if (dirty_bit)
    {
        calc_params();
    }
    return vertex_count;
}

const std::set<Graph::Vertex>& Graph::vertices() const
{
    return vertex_set;
}

const Graph::AdjMap& Graph::edges() const
{
    return adj_list;
}

std::set<Graph::Edge> Graph::edges_iterator() const
{
    std::set<Edge> result{};
    for (const auto& [u, snd] : adj_list)
    {
        for (auto v : snd)
        {
            result.insert({u, v});
        }
    }
    return result;
}

bool Graph::contains(const Vertex v) const
{
    return vertices().contains(v);
}

std::vector<Graph> Graph::get_connected_components()
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
        result.emplace_back(component, adj_list);
    }
    return result;
}

std::vector<Graph> Graph::get_biconnected_components() const
{
    std::vector<Graph> result{};
    BiCompContext context{};
    for (Vertex v : vertex_set)
    {
        if (!context.visited[v])
        {
            dfs_iterative(v, context);
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
    std::ranges::transform(context.result,
                           std::back_insert_iterator<std::vector<Graph>>(result),
                           [this](const std::set<Vertex>& set)
                           {
                               return Graph(set, this->adj_list);
                           });


    return result;
}