#ifndef PLANARITY_TESTING_FIND_UNION_HPP
#define PLANARITY_TESTING_FIND_UNION_HPP

#include <set>
#include <map>

template <class T>
class FindUnion
{
    std::set<T> set;
    std::map<T, T> parent;
    std::map<T, size_t> rank;
public:
    void add(T element)
    {
        set.insert(element);
        parent[element] = element;
        rank[element] = 0;
    }

    T& find(T& element)
    {
        T& par = parent[element];
        if (par == element)
        {
            return element;
        }
        parent[element] = parent[par];
        return find(par);
    }

    void link(T& u, T& v)
    {
        T& u_root = find(u);
        T& v_root = find(v);
        if (u_root == v_root) return;
        if (rank[u_root] > rank[v_root])
        {
            parent[v_root] = u_root;
            return;
        }
        if (rank[u_root] < rank[v_root])
        {
            parent[u_root] = v_root;
            return;
        }
        parent[u_root] = v_root;
        ++rank[v_root];
    }

    bool is_same(T& u, T& v)
    {
        return find(u) == find(v);
    }
};

#endif //PLANARITY_TESTING_FIND_UNION_HPP