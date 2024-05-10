/*
 * Assumes that cost of edge between (v, u) is equal to w_left[v] + w_right[u]
 * Calculates maximum matching of minimum cost 
*/

template<typename T>
struct vertex_weighted_matching {
    int n, m;
    std::vector<std::vector<int>> g_left;
    std::vector<std::vector<int>> g_right;
    std::vector<T> w_left;
    std::vector<T> w_right;
    std::vector<int> p_left;
    std::vector<int> p_right;

    vertex_weighted_matching(int n, int m) : n(n), m(m), g_left(n), g_right(m), w_left(n), w_right(m), p_left(n), p_right(m) {}

    void add(int v, int u) {
        assert(0 <= v && v < n);
        assert(0 <= u && u < m);
        g_left[v].push_back(u);
        g_right[u].push_back(v);
    }

    template<typename result_t>
    std::pair<int, result_t> solve_side(const std::vector<std::vector<int>> &g, const std::vector<int> &w, std::vector<int> &mate) {
        std::fill(mate.begin(), mate.end(), -1);

        std::vector<int> order(g.size());
        std::iota(order.begin(), order.end(), 0);
        std::sort(order.begin(), order.end(), [&w](int i, int j) {
            return w[i] < w[j];
        });

        int pairs = 0;
        std::vector<int> used(g.size(), -1);

        std::function<bool(int)> khun = [&](int v) -> bool {
            if (used[v] == pairs) {
                return false;
            }
            used[v] = pairs;
            for (auto u : g[v]) {
                if (mate[u] == -1) {
                    mate[u] = v;
                    return true;
                }
            }
            for (auto u : g[v]) {
                if (khun(mate[u])) {
                    mate[u] = v;
                    return true;
                }
            }
            return false;
        };

        result_t cost = 0;
        for (auto v : order) {
            if (khun(v)) {
                pairs++;
                cost += w[v];
            }
        }
        return std::make_pair(pairs, cost);
    }

    template<typename result_t>
    std::pair<int, result_t> solve() {
        auto [left_pairs, left_cost] = solve_side<result_t>(g_left, w_left, p_right);
        auto [right_pairs, right_cost] = solve_side<result_t>(g_right, w_right, p_left);
        assert(left_pairs == right_pairs);
        result_t total_cost = left_cost + right_cost;

        std::vector<int> in_deg(n + m);
        for (int i = 0; i < n; i++) {
            if (p_left[i] != -1) {
                in_deg[n + p_left[i]]++;
            }
        }
        for (int i = 0; i < m; i++) {
            if (p_right[i] != -1) {
                in_deg[p_right[i]]++;
            }
        }

        auto nxt = [&](int v) {
            if (v < n) {
                return (p_left[v] == -1 ? -1 : n + p_left[v]);
            }
            return (p_right[v - n] == -1 ? -1 : p_right[v - n]);
        };

        std::vector<int> new_p_left(n, -1);
        std::vector<int> new_p_right(m, -1);

        auto take_edge = [&](int v, int u) {
            if (v >= n) {
                std::swap(v, u);
            }
            u -= n;
            assert(new_p_left[v] == -1 && new_p_right[u] == -1);
            new_p_left[v] = u;
            new_p_right[u] = v;
        };

        std::vector<bool> used(n + m);
        for (int i = 0; i < n + m; i++) {
            if (used[i] || in_deg[i] != 0) {
                continue;
            }
            int v = i;
            for (int step = 0; v != -1; step ^= 1, v = nxt(v)) {
                used[v] = true;
                if (nxt(v) != -1 && step == 1) {
                    take_edge(v, nxt(v));
                }
            }
        }

        for (int i = 0; i < n + m; i++) {
            if (used[i]) {
                continue;
            }
            int v = i;
            for (int step = 0; !used[v]; step ^= 1, v = nxt(v)) {
                used[v] = true;
                if (step == 0) {
                    take_edge(v, nxt(v));
                }
            }
        }

        p_left = new_p_left;
        p_right = new_p_right;
        return std::make_pair(left_pairs, total_cost);
    }
};
