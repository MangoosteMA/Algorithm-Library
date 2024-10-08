/*
 * Include sparse_table to use it.
 * Zero based.
 * Type T is the type of the weight of each edge (int by default).
 * Don't forget to run .build(root) method before using it.
*/
 
template<typename T = int>
struct lca_tree {
    int n;
    std::vector<std::vector<std::pair<int, T>>> g;
    std::vector<int> tin, tout, parent;
    std::vector<std::pair<int, int>> order;
    std::vector<T> depth;
 
    inline static auto merge_min = [](const std::pair<int, int> &a, const std::pair<int, int> &b) -> std::pair<int, int> {
        return a < b ? a : b;
    };
    using sparse_table_t = sparse_table<std::pair<int, int>, decltype(merge_min)>;
    sparse_table_t sparse;
 
    lca_tree(int n = 0) : n(n), g(n), sparse(merge_min) {}
 
    lca_tree(const std::vector<std::vector<std::pair<int, T>>> &graph) :
        n(graph.size()), g(graph), sparse(merge_min)
    {}
 
    lca_tree(const std::vector<std::vector<int>> &graph) : n(graph.size()), g(n), sparse(merge_min) {
        for (int v = 0; v < int(graph.size()); v++) {
            g[v].reserve(graph[v].size());
            for (int u : graph[v])
                g[v].emplace_back(u, 1);
        }
    }
 
    lca_tree(int n, const std::vector<std::pair<int, int>> &edges) : n(n), g(n), sparse(merge_min) {
        for (const auto &[v, u] : edges)
            add(v, u);
    }

    lca_tree(const lca_tree<T> &lca) :
        n(lca.n), g(lca.g), tin(lca.tin), tout(lca.tout), order(lca.order), depth(lca.depth), sparse(lca.sparse)
    {}
 
    // Adding edge (v, u) with weight w (1 by default).
    void add(int v, int u, T w = 1) {
        g[v].emplace_back(u, w);
        g[u].emplace_back(v, w);
    }
 
    int size() const {
        return n;
    }
 
    // Init function.
    void build(int root) {
        tin.resize(n);
        tout.resize(n);
        parent.assign(n, -1);
        order.clear();
        order.reserve(n - 1);
        depth.resize(n);
        int timer = 0;
 
        std::function<void(int, int)> dfs = [&](int v, int dep) {
            tin[v] = timer++;
            for (auto &[u, w] : g[v]) {
                if (u == parent[v])
                    continue;
 
                parent[u] = v;
                depth[u] = depth[v] + w;
                order.emplace_back(dep, v);
                dfs(u, dep + 1);
            }
            tout[v] = timer;
        };
        dfs(root, 0);
 
        sparse = sparse_table_t(order, merge_min);
    }
 
    int lca(int v, int u) const {
        if (v == u)
            return v;
        
        auto [l, r] = std::minmax(tin[v], tin[u]);
        return sparse.query(l, r).second;
    }
 
    T dist(int v, int u) const {
        return depth[v] - 2 * depth[lca(v, u)] + depth[u];
    }
 
    // Returns true if v == u or v is ancestor of u.
    bool is_ancestor(int v, int u) const {
        return tin[v] <= tin[u] && tout[u] <= tout[v];
    }
};
