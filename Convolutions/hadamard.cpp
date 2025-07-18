int hadamard_sign(int x, int y) {
    return __builtin_parity(x & y) ? -1 : 1;
}

template<typename T>
void hadamard(std::vector<T> &a, bool inverse = false) {
    int n = a.size();
    assert(!(n & (n - 1)));
    for (int length = 1; length < n; length <<= 1) {
        for (int i = 0; i < n; i += length << 1) {
            for (int j = 0; j < length; j++) {
                T v = a[i + j], u = a[i + length + j];
                a[i + j] = v + u;
                a[i + length + j] = v - u;
            }
        }
    }

    if (inverse) {
        for (auto &x : a) {
            x /= n;
        }
    }
}

template<typename result_t, typename T1, typename T2>
std::vector<result_t> hadamard_convolution(T1 a_begin, T1 a_end, T2 b_begin, T2 b_end) {
    if (a_begin == a_end || b_begin == b_end) {
        return {};
    }

    std::vector<result_t> a(a_begin, a_end);
    std::vector<result_t> b(b_begin, b_end);
    int n = std::max(a.size(), b.size());
    while (n & (n - 1)) {
        n++;
    }

    a.resize(n);
    b.resize(n);
    hadamard(a);
    hadamard(b);
    for (int i = 0; i < n; i++) {
        a[i] *= b[i];
    }

    hadamard(a, true);
    return a;
}

template<typename T>
class xor_binoms_multiplier {
private:
    int n;
    int accumulated_xor = 0;
    std::vector<pair<T, T>> prod;

public:
    xor_binoms_multiplier(int n = 0) : n(n), prod(n, {1, 0}) {}

    // multiply by a * x^{d1} + b * x^{d2}
    void multiply(T a, int d1, T b, int d2) {
        accumulated_xor ^= d1;
        int deg = d1 ^ d2;
        auto [c, d] = prod[deg];
        prod[deg].first = a * c + b * d;
        prod[deg].second = a * d + b * c;
    }

    // O(n * log(n))
    std::vector<T> product() {
        std::vector<T> f(n), g(n);
        for (int i = 0; i < n; i++) {
            f[i] = prod[i].first + prod[i].second;
            g[i] = prod[i].first - prod[i].second;
        }

        for (int len = 1; len < n; len <<= 1) {
            for (int l = 0; l < n; l += (len << 1)) {
                for (int i = 0; i < len; i++) {
                    int x = l + i;
                    int y = l + len + i;

                    T fl = f[x];
                    T fr = f[y];
                    T gl = g[x];
                    T gr = g[y];

                    f[x] = fl * fr;
                    f[y] = fl * gr;
                    g[x] = gl * gr;
                    g[y] = gl * fr;
                }
            }
        }

        hadamard(f, true);

        for (int i = 0; i < n; i++) {
            int j = i ^ accumulated_xor;
            if (i < j) {
                std::swap(f[i], f[j]);
            }
        }
        return f;
    }
};
