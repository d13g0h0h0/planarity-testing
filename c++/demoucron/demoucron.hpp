#ifndef PLANARITY_TESTING_DEMOUCRON_HPP
#define PLANARITY_TESTING_DEMOUCRON_HPP

#include <variant>

#include "../graph/graph.hpp"
#include "../utils/find-union.hpp"

using Vertex = Graph::Vertex;
using Edge = Graph::Edge;

inline Vertex get_next_neighbor(const Vertex current, const Vertex parent, const std::map<Vertex, std::set<Vertex>>& adj_list)
{
    auto& neighbors = adj_list.at(current);
    for (auto& neighbor : neighbors)
    {
        if (neighbor != parent) return neighbor;
    }
    return current; // shouldn't happen if given a biconnected graph
}

inline std::vector<Vertex> get_cycle(const Graph& graph)
{
    std::deque<Vertex> queue;
    std::map<Vertex, bool> visited;
    auto& adj_list = graph.edges();

    const Vertex start = *graph.vertices().begin();
    visited[start] = true;
    queue.push_back(start);

    Vertex parent = start;
    Vertex current = get_next_neighbor(start, start, adj_list);
    visited[current] = true;
    queue.push_back(current);
    while (true)
    {
        const Vertex temp = current;
        current = get_next_neighbor(current, parent, adj_list);
        parent = temp;
        if (visited[current])
        {
            break;
        }
        visited[current] = true;
        queue.push_back(current);
    }
    while (queue.front() != current)
    {
        queue.pop_front();
    }
    return {queue.begin(), queue.end()};
}

struct Face
{
    size_t id;
    std::vector<Vertex> boundary;
};

struct Chord
{
    Vertex chord_u, chord_v;
};

struct Component
{
    std::set<Vertex> vertices;
};

struct Fragment
{
    std::variant<Chord, Component> fragment;
    std::set<Vertex> contacts;
};

inline std::vector<Fragment> compute_fragments(const Graph& graph, const Graph& H)
{
    auto& adj_list = graph.edges();
    FindUnion<Vertex> find_union{};
    for (auto& v : graph.vertices())
    {
        if (!H.contains(v)) find_union.add(v);
    }

    for (auto& [u, neighbors] : adj_list)
    {
        if (H.contains(u)) continue;
        for (auto& v : neighbors)
        {
            if (!H.contains(v)) find_union.link(u, v);
        }
    }
    std::map<Vertex, std::set<Vertex>> members, contacts;
    for (auto& v : graph.vertices())
    {
        if (!H.contains(v))
        {
            members[find_union.find(v)].insert(v);
        }
    }
    for (auto& [u, neighbors] : adj_list)
    {
        if (H.contains(u)) continue;
        for (auto& v : neighbors)
        {
            if (H.contains(v)) contacts[find_union.find(u)].insert(v);
        }
    }
    std::vector<Fragment> fragments;
    for (auto& [root, vertices] : members)
    {
        Fragment f = {Component{}, {}};
        std::get<Component>(f.fragment).vertices.insert(vertices.begin(), vertices.end());
        auto& c = contacts[root];
        f.contacts.insert(c.begin(), c.end());
        fragments.push_back(std::move(f));
    }

    for (auto& u : H.vertices())
    {
        for (auto& v : adj_list.at(u))
        {
            if (!H.contains(v)
                || v <= u
                || H.edges_iterator().contains({u, v})
                || H.edges_iterator().contains({v, u})) continue;
            Fragment f = {Chord{u, v}, {u, v}};
            fragments.push_back(std::move(f));
        }
    }
    return fragments;
}

inline std::vector<size_t> admissible_faces(
    const Fragment& fragment,
    const std::map<Vertex, std::set<size_t>>& vertex_faces)
{
    std::map<size_t, size_t> count;
    for (auto& v : fragment.contacts)
    {
        auto it = vertex_faces.find(v);
        if (it == vertex_faces.end()) continue;
        for (size_t face_id : it->second)
        {
            count[face_id]++;
        }
    }
    std::vector<size_t> result;
    for (auto& [face_id, c] : count)
    {
        if (c == fragment.contacts.size())
        {
            result.push_back(face_id);
        }
    }
    return result;
}

inline std::vector<Vertex> get_fragment_path(
    const Fragment& fragment,
    const Graph& graph)
{
    if (std::holds_alternative<Chord>(fragment.fragment))
    {
        auto [chord_u, chord_v] = std::get<Chord>(fragment.fragment);
        return {chord_u, chord_v};
    }
    auto& [vertices] = std::get<Component>(fragment.fragment);
    std::set allowed(vertices);
    for (auto& contact : fragment.contacts)
    {
        allowed.insert(contact);
    }

    std::map<Vertex, Vertex> parent;
    std::deque<Vertex> queue;
    const Vertex source = *fragment.contacts.begin();
    const Vertex destination = *(++fragment.contacts.begin());
    queue.push_back(source);
    parent[source] = source;
    auto& adj_list = graph.edges();

    while (!queue.empty())
    {
        Vertex current = queue.front();
        queue.pop_front();
        for (auto& next : adj_list.at(current))
        {
            if (!allowed.contains(next) ||
                parent.contains(next) ||
                (fragment.contacts.contains(current) &&
                    fragment.contacts.contains(next))) continue;
            parent[next] = current;
            queue.push_back(next);
        }
    }

    std::vector<Vertex> path;
    Vertex current = destination;
    while (current != source) {
        path.push_back(current);
        current = parent[current];
    }
    std::ranges::reverse(path);
    return path;
}

inline void embed_path(std::vector<Face>& faces,
    std::map<Vertex, std::set<size_t>>& vertex_faces,
    size_t face_id,
    const std::vector<Vertex>& path)
{
    Face old_face = faces[face_id];
    auto& boundary = old_face.boundary;
    Vertex u = path.front(), w = path.back();
    size_t index_u = 0, index_w = 0;
    size_t size = boundary.size();
    for (auto [i, count] = std::make_tuple(static_cast<size_t>(0), 0);
        i < size; i++)
    {

        if (boundary[i] == u)
        {
            index_u = i;
            ++count;
        }
        if (boundary[i] == w)
        {
            index_w = i;
            ++count;
        }
        if (count == 2) break;
    }

    std::vector<Vertex> arc1, arc2;
    for (size_t i = index_u; i != index_w; i = (i + 1) % size)
    {
        arc1.push_back(boundary[i]);
    }
    for (size_t i = index_w; i != index_u; i = (i + 1) % size)
    {
        arc2.push_back(boundary[i]);
    }
    std::vector<Vertex> face1 = arc1, face2 = arc2;
    for (auto it = path.rbegin() + 1; it != path.rend() - 1; ++it)
    {
        face1.push_back(*it);
    }
    for (auto it = path.begin() + 1; it != path.end() - 1; ++it)
    {
        face2.push_back(*it);
    }

    size_t new_id = faces.size();
    for (auto& v : boundary)
    {
        vertex_faces[v].erase(face_id);
    }
    faces[face_id].boundary = face1;
    faces.push_back(Face{ new_id, face2 });
    for (auto& v : face1)
    {
        vertex_faces[v].insert(face_id);
    }
    for (auto& v : face2)
    {
        vertex_faces[v].insert(new_id);
    }
}

inline bool is_planar_bicomp(const Graph& graph)
{
    if (graph.get_edge_count() <= 1) return true;

    std::vector<Vertex> cycle = get_cycle(graph);
    Graph H = Graph::get_cycle(cycle);

    std::vector<Face> faces = {{0, cycle}, {1, {cycle.rbegin(), cycle.rend()}}};
    std::map<Vertex, std::set<size_t>> vertex_faces;
    for (auto& v : cycle)
    {
        vertex_faces[v].insert(0);
        vertex_faces[v].insert(1);
    }
    size_t H_edge_count = H.get_edge_count();
    size_t total_edges = graph.get_edge_count();

    while (H_edge_count < total_edges)
    {
        auto fragments = compute_fragments(graph, H);
        if (fragments.empty()) break;
        size_t best_fragment = 0;
        bool best_fragment_initialized = false;
        std::vector<size_t> best_admissible;
        for (size_t i = 0; i < fragments.size(); i++)
        {
            auto admissible = admissible_faces(fragments[i], vertex_faces);
            if (admissible.empty())
            {
                return false;
            }
            if (!best_fragment_initialized || admissible.size() < best_admissible.size())
            {
                best_fragment = i;
                best_admissible = admissible;
                best_fragment_initialized = true;
            }
        }

        auto path = get_fragment_path(fragments[best_fragment], graph);
        embed_path(faces, vertex_faces, best_admissible[0], path);

        for (auto& v : path)
        {
            H.add_vertex(v);
        }
        for (size_t i = 0; i + 1 < path.size(); i++)
        {
            H.add_edge(path[i], path[i + 1]);
        }
    }
    return true;
}

inline bool is_planar_demoucron(const Graph& graph)
{
    std::vector<Graph> bicomps = graph.get_biconnected_components();
    for (const Graph& comp : bicomps)
    {
        if (!is_planar_bicomp(comp))
        {
            return false;
        }
    }
    return true;
}

#endif //PLANARITY_TESTING_DEMOUCRON_HPP