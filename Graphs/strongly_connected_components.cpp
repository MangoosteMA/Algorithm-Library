class strongly_connected_components {
private:
    int n;
    std::vector<std::vector<int>> g;

    int cur_id, cur_scc;
    std::vector<int> id, f, scc, st;

    void dfs(int v) {
        st.push_back(v);
        id[v] = f[v] = cur_id++;

        for (auto u : g[v]) {
            if (id[u] == -1) {
                dfs(u);
                f[v] = std::min(f[v], f[u]);
            } else if (scc[u] == -1) {
                f[v] = std::min(f[v], id[u]);
            }
        }

        if (id[v] == f[v]) {
            int u;
            do {
                u = st.back();
                st.pop_back();
                scc[u] = cur_scc;
            } while (u != v);
            cur_scc++;
        }
    }

public:
    strongly_connected_components(int n = 0) : n(n), g(n) {}

    // Adds directed edge (from -> to)
    void add(int from, int to) {
        g[from].push_back(to);
    }

    // Returns for each vertex the scc index from [0, number_of_scc).
    // If u is reachable from v, then scc_index[v] <= scc_index[u].
    std::vector<int> solve() {
        cur_id = cur_scc = 0;
        id.assign(n, -1);
        f.assign(n, -1);
        scc.assign(n, -1);

        for (int v = 0; v < n; v++) {
            if (id[v] == -1) {
                dfs(v);
            }
        }

        for (auto &x : scc) {
            x = cur_scc - 1 - x;
        }
        return scc;
    }
};

class strongly_connected_components {
private:
    int n;
    std::vector<std::vector<int>> g;
    std::vector<std::vector<int>> ig;

    std::vector<bool> used;
    std::vector<int> ord;

    int cur_scc = 0;
    std::vector<int> scc;

    void dfs_topsort(int v) {
        used[v] = true;
        for (auto u : g[v]) {
            if (!used[u]) {
                dfs_topsort(u);
            }
        }
        ord.push_back(v);
    }

    void dfs_scc(int v) {
        scc[v] = cur_scc;
        for (auto u : ig[v]) {
            if (scc[u] == -1) {
                dfs_scc(u);
            }
        }
    }

public:
    strongly_connected_components(int n = 0) : n(n), g(n), ig(n) {}

    // Adds directed edge (from -> to)
    void add(int from, int to) {
        g[from].push_back(to);
        ig[to].push_back(from);
    }

    // Returns for each vertex its scc index (0-indexed).
    // If u is reachable from v, then scc_index[v] <= scc_index[u].
    std::vector<int> solve() {
        used.assign(n, false);
        ord.clear();
        for (int v = 0; v < n; v++) {
            if (!used[v]) {
                dfs_topsort(v);
            }
        }
        std::reverse(ord.begin(), ord.end());

        cur_scc = 0;
        scc.assign(n, -1);
        for (auto v : ord) {
            if (scc[v] == -1) {
                dfs_scc(v);
                cur_scc++;
            }
        }
        return scc;
    }
};
