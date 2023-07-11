/*
 * Include static_modular_int to use it.
 * Don't need to care about precomputing primitive root.
 * Use it like vector<mint> with extra methods.
*/

template<int mod>
class polynom_t : public std::vector<static_modular_int<mod>> {
public:
    using T = static_modular_int<mod>;
    using std::vector<T>::empty;
    using std::vector<T>::back;
    using std::vector<T>::pop_back;
    using std::vector<T>::size;
    using std::vector<T>::clear;
    using std::vector<T>::begin;
    using std::vector<T>::end;

private:
    static constexpr int EVAL_N = 1 << 5;
    static constexpr int MUL_MIN_CUT = 20;
    static constexpr int MUL_MAX_CUT = 1 << 6;
    static constexpr int DIV_N_CUT = 1 << 7;
    static constexpr int DIV_M_CUT = 1 << 6;
    static constexpr int INV_BRUTE_FORCE_SIZE = 1 << 7;

    static void fft(polynom_t<mod> &a) {
        if (a.empty())
            return;

        static T primitive_root = T::primitive_root();
        int n = int(a.size());
        assert((n & (n - 1)) == 0);
        int lg = std::__lg(n);

        static std::vector<int> reversed_mask;
        if (int(reversed_mask.size()) != n) {
            reversed_mask.resize(n);
            for (int mask = 1; mask < n; mask++)
                reversed_mask[mask] = (reversed_mask[mask >> 1] >> 1) + ((mask & 1) << (lg - 1));
        }

        static std::vector<T> roots;
        if (int(roots.size()) != lg) {
            roots.resize(lg);
            for (int i = 0; i < lg; i++)
                roots[i] = primitive_root.power((mod - 1) / (2 << i));
        }

        for (int i = 0; i < n; i++)
            if (reversed_mask[i] < i)
                std::swap(a[i], a[reversed_mask[i]]);

        for (int len = 1; len < n; len <<= 1) {
            T root = roots[std::__lg(len)];
            for (int i = 0; i < n; i += (len << 1)) {
                T current = 1;
                for (int j = 0; j < len; j++, current *= root) {
                    T value = a[i + j + len] * current;
                    a[i + j + len] = a[i + j] - value;
                    a[i + j] = a[i + j] + value;
                }
            }
        }
    }

public:
    template<typename V>
    polynom_t(const std::initializer_list<V> &lst) : std::vector<T>(lst.begin(), lst.end()) {}

    template<typename... Args>
    polynom_t(Args&&... args) : std::vector<T>(std::forward<Args>(args)...) {}

    polynom_t<mod>& resize(int n) {
        std::vector<T>::resize(n);
        return *this;
    }

    // Removes extra zeroes.
    void normalize() {
        while (!empty() && back() == T(0))
            pop_back();
    }

    // Returns -1 if all coefficients are zeroes (not O(1)!).
    int degree() const {
        int deg = int(size()) - 1;
        while (deg >= 0 && (*this)[deg] == T(0))
            deg--;

        return deg;
    }

    T eval(const T &x) const {
        T power = 1, value = 0;
        for (int i = 0; i < int(size()); i++, power *= x)
            value += (*this)[i] * power;

        return value;
    }

    // Calculates eval at the given points.
    // O(n log^2).
    std::vector<T> multipoint_evaluation(const std::vector<T> &points) const {
        const int n = points.size();
        if (n == 0)
            return {};

        const int tree_size = (n + EVAL_N - 1) / EVAL_N;
        std::vector<polynom_t<mod>> tree(tree_size << 1);
        for (int v = 0; v < tree_size; v++) {
            int from = v * EVAL_N, to = std::min(n, from + EVAL_N);
            tree[tree_size + v].resize(to - from + 1);
            tree[tree_size + v][0] = 1;
            for (int i = from; i < to; i++) {
                for (int j = i - from; j >= 0; j--) {
                    tree[tree_size + v][j + 1] += tree[tree_size + v][j];
                    tree[tree_size + v][j] *= -points[i];
                }
            }
        }
        for (int v = tree_size - 1; v > 0; v--)
            tree[v] = tree[v << 1] * tree[v << 1 | 1];

        tree[1] = (*this) % tree[1];
        for (int v = 2; v < 2 * tree_size; v++)
            tree[v] = tree[v >> 1] % tree[v];

        std::vector<T> eval(n);
        for (int v = 0; v < tree_size; v++)
            for (int i = v * EVAL_N; i < std::min(n, (v + 1) * EVAL_N); i++)
                eval[i] = tree[tree_size + v].eval(points[i]);

        return eval;
    }

    polynom_t<mod> operator-() const {
        return polynom_t(*this) * T(-1);
    }

    polynom_t<mod>& operator+=(const polynom_t<mod> &another) {
        if (size() < another.size())
            resize(another.size());
        
        for (int i = 0; i < int(another.size()); i++)
            (*this)[i] += another[i];

        return *this;
    }

    polynom_t<mod>& operator-=(const polynom_t<mod> &another) {
        if (size() < another.size())
            resize(another.size());
        
        for (int i = 0; i < int(another.size()); i++)
            (*this)[i] -= another[i];

        return *this;
    }

    polynom_t<mod>& operator*=(const polynom_t<mod> &another) {
        if (empty() || another.empty()) {
            clear();
            return *this;
        }

        if (std::min(size(), another.size()) <= MUL_MIN_CUT
            || std::max(size(), another.size()) <= MUL_MAX_CUT) {
            polynom_t<mod> product(int(size() + another.size()) - 1);
            for (int i = 0; i < int(size()); i++)
                for (int j = 0; j < int(another.size()); j++)
                    product[i + j] += (*this)[i] * another[j];

            product.swap(*this);
            return *this;
        }

        int real_size = int(size() + another.size()) - 1, n = 1;
        while (n < real_size)
            n <<= 1;

        resize(n);
        polynom_t b = another;
        b.resize(n);
        fft(*this), fft(b);
        for (int i = 0; i < n; i++)
            (*this)[i] *= b[i];

        fft(*this);
        std::reverse(begin() + 1, end());
        T inv_n = T(1) / T(n);
        resize(real_size);
        for (auto &x : *this)
            x *= inv_n;

        return *this;
    }

    // Division with remainder.
    // O(nlog).
    polynom_t<mod>& operator/=(const polynom_t<mod> &another) {
        polynom_t<mod> a(*this), b(another);
        a.normalize(), b.normalize();
        assert(!b.empty());
        int n = int(a.size()), m = int(b.size());
        if (n < m)
            return *this = {};

        if (n <= DIV_N_CUT || m <= DIV_M_CUT) {
            polynom_t<mod> quotient(n - m + 1);
            T inv_b = T(1) / b.back();
            for (int i = n - 1; i >= m - 1; i--) {
                int pos = i - m + 1;
                quotient[pos] = a[i] * inv_b;
                for (int j = 0; j < m; j++)
                    a[pos + j] -= b[j] * quotient[pos];
            }
            quotient.normalize();
            return *this = quotient;
        }

        std::reverse(a.begin(), a.end());
        std::reverse(b.begin(), b.end());
        polynom_t<mod> quotient = a * b.inv(n - m + 1);
        quotient.resize(n - m + 1);
        std::reverse(quotient.begin(), quotient.end());
        quotient.normalize();
        return *this = quotient;
    }

    // O(nlog)
    polynom_t<mod>& operator%=(const polynom_t<mod> &another) {
        *this -= (*this) / another * another;
        normalize();
        return *this;
    }

    // Returns derivative.
    // O(n).
    polynom_t<mod> derivative() const {
        polynom_t<mod> der(std::max(0, int(size()) - 1));
        for (int i = 0; i < int(der.size()); i++)
            der[i] = T(i + 1) * (*this)[i + 1];

        return der;
    }

    // Returns integral.
    // O(n).
    polynom_t<mod> integral(const T &constant = T(0)) const {
        polynom_t<mod> in(size() + 1);
        in[0] = constant;
        for (int i = 1; i < int(in.size()); i++)
            in[i] = (*this)[i - 1] / T(i);

        return in;
    }

    // Returns p^{-1} modulo x^{degree}.
    // O(nlog).
    polynom_t<mod> inv(int degree) const {
        assert(!empty() && (*this)[0] != T(0) && "polynom is not invertable");
        polynom_t<mod> inv(std::min(degree, INV_BRUTE_FORCE_SIZE)), have(inv.size());
        T start_inv = T(1) / (*this)[0];
        for (int i = 0; i < int(inv.size()); i++) {
            inv[i] = ((i == 0 ? T(1) : T(0)) - have[i]) * start_inv;
            int steps = std::min<int>(size(), int(have.size()) - i);
            for (int j = 0; j < steps; j++)
                have[i + j] += inv[i] * (*this)[j];
        }
        for (int power = inv.size(); power < degree; power <<= 1) {
            inv *= (polynom_t<mod>({2})
                - (polynom_t<mod>(begin(), begin() + std::min<int>(size(), power << 1)) * inv).resize(power << 1));
            inv.resize(std::min(degree, power << 1));
        }
        return inv.resize(degree);
    }

    // Returns log(p) modulo x^{degree}.
    // O(nlog).
    polynom_t<mod> log(int degree) const {
        assert(!empty() && (*this)[0] == T(1) && "log is not defined");
        return (derivative().resize(degree) * inv(degree)).resize(degree).integral(T(0)).resize(degree);
    }

    // Returns exp(p) modulo x^{degree}.
    // O(nlog), but with a huge constant.
    polynom_t<mod> exp(int degree) const {
        assert(!empty() && (*this)[0] == T(0) && "exp is not defined");
        polynom_t<mod> exp{1};
        for (int power = 1; power < degree; power <<= 1) {
            exp *= (polynom_t<MOD>{1} - exp.log(power << 1)
                + polynom_t<MOD>(begin(), begin() + std::min<int>(size(), power << 1)));
            exp.resize(std::min(degree, power << 1));
        }
        exp.resize(degree);
        return exp;
    }

    // Returns p^{d} modulo x^{degree}.
    // O(nlog), but with a very huge constant.
    polynom_t<mod> power(int64_t d, int degree) const {
        if (!d || !degree)
            return polynom_t<mod>{1}.resize(degree);

        int pos = 0;
        while (pos < int(size()) && (*this)[pos] == T(0))
            pos++;

        if (pos == int(size()) || pos >= (degree + d - 1) / d)
            return polynom_t<mod>(degree);

        int coeffs_left = degree - d * pos;
        polynom_t<mod> result = ((polynom_t<mod>(begin() + pos, end()) / (*this)[pos]).log(coeffs_left)
                            * T(d)).exp(coeffs_left) * (*this)[pos].power(d);
        result.resize(degree);
        for (int i = degree - 1; i - (degree - coeffs_left) >= 0; i--)
            result[i] = result[i - (degree - coeffs_left)];

        std::fill(result.begin(), result.end() - coeffs_left, T(0));
        return result;
    }

    template<typename V>
    std::enable_if_t<!std::is_same_v<V, polynom_t<mod>>, polynom_t<mod>&> operator*=(const V &value) {
        for (auto &x : *this)
            x *= value;

        return *this;
    }

    template<typename V>
    std::enable_if_t<!std::is_same_v<V, polynom_t<mod>>, polynom_t<mod>&> operator/=(const V &value) {
        for (auto &x : *this)
            x /= value;

        return *this;
    }

    friend std::ostream& operator<<(std::ostream &out, const polynom_t<mod> &p) {
        for (int i = 0; i < int(p.size()); i++) {
            if (i) out << ' ';
            out << p[i];
        }
        return out;
    }

    friend polynom_t<mod> operator+(const polynom_t<mod> &p, const polynom_t<mod> &q) {
        return polynom_t(p) += q;
    }

    friend polynom_t<mod> operator-(const polynom_t<mod> &p, const polynom_t<mod> &q) {
        return polynom_t(p) -= q;
    }

    friend polynom_t<mod> operator*(const polynom_t<mod> &p, const polynom_t<mod> &q) {
        return polynom_t(p) *= q;
    }

    friend polynom_t<mod> operator/(const polynom_t<mod> &p, const polynom_t<mod> &q) {
        return polynom_t(p) /= q;
    }

    friend polynom_t<mod> operator%(const polynom_t<mod> &p, const polynom_t<mod> &q) {
        return polynom_t(p) %= q;
    }

    template<typename V>
    friend std::enable_if_t<!std::is_same_v<V, polynom_t<mod>>, polynom_t<mod>>
        operator*(const V &value, const polynom_t<mod> &p) {
        return polynom_t<mod>(p) *= value;
    }

    template<typename V>
    friend std::enable_if_t<!std::is_same_v<V, polynom_t<mod>>, polynom_t<mod>>
        operator*(const polynom_t<mod> &p, const V &value) {
        return polynom_t<mod>(p) *= value;
    }

    template<typename V>
    friend std::enable_if_t<!std::is_same_v<V, polynom_t<mod>>, polynom_t<mod>>
        operator/(const V &value, const polynom_t<mod> &p) {
        return polynom_t<mod>(p) /= value;
    }

    template<typename V>
    friend std::enable_if_t<!std::is_same_v<V, polynom_t<mod>>, polynom_t<mod>>
        operator/(const polynom_t<mod> &p, const V &value) {
        return polynom_t<mod>(p) /= value;
    }
};

using polynom = polynom_t<MOD>;
