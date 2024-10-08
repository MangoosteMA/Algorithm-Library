/*
 ! WARNING: type T must have sufficient capacity to store the result.
 * Returns h of size |f| + |g| - 1, such that: h_k = min_{i + j = k} f_i + g_j.
 * Require: f and g are two convex downward arrays: f[i + 1] - f[i] <= f[i + 2] - f[i + 1].
 * Require: f and g are not empty.
 */
template<typename T>
std::vector<T> min_plus_convolution_convex_convex(const std::vector<T> &f, const std::vector<T> &g) {
    assert(!f.empty() && !g.empty());
    int n = f.size(), m = g.size();
    for (int i = 0; i + 2 < n; i++) {
        assert(f[i + 1] - f[i] <= f[i + 2] - f[i + 1] && "f is not convex");
    }
    for (int i = 0; i + 2 < m; i++) {
        assert(g[i + 1] - g[i] <= g[i + 2] - g[i + 1] && "g is not convex");
    }

    std::vector<T> conv(n + m - 1);
    for (int i = 0, j = 0; i < n && j < m;) {
        conv[i + j] = f[i] + g[j];
        if (j + 1 == m || (i + 1 != n && f[i + 1] - f[i] <= g[j + 1] - g[j])) {
            i++;
        } else {
            j++;
        }
    }
    return conv;
}

/*
 ! WARNING: type T must have sufficient capacity to store the result.
 * Returns h of size |f| + |g| - 1, such that: h_k = max_{i + j = k} f_i + g_j.
 * Require: f and g are two convex upward arrays: f[i + 1] - f[i] >= f[i + 2] - f[i + 1].
 * Require: f and g are not empty.
 */
template<typename T>
std::vector<T> max_plus_convolution_convex_convex(std::vector<T> f, std::vector<T> g) {
    for (auto &x : f) {
        x *= -1;
    }
    for (auto &x : g) {
        x *= -1;
    }
    return min_plus_convolution_convex_convex(f, g);
}

/*
 ! WARNING: type T must have sufficient capacity to store the result.
 * Returns h of size |f| + |g| - 1, such that: h_k = min_{i + j = k} f_i + g_j.
 * Require: f is a convex downward array: f[i + 1] - f[i] <= f[i + 2] - f[i + 1].
 * Require: f and g are not empty.
 */
template<typename T>
std::vector<T> min_plus_convolution_convex_arbitrary(const std::vector<T> &f, const std::vector<T> &g) {
    assert(!f.empty() && !g.empty());
    int n = f.size(), m = g.size();
    for (int i = 0; i + 2 < n; i++) {
        assert(f[i + 1] - f[i] <= f[i + 2] - f[i + 1] && "f is not convex");
    }
    std::vector<T> conv(n + m - 1);

    auto solve = [&](auto &&self, int l, int r, int opt_l, int opt_r) {
        if (r <= l) {
            return;
        }

        int mid = (l + r) / 2;
        int cur_opt = -1;
        for (int i = std::max(opt_l, mid - n + 1); i < opt_r && i <= mid; i++) {
            if (cur_opt == -1 || g[cur_opt] + f[mid - cur_opt] > g[i] + f[mid - i]) {
                cur_opt = i;
            }
        }

        conv[mid] = g[cur_opt] + f[mid - cur_opt];
        self(self, l, mid, opt_l, cur_opt + 1);
        self(self, mid + 1, r, cur_opt, opt_r);
    };

    solve(solve, 0, conv.size(), 0, g.size());
    return conv;
}

/*
 ! WARNING: type T must have sufficient capacity to store the result.
 * Returns h of size |f| + |g| - 1, such that: h_k = max_{i + j = k} f_i + g_j.
 * Require: f is a convex upward array: f[i + 1] - f[i] >= f[i + 2] - f[i + 1].
 * Require: f and g are not empty.
 */
template<typename T>
std::vector<T> max_plus_convolution_convex_arbitrary(std::vector<T> f, std::vector<T> g) {
    for (auto &x : f) {
        x *= -1;
    }
    for (auto &x : g) {
        x *= -1;
    }
    return min_plus_convolution_convex_arbitary(f, g);
}
