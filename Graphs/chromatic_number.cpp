/*
 * n is the number of vertices.
 * edges is the list of edges (0-indexed).
 * For each subset of vertices of the graph returns it's chromatic number.
 */
std::vector<int> chromatic_number(int n, const std::vector<std::pair<int, int>> &edges) {
    std::vector<int> adj(n);
    for (const auto &[v, u] : edges) {
        adj[v] |= (1 << u);
        adj[u] |= (1 << v);
    }

    constexpr int MOD = 998244353;
    const int M = (1 << n);
    std::vector<int> independed_subsets(M);
    independed_subsets[0] = 1;

    for (int mask = 1; mask < M; mask++) {
        int v = __builtin_ctz(mask);
        independed_subsets[mask] = independed_subsets[mask ^ (1 << v)]
                                 + independed_subsets[(mask ^ (1 << v)) & ~adj[v]];
        if (independed_subsets[mask] >= MOD) {
            independed_subsets[mask] -= MOD;
        }
    }

    std::vector<int> value(M);
    for (int mask = 0; mask < M; mask++) {
        value[mask] = (n - __builtin_popcount(mask)) % 2 == 0 ? 1 : MOD - 1;
    }

    std::vector<int> chromatic(M, n);
    for (int x = 0; x < n && chromatic.back() == n; x++) {
        auto subset_sum = value;
        for (int bit = 0; bit < n; bit++) {
            for (int mask = 0; mask < M; mask++) {
                if (mask >> bit & 1) {
                    subset_sum[mask] += subset_sum[mask ^ (1 << bit)];
                    if (subset_sum[mask] >= MOD) {
                        subset_sum[mask] -= MOD;
                    }
                }
            }
        }

        for (int mask = 0; mask < M; mask++) {
            if (chromatic[mask] == n && subset_sum[mask] != 0) {
                chromatic[mask] = x;
            }
        }

        for (int mask = 0; mask < M; mask++) {
            value[mask] = static_cast<int64_t>(value[mask]) * independed_subsets[mask] % MOD;
        }
    }
    return chromatic;
}

/*
 * n is the number of vertices.
 * edges is the list of edges (0-indexed).
 * Returns color of each vertex (0-indexed).
 */
std::vector<int> find_coloring(int n, const std::vector<std::pair<int, int>> &edges) {
    std::vector<int> adj(n);
    for (const auto &[v, u] : edges) {
        adj[v] |= (1 << u);
        adj[u] |= (1 << v);
    }

    const int M = (1 << n);
    std::vector<bool> independed(M, true);
    for (int mask = 1; mask < M; mask++) {
        for (int v = 0; v < n; v++) {
            if ((mask >> v & 1) && (mask & adj[v]) != 0) {
                independed[mask] = false;
                break;
            }
        }
    }

    auto chromatic = chromatic_number(n, edges);
    int free_color = 0, vset = M - 1;
    std::vector<int> color(n, -1);

    for (int mask = M - 1; mask >= 0; mask--) {
        if ((mask & vset) == mask && chromatic[mask] == chromatic[vset] - 1 && independed[vset ^ mask]) {
            for (int v = 0; v < n; v++) {
                if ((vset ^ mask) >> v & 1) {
                    color[v] = free_color;
                }
            }
            vset = mask;
            free_color++;
        }
    }

    assert(vset == 0);
    return color;
}
