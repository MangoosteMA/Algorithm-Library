template<typename w_t>
std::vector<bool> find_bridges(const graph<false, w_t> &g) {
    const int n = g.size();
    std::vector<int> tin(n, -1), tup(n);
    std::vector<bool> is_bridge(g.edges.size());
    int timer = 0;

    std::function<void(int, int)> dfs = [&](int v, int pedge) {
        tin[v] = tup[v] = timer++;
        for (auto &[_, u, w, id] : g[v]) {
            if (id == pedge)
                continue;
            
            if (tin[u] == -1) {
                dfs(u, id);
                is_bridge[id] = tup[u] > tin[v];
                tup[v] = std::min(tup[v], tup[u]);
            } else {
                tup[v] = std::min(tup[v], tin[u]);
            }
        }
    };

    for (int v = 0; v < n; v++)
        if (tin[v] == -1)
            dfs(v, -1);

    return is_bridge;
}

template<typename w_t>
std::vector<int> compress_graph(const graph<false, w_t> &g) {
    const int n = g.size();
    auto is_bridge = find_bridges(g);
    std::vector<int> component(n, -1);
    int current_component = 0;

    std::function<void(int)> dfs = [&](int v) {
        component[v] = current_component;
        for (auto &[_, u, w, id] : g[v])
            if (!is_bridge[id] && component[u] == -1)
                dfs(u);
    };

    for (int v = 0; v < n; v++)
        if (component[v] == -1)
            dfs(v), current_component++;
    
    return component;
}
