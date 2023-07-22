struct strongly_connected_component {
    int n;
    std::vector<std::vector<int>> g;

    strongly_connected_component(int n = 0) : n(n), g(n) {}

    int size() const {
        return n;
    }

    void add(int from, int to) {
        assert(0 <= from && from < n);
        assert(0 <= to && to < n);
        g[from].push_back(to);
    }

    // Returns for each vertex the scc id from [0, number_of_scc).
    // If u is reachable from v, then scc[v] <= scc[u].
    std::vector<int> scc() const {
        std::vector<int> num(n, -1), low(n), scc(n, -1), st;
        int v_num = 0, scc_num = 0;

        std::function<void(int)> tarjan = [&](int v) {
            st.push_back(v);
            num[v] = low[v] = v_num++;
            for (auto u : g[v]) 
                if (num[u] == -1) {
                    tarjan(u);
                    low[v] = std::min(low[v], low[u]);
                } else if (scc[u] == -1) {
                    low[v] = std::min(low[v], num[u]);
                }

            if (num[v] == low[v]) {
                while (true) {
                    int u = st.back();
                    st.pop_back();
                    scc[u] = scc_num;
                    if (u == v)
                        break;
                }
                scc_num++;
            }
        };

        for (int v = 0; v < n; v++)
            if (num[v] == -1)
                tarjan(v);

        assert(st.empty());
        for (auto &x : scc)
            x = scc_num - 1 - x;

        return scc;
    }
};

struct strongly_connected_component {
    int n;
    std::vector<std::vector<int>> g, rev_g;

    strongly_connected_component(int n = 0) : n(n), g(n), rev_g(n) {}

    int size() const {
        return n;
    }

    void add(int from, int to) {
        assert(0 <= from && from < n);
        assert(0 <= to && to < n);
        g[from].push_back(to);
        rev_g[to].push_back(from);
    }

    // Returns for each vertex the scc id from [0, number_of_scc).
    // If u is reachable from v, then scc[v] <= scc[u].
    std::vector<int> scc() const {
        std::vector<int> order;
        order.reserve(n);
        std::vector<char> used(n);

        std::function<void(int)> dfs_order = [&](int v) {
            used[v] = 1;
            for (auto u : g[v])
                if (!used[u])
                    dfs_order(u);

            order.push_back(v);
        };

        for (int i = 0; i < n; i++)
            if (!used[i])
                dfs_order(i);

        std::reverse(order.begin(), order.end());
        int current_num = 0;
        std::vector<int> scc_num(n, -1);

        std::function<void(int)> dfs_fill = [&](int v) {
            scc_num[v] = current_num;
            for (auto u : rev_g[v])
                if (scc_num[u] == -1)
                    dfs_fill(u);
        };

        for (auto i : order)
            if (scc_num[i] == -1) {
                dfs_fill(i);
                current_num++;
            }

        return scc_num;
    }
};
