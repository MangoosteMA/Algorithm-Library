namespace FFT {
    template<typename T>
    void normilize(T &poly) {
        while (!poly.empty() && poly.back() == 0)
            poly.pop_back();
    }

    template<int mod>
    void fft(std::vector<static_modular_int<mod>> &a) {
        using mint = static_modular_int<mod>;

        if (a.empty())
            return;

        static mint primitive_root = mint::primitive_root();
        int n = int(a.size());
        assert((n & (n - 1)) == 0);
        int lg = std::__lg(n);

        static std::vector<int> reversed_mask;
        if (int(reversed_mask.size()) != n) {
            reversed_mask.resize(n);
            for (int mask = 1; mask < n; mask++)
                reversed_mask[mask] = (reversed_mask[mask >> 1] >> 1) + ((mask & 1) << (lg - 1));
        }

        static std::vector<mint> roots;
        if (int(roots.size()) != lg) {
            roots.resize(lg);
            for (int i = 0; i < lg; i++)
                roots[i] = primitive_root.power((mod - 1) / (2 << i));
        }

        for (int i = 0; i < n; i++)
            if (reversed_mask[i] < i)
                std::swap(a[i], a[reversed_mask[i]]);

        for (int len = 1; len < n; len <<= 1) {
            mint root = roots[std::__lg(len)];
            for (int i = 0; i < n; i += (len << 1)) {
                mint current = 1;
                for (int j = 0; j < len; j++, current *= root) {
                    mint value = a[i + j + len] * current;
                    a[i + j + len] = a[i + j] - value;
                    a[i + j] = a[i + j] + value;
                }
            }
        }
    }

    template<int mod, typename T>
    std::vector<static_modular_int<mod>> multiply(T a_begin, T a_end, T b_begin, T b_end) {
        using mint = static_modular_int<mod>;

        std::vector<mint> a(a_begin, a_end);
        std::vector<mint> b(b_begin, b_end);
        if (a_begin == a_end || b_begin == b_end)
            return {};

        if (std::min(a.size(), b.size()) <= 20 || std::max(a.size(), b.size()) <= 64) {
            std::vector<mint> prod(int(a.size() + b.size()) - 1);
            for (int i = 0; i < int(a.size()); i++)
                for (int j = 0; j < int(b.size()); j++)
                    prod[i + j] += a[i] * b[j];
            
            return prod;
        }

        int real_size = int(a.size() + b.size()) - 1;
        int n = 1;
        while (n < real_size)
            n <<= 1;

        a.resize(n);
        b.resize(n);
        fft(a), fft(b);

        for (int i = 0; i < n; i++)
            a[i] *= b[i];

        fft(a);
        std::reverse(a.begin() + 1, a.end());

        mint inv_n = mint(n).inv();
        a.resize(real_size);
        for (auto &x : a)
            x *= inv_n;

        return a;
    }

    template<int mod>
    void fft_2d(std::vector<std::vector<static_modular_int<mod>>> &a, bool invert) {
        using mint = static_modular_int<mod>;

        for (int rot : {0, 1}) {
            for (auto &v : a) {
                fft(v);
                if (invert) {
                    std::reverse(v.begin() + 1, v.end());
                    mint inv_n = mint(v.size()).inv();
                    for (auto &x : v)
                        x *= inv_n;
                }
            }

            for (int i = 0; i < int(a.size()); i++)
                for (int j = 0; j < i; j++)
                    std::swap(a[i][j], a[j][i]);
        }
    }

    template<int mod, typename T>
    std::vector<std::vector<static_modular_int<mod>>> multiply_2d(T a_begin, T a_end, T b_begin, T b_end) {
        using mint = static_modular_int<mod>;

        if (a_begin == a_end || b_begin == b_end || (*a_begin).empty() || (*b_begin).empty())
            return {};

        int real_size_x = std::distance(a_begin, a_end) + std::distance(b_begin, b_end) - 1;
        int real_size_y = int((*a_begin).size() + (*b_begin).size()) - 1;
        int base = 2;
        while (base < std::max(real_size_x, real_size_y))
            base <<= 1;

        auto get = [&](T a_begin, T a_end) {
            std::vector<std::vector<mint>> a(base, std::vector<mint>(base));
            for (int i = 0; a_begin != a_end; i++, a_begin++)
                for (int j = 0; j < int((*a_begin).size()); j++)
                    a[i][j] = (*a_begin)[j];

            return a;
        };

        auto a = get(a_begin, a_end), b = get(b_begin, b_end);
        fft_2d(a, false);
        fft_2d(b, false);

        for (int i = 0; i < base; i++)
            for (int j = 0; j < base; j++)
                a[i][j] *= b[i][j];

        fft_2d(a, true);
        a.resize(real_size_x);
        for (auto &v : a)
            v.resize(real_size_y);

        return a;
    }

    // returns p^-1 modulo x^degree
    template<int mod, typename T>
    std::vector<static_modular_int<mod>> inverse(T begin, T end, int degree) {
        using mint = static_modular_int<mod>;

        int size = std::distance(begin, end);
        assert(size > 0 && *(begin) != 0);

        std::vector<mint> inv(std::min(degree, 128)), have(inv.size());
        mint start_inv = 1 / *begin;
        for (int i = 0; i < int(inv.size()); i++) {
            inv[i] = ((i == 0 ? 1 : 0) - have[i]) * start_inv;
            int steps = std::min(size, int(have.size()) - i);
            for (int j = 0; j < steps; j++)
                have[i + j] += inv[i] * *(begin + j);
        }

        for (int power = inv.size(); power < degree; power <<= 1) {
            auto product = multiply<mod>(inv.begin(), inv.end(), begin, begin + std::min(size, 2 * power));
            for (int i = 0; i < std::min<int>(product.size(), 2 * power); i++)
                product[i] = (i == 0 ? 2 : 0) - product[i];

            inv = multiply<mod>(inv.begin(), inv.end(),
                                product.begin(), product.begin() + std::min<int>(product.size(), 2 * power));
            inv.resize(2 * power);
        }
        inv.resize(degree);
        return inv;
    }

    // returns (quotient, remainder)
    template<int mod, typename T>
    std::pair<std::vector<static_modular_int<mod>>, std::vector<static_modular_int<mod>>>
        divide(T a_begin, T a_end, T b_begin, T b_end) {
        using mint = static_modular_int<mod>;

        std::vector<mint> a(a_begin, a_end), b(b_begin, b_end);
        normilize(a), normilize(b);
        assert(!b.empty());
        int n = int(a.size()), m = int(b.size());
        if (n < m)
            return {{}, a};

        if (n <= 128 || m <= 64) {
            std::vector<mint> quotient(n - m + 1);
            mint inv_b = 1 / b.back();
            for (int i = n - 1; i >= m - 1; i--) {
                int pos = i - m + 1;
                quotient[pos] = a[i] * inv_b;
                for (int j = 0; j < m; j++)
                    a[pos + j] -= b[j] * quotient[pos];
            }
            normilize(a);
            return {quotient, a};
        }

        std::reverse(a.begin(), a.end());
        std::reverse(b.begin(), b.end());
        auto inv_b = inverse<mod>(b.begin(), b.end(), n - m + 1);
        auto quotient = multiply<mod>(a.begin(), a.end(), inv_b.begin(), inv_b.end());
        quotient.resize(n - m + 1);
        std::reverse(quotient.begin(), quotient.end());
        normilize(quotient);

        std::reverse(a.begin(), a.end());
        std::reverse(b.begin(), b.end());
        auto product = multiply<mod>(quotient.begin(), quotient.end(), b.begin(), b.end());
        assert(int(product.size()) == int(a.size()));

        std::vector<mint> remainder(m);
        for (int i = 0; i < m; i++)
            remainder[i] = a[i] - product[i];
        
        normilize(remainder);
        return {quotient, remainder};
    }

    template<int mod, typename T>
    std::vector<static_modular_int<mod>> multipoint_evaluation(T p_begin, T p_end, T x_begin, T x_end) {
        using mint = static_modular_int<mod>;

        int n = std::distance(x_begin, x_end);
        if (n == 0)
            return {};

        static constexpr int N = 32;
        const int tree_size = (n + N - 1) / N;
        std::vector<std::vector<mint>> tree(tree_size << 1);
        for (int v = 0; v < tree_size; v++) {
            int from = v * N, to = std::min(n, from + N);
            tree[tree_size + v].resize(to - from + 1);
            tree[tree_size + v][0] = 1;
            for (int i = from; i < to; i++) {
                for (int j = i - from; j >= 0; j--) {
                    tree[tree_size + v][j + 1] += tree[tree_size + v][j];
                    tree[tree_size + v][j] *= -*(x_begin + i);
                }
            }
        }
        for (int v = tree_size - 1; v > 0; v--)
            tree[v] = FFT::multiply<MOD>(tree[v << 1].begin(), tree[v << 1].end(),
                                         tree[v << 1 | 1].begin(), tree[v << 1 | 1].end());
        tree[1] = divide<MOD>(p_begin, p_end, tree[1].begin(), tree[1].end()).second;
        for (int v = 2; v < 2 * tree_size; v++)
            tree[v] = divide<MOD>(tree[v >> 1].begin(), tree[v >> 1].end(),
                                  tree[v].begin(), tree[v].end()).second;

        std::vector<mint> eval(n);
        for (int v = 0; v < tree_size; v++) {
            int from = v * N, to = std::min(n, from + N);
            for (int i = from; i < to; i++) {
                mint cur_power = 1;
                for (int j = 0; j < int(tree[tree_size + v].size()); j++, cur_power *= *(x_begin + i))
                    eval[i] += cur_power * tree[tree_size + v][j];
            }
        }
        return eval;
    }
} // namespace FFT
