/*
it is not finished
it doesn't work on test x - y = 0 with maximization x
*/

// I hate simplex method

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
    int timer;

    bool khun(int v) {
        if (used[v] == timer)
            return false;

        used[v] = timer;
        for (auto u : g[v])
            if (p_right[u] == -1) {
                p_right[u] = v;
                p_left[v] = u;
                return true;
            }

        for (auto u : g[v])
            if (khun(p_right[u])) {
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
        timer = 0;

        std::vector<int> order(n);
        std::iota(order.begin(), order.end(), 0);
        if (need_to_shuffle)
            std::shuffle(order.begin(), order.end(), std::mt19937(std::chrono::steady_clock::now().time_since_epoch().count()));

        int pairs = 0;
        for (int v : order) {
            timer++;
            pairs += khun(v);
        }
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

template<typename T>
struct simplex {
    static constexpr long double EPS = 1e-9;

    int n, m; // n equations and m variables
    std::vector<std::vector<T>> matrix;
    std::vector<T> max_function, values;

    simplex(int n = 0, int m = 0) : n(n), m(m), matrix(n, std::vector<T>(m + 1)), max_function(m), values(m) {}

    std::pair<int, int> size() const {
        return {n, m};
    }

    void add(const std::vector<T> &new_equation) {
        matrix.push_back(new_equation);
    }

    /*
    if the first parameter equals to:
      0: in that case there is no solution and the second parameter is 0
      1: the second parameter is the maximum value of the function
      2: the function is unbounded and the second parameter is 0
    */
    std::pair<int, T> solve() {
        auto aux_matrix = matrix;

        auto subtract = [&](int from, int to, T coeff) {
            if (fabs(coeff) < EPS)
                return;

            assert(from != to);
            for (int col = 0; col < int(aux_matrix[from].size()); col++)
                aux_matrix[from][col] -= aux_matrix[to][col] * coeff;
        };

        int ptr = 0;
        for (int i = 0; i < m && ptr < n; i++) {
            int row = ptr;
            while (row < n && fabs(aux_matrix[row][i]) < EPS)
                row++;

            if (row == n)
                continue;

            std::swap(aux_matrix[row], aux_matrix[ptr]);
            for (int j = ptr + 1; j < n; j++)
                subtract(j, ptr, aux_matrix[j][i] / aux_matrix[ptr][i]);

            ptr++;
        }

        #ifdef LOCAL
            std::cerr << "--------" << std::endl;
            std::cerr << "after gauss transformation" << std::endl;
            std::cerr << "aux_matrix:\n";
            for (int i = 0; i < n; i++) {
                for (auto x : aux_matrix[i]) std::cerr << x << ' ';
                std::cerr << std::endl;
            }
            std::cerr << "ptr = " << ptr << std::endl;
        #endif

        for (int row = ptr; row < n; row++)
            if (fabs(aux_matrix[row][m]) > EPS)
                return {0, 0};

        aux_matrix.resize(ptr);
        n = ptr;

        if (n == 0) {
            std::fill(values.begin(), values.end(), 0);
            for (int col = 0; col < m; col++)
                if (max_function[col] > EPS)
                    return {2, 0};

            return {1, 0};
        }

        for (auto &v : aux_matrix)
            if (v[m] < 0)
                for (auto &x : v)
                    x *= -1;

        for (int row = 0; row < n; row++) {
            aux_matrix[row].resize(n + m + 1);
            aux_matrix[row][n + m] = aux_matrix[row][m];
            aux_matrix[row][m] = 0;
            aux_matrix[row][m + row] = 1;
        }

        #ifdef LOCAL
            std::cerr << std::endl;
            std::cerr << "--------" << std::endl;
            std::cerr << "before fist simplex" << std::endl;
            std::cerr << "aux_matrix:\n";
            for (int i = 0; i < n; i++) {
                for (auto x : aux_matrix[i]) std::cerr << x << ' ';
                std::cerr << std::endl;
            }
        #endif

        std::vector<int> pos(n + m, -1);
        std::vector<T> aux_function(n + m);
        values.assign(n + m, 0);
        for (int col = m; col < n + m; col++) {
            values[col] = aux_matrix[col - m][n + m];
            pos[col] = col - m;
            aux_function[col] = -1;
        }

        auto simplex = [&](const std::vector<T> &max_function) {
            std::vector<T> coeffs(max_function.size());
            // maybe first you need to iterate over remove and only then over new_col
            // but it sounds incorrect
            while (true) {
                int best_col = -1, best_bound_index = -1;
                T best_step, best_bound;

                auto initialize_coeffs = [&](int new_col) {
                    for (int remove = 0; remove < int(max_function.size()); remove++)
                        if (pos[remove] != -1)
                            coeffs[remove] = -aux_matrix[pos[remove]][new_col] / aux_matrix[pos[remove]][remove];
                };

                for (int new_col = 0; new_col < int(max_function.size()); new_col++) {
                    if (pos[new_col] != -1)
                        continue;

                    int bound_index = -1;
                    T bound, delta_cost = max_function[new_col];

                    initialize_coeffs(new_col);
                    for (int remove = 0; remove < int(max_function.size()); remove++) {
                        if (pos[remove] == -1)
                            continue;

                        delta_cost += max_function[remove] * coeffs[remove];
                        if (coeffs[remove] > -EPS)
                            continue;

                        T cur_bound = -values[remove] / coeffs[remove];
                        if (bound_index == -1 || bound > cur_bound) {
                            bound_index = remove;
                            bound = cur_bound;
                        }
                    }

                    if (bound_index == -1) {
                        if (bound * delta_cost > EPS)
                            return false;

                        continue;
                    }

                    if (best_col == -1 || best_step < bound * delta_cost - EPS
                        || (fabs(best_step - bound * delta_cost) < EPS && max_function[new_col] > max_function[best_bound_index] + EPS)) {
                        best_col = new_col;
                        best_bound_index = bound_index;
                        best_step = bound * delta_cost;
                        best_bound = bound;
                    }
                }
                
                if (best_col == -1 || best_step < -EPS)
                    return true;

                if (best_step < EPS && max_function[best_col] < max_function[best_bound_index] + EPS)
                    return true;

                initialize_coeffs(best_col);
                values[best_col] += best_bound;
                assert(values[best_col] > -EPS);
                for (int col = 0; col < int(max_function.size()); col++)
                    if (pos[col] != -1) {
                        values[col] += coeffs[col] * best_bound;
                        assert(values[col] > -EPS);
                    }

                std::swap(pos[best_col], pos[best_bound_index]);
                for (int col = 0; col < int(max_function.size()); col++)
                    if (pos[col] != -1 && col != best_col)
                        subtract(pos[best_col], pos[col], aux_matrix[pos[best_col]][col] / aux_matrix[pos[col]][col]);

                for (int row = 0; row < n; row++)
                    if (row != pos[best_col])
                        subtract(row, pos[best_col], aux_matrix[row][best_col] / aux_matrix[pos[best_col]][best_col]);
            }
            assert(false);
        };

        bool bounded = simplex(aux_function);

        #ifdef LOCAL
            std::cerr << std::endl;
            std::cerr << "--------" << std::endl;
            std::cerr << "after fist simplex" << std::endl;
            std::cerr << "values: ";
            for (auto x : values) std::cerr << x << ' ';
            std::cerr << std::endl;
            std::cerr << "aux_matrix:\n";
            for (int i = 0; i < n; i++) {
                for (auto x : aux_matrix[i]) std::cerr << x << ' ';
                std::cerr << std::endl;
            }
            std::cerr << "pos: ";
            for (auto x : pos) std::cerr << x << ' ';
            std::cerr << std::endl;
            std::cerr << std::endl;
        #endif

        assert(bounded);
        for (int col = m; col < n + m; col++)
            if (fabs(values[col]) > EPS) {
                values.resize(m);
                return {0, 0};
            }

        std::vector<int> bad_variables;
        for (int col = m; col < n + m; col++)
            if (pos[col] != -1)
                bad_variables.push_back(col);

        if (!bad_variables.empty()) {
            std::vector<int> free_variables;
            for (int col = 0; col < m; col++)
                if (pos[col] == -1)
                    free_variables.push_back(col);

            matching mt(free_variables.size(), bad_variables.size());
            for (int right = 0; right < int(bad_variables.size()); right++)
                for (int left = 0; left < int(free_variables.size()); left++)
                    if (fabs(aux_matrix[pos[bad_variables[right]]][free_variables[left]]) > EPS)
                        mt.add(left, right);

            int tot = mt.solve(true);
            assert(tot == int(bad_variables.size()));
            for (int right = 0; right < int(bad_variables.size()); right++)
                std::swap(pos[bad_variables[right]], pos[free_variables[mt.p_right[right]]]);

            for (int col = 0; col < m; col++) {
                if (pos[col] == -1)
                    continue;

                for (int row = 0; row < n; row++)
                    if (row != pos[col])
                        subtract(row, pos[col], aux_matrix[row][col] / aux_matrix[pos[col]][col]);
            }
        }

        values.resize(m);
        pos.resize(m);
        for (int row = 0; row < n; row++) {
            aux_matrix[row][m] = aux_matrix[row][n + m];
            aux_matrix[row].resize(m + 1);
        }

        for (int col = 0; col < m; col++) {
            if (max_function[col] < EPS)
                continue;

            bool all_zero = true;
            for (int row = 0; row < n; row++)
                if (fabs(aux_matrix[row][col]) > EPS) {
                    all_zero = false;
                    break;
                }

            if (all_zero)
                return {2, 0};
        }

        for (int col1 = 0; col1 < m; col1++)
            for (int col2 = col1 + 1; col2 < m; col2++) {
                bool any_zero = false;
                for (int row = 0; row < n; row++)
                    if (fabs(aux_matrix[row][col1]) < EPS || fabs(aux_matrix[row][col2]) < EPS) {
                        any_zero = true;
                        break;
                    }

                if (any_zero)
                    continue;

                T coeff = aux_matrix[0][col1] / aux_matrix[0][col2];
                if (coeff > -EPS)
                    continue;

                bool same = true;
                for (int row = 1; row < n; row++)
                    if (fabs(coeff - aux_matrix[row][col1] / aux_matrix[row][col2]) > EPS) {
                        same = false;
                        break;
                    }

                if (same && max_function[col1] + max_function[col2] * coeff > EPS)
                    return {2, 0};
            }

        bounded = simplex(max_function);

        #ifdef LOCAL
            std::cerr << "second simplex is done" << std::endl << std::endl;
        #endif

        if (!bounded)
            return {2, 0};

        T function_value = 0;
        for (int col = 0; col < m; col++)
            function_value += values[col] * max_function[col];

        return {1, function_value};
    }
};
