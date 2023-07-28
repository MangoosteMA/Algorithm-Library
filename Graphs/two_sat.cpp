/*
 * Include strongly_connected_components to use it.
*/

struct two_sat {
    int n;
    strongly_connected_components g;

    two_sat(int n = 0) : n(n), g(n << 1) {}

    int size() const {
        return n;
    }

    // Add condition value(v) == value_v.
    void add(int v, bool value_v) {
        assert(0 <= v && v < n);
        g.add((v << 1) ^ value_v ^ 1, (v << 1) ^ value_v);
    }

    // Add condition (value(v) == value_v) or (value(u) == value_u).
    void add(int v, bool value_v, int u, bool value_u) {
        assert(0 <= v && v < n && 0 <= u && u < n);
        g.add((v << 1) ^ value_v ^ 1, (u << 1) ^ value_u);
        g.add((u << 1) ^ value_u ^ 1, (v << 1) ^ value_v);
    }

    // Returns an empty vector if there is no solution.
    std::vector<bool> solve() const {
        const std::vector<int> scc = g.scc();
        std::vector<bool> solution(n);
        for (int i = 0; i < n; i++) {
            if (scc[i << 1] == scc[i << 1 | 1])
                return std::vector<bool>();

            solution[i] = scc[i << 1] < scc[i << 1 | 1];
        }
        return solution;
    }

    // Returns true if solution exists.
    bool any() const {
        const std::vector<int> scc = g.scc();
        for (int i = 0; i < n; i++)
            if (scc[i << 1] == scc[i << 1 | 1])
                return false;

        return true;
    }
};
