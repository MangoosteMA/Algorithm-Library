/*
 * Zero based.
 * This is an implementation of Edmonds algorithm in O(V^3).
 */
class general_matching {
private:
    int n;
    std::vector<std::vector<int>> g;
    std::vector<int> match, nxt, par, used, join;
    std::queue<int> que;
    int used_timer;

    void greedy_match() {
        for (int v = 0; v < n; v++) {
            for (auto u : g[v]) {
                if (match[v] == -1 && match[u] == -1) {
                    match[v] = u;
                    match[u] = v;
                    break;
                }
            }
        }
    }

    int root(int v) {
        return par[v] == v ? v : par[v] = root(par[v]);
    }

    int lca(int v, int u) {
        for (used_timer++;; std::swap(v, u)) {
            if (v == -1) {
                continue;
            }
            v = root(v);
            if (std::exchange(used[v], used_timer) == used_timer) {
                return v;
            }
            v = (match[v] == -1 ? -1 : nxt[match[v]]);
        }
    }

    void fix_path(int v, int u, int w) {
        while (root(v) != w) {
            join.push_back(v);
            join.push_back(match[v]);
            nxt[v] = u;
            u = match[v];
            que.push(u);
            v = nxt[u];
        }
    }

    int expand(int start) {
        nxt.assign(n, -1);
        par.resize(n);
        std::iota(par.begin(), par.end(), 0);
        used.assign(n, -1);
        used_timer = 0;

        que = std::queue<int>();
        que.push(start);

        while (!que.empty()) {
            int v = que.front();
            que.pop();
            for (auto u : g[v]) {
                if (match[v] == u || root(v) == root(u) || nxt[u] != -1) {
                    continue;
                }

                if (u == start || (match[u] != -1 && nxt[match[u]] != -1)) {
                    int w = lca(v, u);
                    join.clear();
                    fix_path(v, u, w);
                    fix_path(u, v, w);
                    for (auto v : join) {
                        par[root(v)] = w;
                    }
                    continue;
                }

                nxt[u] = v;
                if (match[u] == -1) {
                    return u;
                }
                que.push(match[u]);
            }
        }
        return -1;
    }

public:
    general_matching(int n = 0) : n(n), g(n) {}

    int size() const {
        return n;
    }

    void add(int v, int u) {
        g[v].push_back(u);
        g[u].push_back(v);
    }

    /*
     * Returns the edges of the maximum matching.
     * Complexity: O(V^3). In practice it is much faster.
     */
    std::vector<std::pair<int, int>> solve() {
        match.assign(n, -1);
        greedy_match();

        for (int v = 0; v < n; v++) {
            if (match[v] != -1) {
                continue;
            }

            int x = expand(v);
            while (x != -1) {
                int y = nxt[x];
                match[x] = y;
                match[y] = std::exchange(x, match[y]);
            }
        }

        std::vector<std::pair<int, int>> edges;
        for (int v = 0; v < n; v++) {
            if (v < match[v]) {
                edges.emplace_back(v, match[v]);
            }
        }
        return edges;
    }
};

/*
 * Zero based.
 * This is an implementation of Edmonds algorithm in O(V (V logV + E)).
 * In +- random test or special graphs it works slower than O(V^3).
 */
class general_matching {
private:
    int n;
    std::vector<std::vector<int>> g;
    std::vector<int> match, nxt, hyper, par, used, join;
    std::queue<int> que;
    int used_timer;

    void greedy_match() {
        for (int v = 0; v < n; v++) {
            for (auto u : g[v]) {
                if (match[v] == -1 && match[u] == -1) {
                    match[v] = u;
                    match[u] = v;
                    break;
                }
            }
        }
    }

    int root(int v) {
        return par[v] == v ? v : par[v] = root(par[v]);
    }

    int lca(int v, int u) {
        for (used_timer++;; std::swap(v, u)) {
            if (v == -1) {
                continue;
            }
            v = root(v);
            if (std::exchange(used[v], used_timer) == used_timer) {
                return v;
            }
            v = (match[v] == -1 ? -1 : nxt[match[v]]);
        }
    }

    int fix_path(int v, int u, int w) {
        if (root(v) == w) {
            return v;
        }
        join.push_back(v);
        join.push_back(match[v]);

        nxt[v] = u;
        u = match[v];
        que.push(u);
        v = (hyper[u] == -1 ? nxt[u] : hyper[u]);
        return hyper[u] = fix_path(v, u, w);
    }

    int expand(int start) {
        nxt.assign(n, -1);
        hyper.assign(n, -1);
        par.resize(n);
        std::iota(par.begin(), par.end(), 0);

        used.assign(n, -1);
        used_timer = 0;

        que = std::queue<int>();
        que.push(start);

        while (!que.empty()) {
            int v = que.front();
            que.pop();
            for (auto u : g[v]) {
                if (match[v] == u || root(v) == root(u) || nxt[u] != -1) {
                    continue;
                }

                if (u == start || (match[u] != -1 && nxt[match[u]] != -1)) {
                    int w = lca(v, u);
                    join.clear();
                    fix_path(v, u, w);
                    fix_path(u, v, w);
                    for (auto v : join) {
                        par[root(v)] = w;
                    }
                    continue;
                }

                nxt[u] = v;
                if (match[u] == -1) {
                    return u;
                }
                que.push(match[u]);
            }
        }
        return -1;
    }

public:
    general_matching(int n = 0) : n(n), g(n) {}

    int size() const {
        return n;
    }

    void add(int v, int u) {
        g[v].push_back(u);
        g[u].push_back(v);
    }

    /*
     * Returns the edges of the maximum matching.
     * Complexity: O(V (V logV + E)). In practice it is even faster than O(VE).
     */
    std::vector<std::pair<int, int>> solve() {
        match.assign(n, -1);
        greedy_match();

        for (int v = 0; v < n; v++) {
            if (match[v] != -1) {
                continue;
            }

            int x = expand(v);
            while (x != -1) {
                int y = nxt[x];
                match[x] = y;
                match[y] = std::exchange(x, match[y]);
            }
        }

        std::vector<std::pair<int, int>> edges;
        for (int v = 0; v < n; v++) {
            if (v < match[v]) {
                edges.emplace_back(v, match[v]);
            }
        }
        return edges;
    }
};
