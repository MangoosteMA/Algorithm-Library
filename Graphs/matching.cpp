struct matching {
    int n, m;
    std::vector<std::vector<int>> g;
    std::vector<int> p_left, p_right;

    matching(int n = 0, int m = 0) :
        n(n), m(m), g(n), p_left(n, -1), p_right(m, -1)
    {}

    std::pair<int, int> size() const {
        return {n, m};
    }

    void add(int left, int right) {
        g[left].push_back(right);
    }

    std::vector<int> used;
    int pairs = 0;

    bool kuhn(int v) {
        if (used[v] == pairs + 1)
            return false;

        used[v] = pairs + 1;
        for (auto u : g[v])
            if (p_right[u] == -1) {
                p_right[u] = v;
                p_left[v] = u;
                return true;
            }

        for (auto u : g[v])
            if (kuhn(p_right[u])) {
                p_right[u] = v;
                p_left[v] = u;
                return true;
            }

        return false;
    }

    int solve(bool need_to_shuffle = false) {
        std::fill(p_left.begin(), p_left.end(), -1);
        std::fill(p_right.begin(), p_right.end(), -1);
        used.assign(n, 0);

        std::vector<int> order(n);
        std::iota(order.begin(), order.end(), 0);
        if (need_to_shuffle) {
            std::shuffle(order.begin(), order.end(), std::mt19937(std::chrono::steady_clock::now().time_since_epoch().count()));
            for (auto &v : g)
                std::shuffle(v.begin(), v.end(), std::mt19937(std::chrono::steady_clock::now().time_since_epoch().count()));
        }

        pairs = 0;
        for (int v : order)
            pairs += kuhn(v);

        return pairs;
    }

    void dfs(int v) {
        if (used[v])
            return;

        used[v] = 1;
        for (auto u : g[v])
            if (u != p_left[v])
                dfs(p_right[u]);
    }

    std::pair<std::vector<int>, std::vector<int>> minimum_vertex_covering(bool need_to_shuffle = false) {
        int pairs = solve(need_to_shuffle);
        used.assign(n, 0);

        for (int i = 0; i < n; i++)
            if (p_left[i] == -1)
                dfs(i);

        std::vector<int> left;
        std::vector<bool> used_right(m);

        for (int i = 0; i < n; i++)
            if (!used[i]) {
                left.push_back(i);
            } else if (p_left[i] != -1) {
                for (auto j : g[i])
                    used_right[j] = true;
            }

        std::vector<int> right;
        for (int i = 0; i < m; i++)
            if (used_right[i])
                right.push_back(i);

        assert(int(left.size() + right.size()) == pairs);
        return std::make_pair(left, right);
    }
};
