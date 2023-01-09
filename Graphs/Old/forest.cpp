template<typename T>
struct forest : public graph<T> {
    using graph<T>::n;
    using graph<T>::edges;
    using graph<T>::g;

    std::vector<int> root;
    std::vector<int> parent;
    std::vector<int> previous_edge;
    std::vector<int> depth;
    std::vector<int> subtree_size;
    std::vector<T> dist;
    std::vector<int> tin;
    std::vector<int> tout;

    forest(int n = 0) : graph<T>(n) {}

    int add(int from, int to, T cost = T(1)) {
        assert(0 <= from && from < n && 0 <= to && to < n);
        int id = int(edges.size());
        g[from].push_back(id);
        g[to].push_back(id);
        edges.push_back({from, to, cost});
        return id;
    }

    bool is_ancestor(int v, int u) const {
        return tin[v] <= tin[u] && tout[u] <= tout[v];
    }

    bool is_root(int v) const {
        return root[v] == v;
    }

    bool connected(int v, int u) const {
        return root[v] == root[u];
    }

    void set_default() {
        root.resize(n);
        std::iota(root.begin(), root.end(), 0);
        parent.resize(n);
        std::iota(parent.begin(), parent.end(), 0);
        previous_edge.assign(n, -1);
        depth.assign(n, 0);
        subtree_size.assign(n, 0);
        dist.assign(n, T(0));
        tin.assign(n, 0);
        tout.assign(n, 0);
    }

    void set_root(int new_root, int start_timer = std::numeric_limits<int>::max()) {
        if (root.empty())
            set_default();

        static int timer = 0;
        if (start_timer != std::numeric_limits<int>::max())
            timer = start_timer;

        std::function<void(int)> dfs = [&](int v) {
            subtree_size[v] = 1;
            tin[v] = timer++;
            for (const int id : g[v]) {
                int u = v ^ edges[id].from ^ edges[id].to;
                if (u == parent[v])
                    continue;

                root[u] = root[v];
                parent[u] = v;
                previous_edge[u] = id;
                depth[u] = depth[v] + 1;
                dist[u] = dist[v] + edges[id].cost;

                dfs(u);
                subtree_size[v] += subtree_size[u];
            }
            tout[v] = timer;

            for (int i = 0; i < int(g[v].size()); i++) {
                int u = v ^ edges[g[v][i]].from ^ edges[g[v][i]].to;
                if (u == parent[v]) {
                    for (int j = i; j > 0; j--)
                        std::swap(g[v][j], g[v][j - 1]);

                    break;
                }
            }
        };

        parent[new_root] = root[new_root] = new_root;
        previous_edge[new_root] = -1;
        depth[new_root] = 0;
        dist[new_root] = T(0);
        dfs(new_root);
    }

    void init() {
        set_default();
        for (int v = 0; v < n; v++)
            if (root[v] == v)
                set_root(v);
    }

    std::vector<int> order(int start) const {
        std::vector<int> order;

        std::function<void(int, int)> dfs = [&](int v, int parent) {
            order.push_back(v);
            for (const int id : g[v]) {
                int u = v ^ edges[id].from ^ edges[id].to;
                if (u == parent)
                    continue;

                dfs(u, v);
            }
        };

        dfs(start, start);
        return order;
    }
};
