namespace FFT {
    constexpr int MOD = 998'244'353;

    namespace mod_operations {
        int add(int a, int b) {
            a += b;
            return a - MOD * (a >= MOD);
        }

        int sub(int a, int b) {
            a -= b;
            return a + MOD * (a < 0);
        }

        int mult(int a, int b) {
            return int64_t(a) * b % MOD;
        }

        int power(int a, int b) {
            int prod = 1;
            for (; b; b >>= 1, a = mult(a, a))
                if (b & 1)
                    prod = mult(prod, a);

            return prod;
        }

        int inv(int a) {
            return power(a, MOD - 2);
        }
    } // namespace mod_operations

    void fft(std::vector<int> &a) {
        using namespace mod_operations;

        if (a.empty())
            return;

        static auto get_primitive_root = [&]() {
            if (MOD == 998'244'353)
                return 3;

            if (MOD == 786433)
                return 10;

            std::vector<int> primes;
            int value = MOD - 1;
            for (int i = 2; i * i <= value; i++)
                if (value % i == 0) {
                    primes.push_back(i);
                    while (value % i == 0)
                        value /= i;
                }

            if (value != 1)
                primes.push_back(value);

            for (int r = 2;; r++) {
                bool ok = true;
                for (auto p : primes)
                    if (power(r, (MOD - 1) / p) == 1) {
                        ok = false;
                        break;
                    }

                if (ok)
                    return r;
            }
        };

        static int primitive_root = get_primitive_root();

        int n = int(a.size());
        assert((n & (n - 1)) == 0);
        int lg = std::__lg(n);

        static std::vector<int> reversed_mask;
        if (int(reversed_mask.size()) != n) {
            reversed_mask.resize(n);
            for (int mask = 1; mask < n; mask++)
                reversed_mask[mask] = (reversed_mask[mask >> 1] >> 1) + ((mask & 1) << (lg - 1));
        }

        static std::vector<int> roots;
        if (int(roots.size()) != lg) {
            roots.resize(lg);
            for (int i = 0; i < lg; i++)
                roots[i] = power(primitive_root, (MOD - 1) / (2 << i));
        }

        for (int i = 0; i < n; i++)
            if (reversed_mask[i] < i)
                std::swap(a[i], a[reversed_mask[i]]);

        for (int len = 1; len < n; len <<= 1) {
            int root = roots[std::__lg(len)], current;
            for (int i = 0; i < n; i += (len << 1)) {
                current = 1;
                for (int j = 0; j < len; j++, current = mult(current, root)) {
                    int value = mult(a[i + j + len], current);
                    a[i + j + len] = sub(a[i + j], value);
                    a[i + j] = add(a[i + j], value);
                }
            }
        }
    }

    template<typename T>
    std::vector<int> multiply(T a_begin, T a_end, T b_begin, T b_end) {
        using namespace mod_operations;

        std::vector<int> a(a_begin, a_end);
        std::vector<int> b(b_begin, b_end);
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
            a[i] = mult(a[i], b[i]);

        fft(a);
        std::reverse(a.begin() + 1, a.end());

        int inv_n = inv(n);
        a.resize(real_size);
        for (auto &x : a)
            x = mult(x, inv_n);

        return a;
    }

    void fft_2d(std::vector<std::vector<int>> &a, bool invert) {
        using namespace mod_operations;

        for (int rot : {0, 1}) {
            for (auto &v : a) {
                fft(v);
                if (invert) {
                    std::reverse(v.begin() + 1, v.end());
                    int inv_n = inv(v.size());
                    for (auto &x : v)
                        x = mult(x, inv_n);
                }
            }

            for (int i = 0; i < int(a.size()); i++)
                for (int j = 0; j < i; j++)
                    std::swap(a[i][j], a[j][i]);
        }
    }

    template<typename T>
    std::vector<std::vector<int>> multiply_2d(T a_begin, T a_end, T b_begin, T b_end) {
        using namespace mod_operations;

        if (a_begin == a_end || b_begin == b_end || (*a_begin).empty() || (*b_begin).empty())
            return {};

        int real_size_x = std::distance(a_begin, a_end) + std::distance(b_begin, b_end) - 1;
        int real_size_y = int((*a_begin).size() + (*b_begin).size()) - 1;
        int base = 2;
        while (base < std::max(real_size_x, real_size_y))
            base <<= 1;

        auto get = [&](T a_begin, T a_end) {
            std::vector<std::vector<int>> a(base, std::vector<int>(base));
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
                a[i][j] = mult(a[i][j], b[i][j]);

        fft_2d(a, true);
        a.resize(real_size_x);
        for (auto &v : a)
            v.resize(real_size_y);

        return a;
    }
} // namespace FFT
