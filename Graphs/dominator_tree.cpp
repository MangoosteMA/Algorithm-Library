struct dominator_tree {
    template<typename T>
    struct link_eval {
        std::vector<std::pair<int, T>> par;

        link_eval(int n) : par(n) {
            for (int i = 0; i < n; i++)
                par[i].first = i;
        }

        void set_value(int v, const T &val) {
            par[v].second = val;
        }

        int root(int v) {
            if (par[v].first == v)
                return v;

            int u = par[v].first;
            par[v].first = root(u);
            par[v].second = std::min(par[v].second, par[u].second);
            return par[v].first;
        }

        T eval(int v) {
            root(v);
            return par[v].second;
        }

        void link(int v, int u) {
            par[root(v)].first = root(u);
        }
    };

    int n;
    std::vector<std::vector<int>> g;

    dominator_tree(int n = 0) : n(n), g(n) {}

    void add(int from, int to) {
        g[from].push_back(to);
    }

    std::vector<int> solve(int s) {
        int timer = 0;
        std::vector<int> tin(n, -1), sdom(n, -1), order;
        order.reserve(n);
        std::vector<std::vector<int>> dfs_tree(n), input_edges(n);

        std::function<void(int)> init_dfs = [&](int v) {
            tin[v] = timer++;
            order.push_back(v);
            dfs_tree[v].reserve(g[v].size());
            for (auto u : g[v]) {
                if (tin[u] == -1) {
                    dfs_tree[v].push_back(u);
                    init_dfs(u);
                }
                if (tin[v] < tin[u])
                    sdom[u] = tin[v];
                else if (u != v)
                    input_edges[u].push_back(v);
            }
        };
        init_dfs(s);

        link_eval<int> dsu(n);
        std::vector<std::vector<int>> to_process(n);

        std::function<void(int)> find_sdoms = [&](int v) {
            std::reverse(dfs_tree[v].begin(), dfs_tree[v].end());
            for (auto u : dfs_tree[v])
                find_sdoms(u);

            for (auto u : input_edges[v])
                sdom[v] = std::min(sdom[v], dsu.eval(u));

            if (v != s)
                to_process[order[sdom[v]]].push_back(v);

            dsu.set_value(v, sdom[v]);
            for (auto u : dfs_tree[v])
                dsu.link(u, v);
        };
        find_sdoms(s);

        link_eval<std::pair<int, int>> linker(n);
        std::vector<int> dom(n, -1);
        std::vector<std::pair<int, int>> set_later;
        set_later.reserve(n);

        std::function<void(int)> find_doms = [&](int v) {
            for (auto u : dfs_tree[v])
                find_doms(u);

            for (auto u : to_process[v]) {
                int p = linker.eval(u).second;
                if (p == u)
                    dom[u] = order[sdom[u]];
                else
                    set_later.emplace_back(u, p);
            }
            linker.set_value(v, {sdom[v], v});
            for (auto u : dfs_tree[v])
                linker.link(u, v);
        };
        find_doms(s);

        std::reverse(set_later.begin(), set_later.end());
        for (auto &[v, u] : set_later)
            dom[v] = dom[u];

        dom[s] = s;
        return dom;
    }
};
