namespace FFT {
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

    template<int mod, typename T>
    std::vector<static_modular_int<mod>> inverse(T begin, T end, int degree) {
        using mint = static_modular_int<mod>;

        int size = std::distance(begin, end);
        if (size == 0)
            return std::vector<mint>(degree + 1);

        std::vector<mint> inv{1 / *begin};
        for (int power = 1; power <= degree; power <<= 1) {
            auto product = multiply<mod>(inv.begin(), inv.end(), begin, begin + std::min(size, 2 * power));
            for (int i = 0; i < std::min<int>(product.size(), 2 * power); i++)
                product[i] = (i == 0 ? 2 : 0) - product[i];

            inv = multiply<mod>(inv.begin(), inv.end(),
                                product.begin(), product.begin() + std::min<int>(product.size(), 2 * power));
            inv.resize(2 * power);
        }
        inv.resize(degree + 1);
        return inv;
    }
} // namespace FFT
