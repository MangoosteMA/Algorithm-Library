template<typename T>
struct lca_forest : public forest<T> {
    struct sparse_table {
        std::vector<std::vector<int>> sparse;

        sparse_table() {}
        sparse_table(const std::vector<int> &a) {
            if (a.empty())
                return;

            int n = int(a.size());
            int lg = std::__lg(n);
            sparse.reserve(lg);
            sparse.emplace_back(a);
            for (int level = 1; level <= lg; level++) {
                sparse.emplace_back(std::vector<int>(n - (1 << level) + 1));
                for (int i = 0; i < int(sparse.back().size()); i++)
                    sparse[level][i] = std::min(sparse[level - 1][i], sparse[level - 1][i + (1 << (level - 1))]);
            }
        }

        int query(int l, int r) const {
            assert(l < r);
            int level = std::__lg(r - l);
            return std::min(sparse[level][l], sparse[level][r - (1 << level)]);
        }
    };

    using forest<T>::n;
    using forest<T>::edges;
    using forest<T>::g;
    using forest<T>::root;
    using forest<T>::parent;
    using forest<T>::previous_edge;
    using forest<T>::depth;
    using forest<T>::subtree_size;
    using forest<T>:: dist;
    using forest<T>::tin;
    using forest<T>::tout;

    sparse_table rmq;
    std::vector<int> path;
    std::vector<int> events;

    lca_forest(int n = 0) : forest<T>(n) {}

    void set_root(int new_root, bool init_rmq = true) {
        forest<T>::set_root(new_root);
        path.reserve(int(path.size()) + subtree_size[new_root] - 1);
        events.reserve(int(events.size()) + subtree_size[new_root] - 1);

        std::function<void(int)> dfs = [&](int v) {
            for (const int id : g[v]) {
                int u = v ^ edges[id].from ^ edges[id].to;
                if (u == parent[v])
                    continue;

                path.push_back(v);
                events.push_back(tin[v]);
                dfs(u);
            }
        };

        dfs(new_root);
        if (init_rmq)
            rmq = sparse_table(events);
    }

    void init() {
        forest<T>::set_default();
        for (int v = 0; v < n; v++)
            if (root[v] == v)
                set_root(v, false);

        rmq = sparse_table(events);
    }

    int lca(int v, int u) const {
        if (v == u)
            return v;

        std::tie(v, u) = minmax(tin[v], tin[u]);
        return path[rmq.query(v, u)];
    }

    int length(int v, int u) const {
        return depth[v] + depth[u] - 2 * depth[lca(v, u)];
    }

    T distance(int v, int u) const {
        return dist[v] + dist[u] - T(2) * dist[lca(v, u)];
    }
};
