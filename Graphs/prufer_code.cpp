// Returns prufer code (0-indexed)
std::vector<int> build_prufer_code(const std::vector<std::pair<int, int>> &edges) {
    int vertices = edges.size() + 1;
    if (vertices == 1) {
        return {};
    }

    std::vector<std::vector<int>> g(vertices);
    for (const auto &[v, u] : edges) {
        g[v].push_back(u);
        g[u].push_back(v);
    }
    std::vector<int> parent(vertices), degree(vertices);

    std::function<void(int)> dfs = [&](int v) -> void {
        degree[v] = g[v].size();
        for (auto u : g[v]) {
            g[u].erase(std::find(g[u].begin(), g[u].end(), v));
            parent[u] = v;
            dfs(u);
        }
    };
    dfs(vertices - 1);

    std::vector<int> prufer(vertices - 2);
    int pointer = 0, option = -1;

    for (int i = 0; i < vertices - 2; i++) {
        if (option == -1) {
            while (degree[pointer] > 0) {
                pointer++;
            }
            option = pointer++;
        }
        prufer[i] = parent[option];
        option = -1;
        if (--degree[prufer[i]] == 0 && prufer[i] < pointer) {
            option = prufer[i];
        }
    }
    return prufer;
}

// Returns list of edges of the decoded tree (0-indexed)
std::vector<std::pair<int, int>> decode_prufer_code(const std::vector<int> &prufer) {
    int vertices = prufer.size() + 2;
    std::vector<int> count(vertices);
    for (auto vertex : prufer) {
        count[vertex]++;
    }

    std::vector<std::pair<int, int>> edges;
    edges.reserve(vertices - 1);
    int pointer = 0, option = -1;

    for (auto vertex : prufer) {
        if (option == -1) {
            while (count[pointer] > 0) {
                pointer++;
            }
            option = pointer++;
        }
        edges.emplace_back(vertex, option);
        option = -1;
        if (--count[vertex] == 0 && vertex < pointer) {
            option = vertex;
        }
    }

    if (vertices == 2 || prufer.back() == vertices - 1) {
        while (count[pointer] > 0) {
            pointer++;
        }
        edges.emplace_back(pointer, vertices - 1);
    } else {
        edges.emplace_back(prufer.back(), vertices - 1);
    }
    return edges;
}
