template<typename w_t>
std::vector<int> vertex_biconnected(const graph<false, w_t> &g) {
    const int n = g.size();
    std::vector<int> tin(n), tup(n);
    std::vector<bool> used(n);
    int timer = 0;

    std::function<void(int, int)> dfs_fill = [&](int v, int pedge) {
        used[v] = true;
        tin[v] = tup[v] = timer++;
        for (auto [_, u, w, id] : g[v]) {
            if (id == pedge)
                continue;

            if (!used[u]) {
                dfs_fill(u, id);
                tup[v] = std::min(tup[v], tup[u]);
            } else {
                tup[v] = std::min(tup[v], tin[u]);
            }
        }
    };

    for (int v = 0; v < n; v++)
        if (!used[v])
            dfs_fill(v, -1);

    std::vector<int> edge_component(g.edges.size(), -1);
    int current_component = 0;

    std::function<void(int, int)> dfs = [&](int v, int color) {
        used[v] = true;
        for (auto [_, u, w, id] : g[v]) {
            if ((used[u] && tin[u] < tin[v]) || tup[u] < tin[v])
                edge_component[id] = color;
            else if (!used[u])
                edge_component[id] = current_component++;

            if (!used[u])
                dfs(u, edge_component[id]);
        }
    };

    std::fill(used.begin(), used.end(), false);
    for (int v = 0; v < n; v++)
        if (!used[v])
            dfs(v, -1);
    
    return edge_component;
}
