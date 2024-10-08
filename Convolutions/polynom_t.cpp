/*
 ! WARNING: (MOD - 1) must be divisible by (2n), where n is max degree of polynomials.
 * Include static_modular_int or montgomery (faster) to use it.
 * Don't need to care about precomputing primitive root.
 * Use it like std::vector<mint> with extra methods.
 */
template<typename mint>
class polynom_t : public std::vector<mint> {
public:
    static constexpr int RANK = __builtin_ctz(mint::get_mod() - 1);
    static_assert(RANK >= 15, "MOD doesn't seem fft-friendly.");

    using value_type = mint;

    using std::vector<mint>::empty;
    using std::vector<mint>::back;
    using std::vector<mint>::pop_back;
    using std::vector<mint>::size;
    using std::vector<mint>::clear;
    using std::vector<mint>::begin;
    using std::vector<mint>::end;

private:
    static constexpr int EVAL_BRUTE_SIZE = 1 << 4;
    static constexpr int MUL_MIN_CUT = 20;
    static constexpr int MUL_MAX_CUT = 1 << 6;
    static constexpr int DIV_N_CUT = 1 << 7;
    static constexpr int DIV_M_CUT = 1 << 6;
    static constexpr int INV_BRUTE_FORCE_SIZE = 1 << 3;

    class fft_precalc {
    private:
        std::vector<mint> inv_{0, 1};

    public:
        mint root;
        // roots[x] = root.power((MOD - 1) / 2^x)
        // inv_roots[x] = roots[x].inv()
        std::array<mint, RANK> roots, inv_roots;
        // inv_l[x] = 2^{-x}
        std::array<mint, RANK> inv_l;
        // r_transition[x] = root.power((MOD - 1) * (3 - 2^{x + 1}) / 2^{x + 2})
        // inv_r_transition[x] = r_transition[x].inv()
        std::array<mint, RANK> r_transition, inv_r_transition;

        fft_precalc() {
            root = mint::primitive_root();
            for (int x = 0; x < RANK; x++) {
                roots[x] = mint(root).power((mint::get_mod() - 1) >> x);
                inv_roots[x] = roots[x].inv();
                inv_l[x] = mint(1 << x).inv();
                r_transition[x] = root.power(((mint::get_mod() - 1) >> (x + 2)) * (3 + (1 << (x + 1))));
                inv_r_transition[x] = r_transition[x].inv();
            }
        }

        mint inv(int x) {
            for (int i = inv_.size(); i <= x; i++) {
                inv_.push_back(-inv_[mint::get_mod() % i] * mint(mint::get_mod() / i));
            }
            return inv_[x];
        }
    };

    inline static fft_precalc fft_data;

public:
    /*
     * Let r be the primitive root of the given MOD.
     * Let rev(i) be the reversed i as a binary mask of size log(n).
     * Let R[i] = r.power((MOD - 1) / a.size() * rev(i)).
     * It replaces a[i] with a.eval(R[i]).
     */
    static void fft(polynom_t<mint> &a) {
        assert(!a.empty());
        int n = a.size(), l = __builtin_ctz(n);
        for (int len = 0; len < l; len++) {
            int m = n >> (len + 1);
            mint r = 1;
            for (int i = 0; i < (1 << len); i++) {
                // rot = r^((MOD - 1) / n * rev(2 * i))
                int id = i << (l - len);
                for (int j = 0; j < m; j++) {
                    auto u = a[id + j];
                    auto v = a[id + j + m] * r;
                    a[id + j] = u + v;
                    a[id + j + m] = u - v;
                }
                if (i + 1 < (1 << len)) {
                    r *= fft_data.r_transition[__builtin_ctz(~i)];
                }
            }
        }
    }

    // inv_fft(fft(a)) = a
    static void inv_fft(polynom_t<mint> &a) {
        assert(!a.empty());
        int n = a.size(), l = __builtin_ctz(n);
        for (int len = l - 1; len >= 0; len--) {
            int m = n >> (len + 1);
            mint ir = 1;
            for (int i = 0; i < (1 << len); i++) {
                int id = i << (l - len);
                for (int j = 0; j < m; j++) {
                    auto u = a[id + j];
                    auto v = a[id + j + m];
                    a[id + j] = u + v;
                    a[id + j + m] = (u - v) * ir;
                }
                if (i + 1 < (1 << len)) {
                    ir *= fft_data.inv_r_transition[__builtin_ctz(~i)];
                }
            }
        }
        for (auto &value : a) {
            value *= fft_data.inv_l[l];
        }
    }

private:
    // Completes fft assuming that a is the right half of the polynomial after the first iteration of the fft.
    static void right_half_fft(polynom_t<mint> &a) {
        assert(!a.empty());
        int n = a.size(), l = __builtin_ctz(n);
        for (int len = 0; len < l; len++) {
            int m = n >> (len + 1);
            mint r = fft_data.roots[len + 2];
            for (int i = 0; i < (1 << len); i++) {
                int id = i << (l - len);
                for (int j = 0; j < m; j++) {
                    auto u = a[id + j];
                    auto v = a[id + j + m] * r;
                    a[id + j] = u + v;
                    a[id + j + m] = u - v;
                }
                if (i + 1 < (1 << len)) {
                    r *= fft_data.r_transition[__builtin_ctz(~i)];
                }
            }
        }
    }

    // inv_right_half_fft(right_half_fft(a)) = a
    static void inv_right_half_fft(polynom_t<mint> &a) {
        assert(!a.empty());
        int n = a.size(), l = __builtin_ctz(n);
        for (int len = l - 1; len >= 0; len--) {
            int m = n >> (len + 1);
            mint ir = fft_data.inv_roots[len + 2];
            for (int i = 0; i < (1 << len); i++) {
                int id = i << (l - len);
                for (int j = 0; j < m; j++) {
                    auto u = a[id + j];
                    auto v = a[id + j + m];
                    a[id + j] = u + v;
                    a[id + j + m] = (u - v) * ir;
                }
                if (i + 1 < (1 << len)) {
                    ir *= fft_data.inv_r_transition[__builtin_ctz(~i)];
                }
            }
        }
        for (auto &value : a) {
            value *= fft_data.inv_l[l];
        }
    }

    class multipoint_evaluation_tree {
    private:
        int n_points, l;
        std::vector<polynom_t<mint>> segtree;

    public:
        multipoint_evaluation_tree(int n, const std::vector<mint> &points) : n_points(points.size()), l(1) {
            while ((1 << l) < n) {
                l++;
            }
            polynom_t<mint> aux;
            aux.reserve(1 << l);

            segtree.assign(l + 1, polynom_t<mint>(1 << (l + 1)));
            for (int i = 0; i < (1 << l); i++) {
                aux = {i < n_points ? -points[i] : 0, 1};
                fft(aux);
                segtree[0][i << 1] = aux[0];
                segtree[0][i << 1 | 1] = aux[1];
            }

            for (int len = 0; len < l; len++) {
                aux.resize(1 << (len + 1));
                for (int i = 0; i < (1 << (l + 1)); i += (1 << (len + 2))) {
                    for (int j = 0; j < static_cast<int>(aux.size()); j++) {
                        aux[j] = segtree[len][i + j] * segtree[len][i + j + aux.size()];
                    }
                    if (len + 1 < l) {
                        std::copy(aux.begin(), aux.end(), segtree[len + 1].begin() + i);
                        inv_fft(aux);
                        aux[0] -= 2;
                        right_half_fft(aux);
                        std::copy(aux.begin(), aux.end(), segtree[len + 1].begin() + i + aux.size());
                    } else {
                        inv_fft(aux);
                        aux[0]--;
                        std::copy(aux.begin(), aux.end(), segtree[len + 1].begin() + i);
                        segtree[len + 1][1 << (len + 1)]++;
                    }
                }
            }
        }

        std::vector<mint> evaluate(polynom_t<mint> f) const {
            if (static_cast<int>(f.size()) > (1 << l)) {
                f %= segtree[l];
            }
            assert(static_cast<int>(f.size()) <= (1 << l));
            f.resize(1 << (l + 1));
            std::rotate(f.begin(), std::prev(f.end()), f.end());
            fft(f);

            auto g = segtree[l];
            std::reverse(g.begin(), g.begin() + 1 + (1 << l));
            g.resize(1 << l);
            g = g.inv(1 << l);
            std::reverse(g.begin(), g.end());
            g.resize(1 << (l + 1));
            fft(g);
            for (int i = 0; i < (1 << (l + 1)); i++) {
                g[i] *= f[i];
            }

            polynom_t<mint> aux;
            aux.reserve(1 << l);
            for (int len = l - 1; len >= 0; len--) {
                aux.resize(1 << (len + 1));
                for (int i = 0; i < (1 << (l + 1)); i += (1 << (len + 2))) {
                    for (int j = 0; j < static_cast<int>(aux.size()); j++) {
                        aux[j] = g[i + j + (1 << (len + 1))];
                    }
                    inv_right_half_fft(aux);
                    fft(aux);
                    std::copy(aux.begin(), aux.end(), g.begin() + i + (1 << (len + 1)));
                    for (int j = 0; j < (1 << (len + 1)); j++) {
                        auto x = g[i + j] - g[i + j + (1 << (len + 1))];
                        g[i + j + (1 << (len + 1))] = x * segtree[len][i + j];
                        g[i + j] = x * segtree[len][i + j + (1 << (len + 1))];
                    }
                }
            }

            std::vector<mint> eval(n_points);
            for (int i = 0; i < n_points; i++) {
                eval[i] = (g[2 * i] - g[2 * i + 1]) * fft_data.inv_l[l + 1];
            }
            return eval;
        }

        polynom_t<mint> interpolate(const std::vector<mint> &y) const {
            auto prod = segtree[l];
            prod.erase(prod.begin(), prod.begin() + ((1 << l) - n_points));
            auto values = evaluate(prod.derivative());

            polynom_t<mint> aux;
            aux.reserve(1 << l);
            polynom_t<mint> result(1 << (l + 1));
            for (int i = 0; i < (1 << l); i++) {
                aux = {i < n_points ? y[i] / values[i] : 0, 0};
                fft(aux);
                result[i << 1] = aux[0];
                result[i << 1 | 1] = aux[1];
            }

            polynom_t<mint> next(1 << (l + 1));
            for (int len = 0; len < l; len++) {
                aux.resize(1 << (len + 1));
                for (int i = 0; i < (1 << (l + 1)); i += (1 << (len + 2))) {
                    for (int j = 0; j < (1 << (len + 1)); j++) {
                        aux[j] = segtree[len][i + j] * result[i + j + aux.size()]
                               + segtree[len][i + j + aux.size()] * result[i + j];
                    }
                    if (len + 1 < l) {
                        std::copy(aux.begin(), aux.end(), next.begin() + i);
                        inv_fft(aux);
                        right_half_fft(aux);
                        std::copy(aux.begin(), aux.end(), next.begin() + i + (1 << (len + 1)));
                    } else {
                        inv_fft(aux);
                        std::copy(aux.begin(), aux.end(), next.begin());
                    }
                }
                result.swap(next);
            }
            result.erase(result.begin(), result.begin() + ((1 << l) - n_points));
            return result.resize(n_points);
        }
    };

public:
    polynom_t() : std::vector<mint>() {}
    polynom_t(size_t n, mint value = 0) : std::vector<mint>(n, value) {}
    polynom_t(std::initializer_list<mint> values) : std::vector<mint>(values.begin(), values.end()) {}

    template<typename Iterator>
    polynom_t(Iterator first, Iterator last) : std::vector<mint>(first, last) {}

    polynom_t<mint>& resize(int n) {
        std::vector<mint>::resize(n);
        return *this;
    }

    // Removes extra zeroes.
    void normalize() {
        while (!empty() && back() == mint(0)) {
            pop_back();
        }
    }

    // Returns -1 if all coefficients are zeroes (not O(1)!).
    int degree() const {
        int deg = static_cast<int>(size()) - 1;
        while (deg >= 0 && (*this)[deg] == mint(0)) {
            deg--;
        }
        return deg;
    }

    mint eval(const mint &x) const {
        mint power = 1, value = 0;
        for (int i = 0; i < static_cast<int>(size()); i++, power *= x) {
            value += (*this)[i] * power;
        }
        return value;
    }

    // Calculates eval at the given points.
    // O(n log^2).
    std::vector<mint> multipoint_evaluation(const std::vector<mint> &points) const {
        if (std::min(size(), points.size()) <= EVAL_BRUTE_SIZE) {
            std::vector<mint> eval(points.size());
            for (int i = 0; i < static_cast<int>(points.size()); i++) {
                eval[i] = this->eval(points[i]);
            }
            return eval;
        }
        return multipoint_evaluation_tree(std::max(size(), points.size()), points).evaluate(*this);
    }

    // Interpolates polynomial f, such that f(x[i]) = y[i].
    // O(n log^2).
    static polynom_t<mint> interpolate(const std::vector<mint> &x, const std::vector<mint> &y) {
        assert(x.size() == y.size());
        return multipoint_evaluation_tree(x.size(), x).interpolate(y);
    }

    polynom_t<mint> operator-() const {
        return polynom_t(*this) * mint(-1);
    }

    polynom_t<mint>& operator+=(const polynom_t<mint> &another) {
        if (size() < another.size()) {
            resize(another.size());
        }
        for (int i = 0; i < static_cast<int>(another.size()); i++) {
            (*this)[i] += another[i];
        }
        return *this;
    }

    polynom_t<mint>& operator-=(const polynom_t<mint> &another) {
        if (size() < another.size()) {
            resize(another.size());
        }
        for (int i = 0; i < static_cast<int>(another.size()); i++) {
            (*this)[i] -= another[i];
        }
        return *this;
    }

    polynom_t<mint>& operator*=(const polynom_t<mint> &another) {
        if (empty() || another.empty()) {
            clear();
            return *this;
        }

        if (std::min(size(), another.size()) <= MUL_MIN_CUT
            || std::max(size(), another.size()) <= MUL_MAX_CUT) {
            polynom_t<mint> product(int(size() + another.size()) - 1);
            for (int i = 0; i < static_cast<int>(size()); i++) {
                for (int j = 0; j < static_cast<int>(another.size()); j++) {
                    product[i + j] += (*this)[i] * another[j];
                }
            }
            product.swap(*this);
            return *this;
        }

        int real_size = static_cast<int>(size() + another.size()) - 1, n = 1;
        while (n < real_size) {
            n <<= 1;
        }

        if ((*this) == another) {
            resize(n);
            fft(*this);
            for (int i = 0; i < n; i++) {
                (*this)[i] *= (*this)[i];
            }
        } else {
            resize(n);
            polynom_t b = another;
            b.resize(n);
            fft(*this), fft(b);
            for (int i = 0; i < n; i++) {
                (*this)[i] *= b[i];
            }
        }
        inv_fft(*this);
        return resize(real_size);
    }

    // Division with remainder.
    // O(n log).
    polynom_t<mint>& operator/=(const polynom_t<mint> &another) {
        polynom_t<mint> a(*this), b(another);
        a.normalize(), b.normalize();
        assert(!b.empty());
        int n = a.size(), m = b.size();
        if (n < m) {
            return *this = {};
        }
        if (n <= DIV_N_CUT || m <= DIV_M_CUT) {
            polynom_t<mint> quotient(n - m + 1);
            mint inv_b = mint(1) / b.back();
            for (int i = n - 1; i >= m - 1; i--) {
                int pos = i - m + 1;
                quotient[pos] = a[i] * inv_b;
                for (int j = 0; j < m; j++) {
                    a[pos + j] -= b[j] * quotient[pos];
                }
            }
            quotient.normalize();
            return *this = quotient;
        }

        std::reverse(a.begin(), a.end());
        std::reverse(b.begin(), b.end());
        polynom_t<mint> quotient = a * b.inv(n - m + 1);
        quotient.resize(n - m + 1);
        std::reverse(quotient.begin(), quotient.end());
        quotient.normalize();
        return *this = quotient;
    }

    // O(n log).
    polynom_t<mint>& operator%=(const polynom_t<mint> &another) {
        *this -= (*this) / another * another;
        normalize();
        return *this;
    }

    // Returns derivative.
    // O(n).
    polynom_t<mint> derivative() const {
        polynom_t<mint> der(std::max(0, static_cast<int>(size()) - 1));
        for (int i = 0; i < static_cast<int>(der.size()); i++) {
            der[i] = mint(i + 1) * (*this)[i + 1];
        }
        return der;
    }

    // Returns integral.
    // O(n).
    polynom_t<mint> integral(const mint &constant = mint(0)) const {
        polynom_t<mint> in(size() + 1);
        in[0] = constant;
        for (int i = 1; i < static_cast<int>(in.size()); i++) {
            in[i] = (*this)[i - 1] * fft_data.inv(i);
        }
        return in;
    }

    // Returns p^{-1} modulo x^{degree}.
    // O(n log).
    polynom_t<mint> inv(int degree) const {
        assert(!empty() && (*this)[0] != mint(0) && "polynom is not invertable");

        int result_size = 1;
        while (result_size < degree) {
            result_size <<= 1;
        }
        polynom_t<mint> inv(result_size);
        inv[0] = (*this)[0].inv();

        int brute_calculation = std::min(degree, INV_BRUTE_FORCE_SIZE);
        mint start_inv = (*this)[0].inv();

        polynom_t<mint> have(brute_calculation);
        for (int i = 0; i < brute_calculation; i++) {
            inv[i] = ((i == 0 ? 1 : 0) - have[i]) * start_inv;
            int steps = std::min<int>(size(), have.size() - i);
            for (int j = 0; j < steps; j++) {
                have[i + j] += inv[i] * (*this)[j];
            }
        }

        polynom_t<mint> this_copy, inv_copy;
        this_copy.reserve(result_size);
        inv_copy.reserve(result_size);

        for (int power = brute_calculation; power < degree; power <<= 1) {
            this_copy.resize(power << 1);
            std::fill(this_copy.begin() + std::min<int>(size(), power << 1), this_copy.end(), 0);
            std::copy(begin(), begin() + std::min<int>(size(), power << 1), this_copy.begin());
            inv_copy.resize(power << 1);
            std::copy(inv.begin(), inv.begin() + power, inv_copy.begin());

            fft(this_copy);
            fft(inv_copy);
            for (int i = 0; i < (power << 1); i++) {
                this_copy[i] *= inv_copy[i];
            }
            inv_fft(this_copy);
            std::fill(this_copy.begin(), this_copy.begin() + power, 0);
            fft(this_copy);
            for (int i = 0; i < (power << 1); i++) {
                this_copy[i] *= inv_copy[i];
            }
            inv_fft(this_copy);
            for (int i = power; i < (power << 1); i++) {
                inv[i] = -this_copy[i];
            }
        }
        return inv.resize(degree);
    }

    // Returns log(p) modulo x^{degree}.
    // O(n log).
    polynom_t<mint> log(int degree) const {
        assert(!empty() && (*this)[0] == mint(1) && "log is not defined");
        return (derivative().resize(std::min<int>(degree, size())) * inv(degree)).resize(degree).integral(mint(0)).resize(degree);
    }

    // Returns exp(p) modulo x^{degree}.
    // O(n log).
    polynom_t<mint> exp(int degree) const {
        assert(!empty() && (*this)[0] == mint(0) && "exp is not defined");

        int result_size = 1;
        while (result_size < degree) {
            result_size <<= 1;
        }

        polynom_t<mint> exp, exp_fft, inv_exp, inv_exp_fft;
        exp = exp_fft = inv_exp = inv_exp_fft = {1};
        polynom_t<mint> h(1), q(1);
        auto diff = derivative().resize(result_size);

        for (int power = 1; power < degree; power <<= 1) {
            exp_fft = polynom_t<mint>(exp).resize(power << 1);
            fft(exp_fft);
            for (int i = 0; i < power; i++) {
                h[i] = exp_fft[i] * inv_exp_fft[i];
            }
            inv_fft(h);

            std::fill(h.begin(), h.begin() + ((power + 1) >> 1), 0);
            fft(h);
            for (int i = 0; i < power; i++) {
                h[i] *= -inv_exp_fft[i];
            }
            inv_fft(h);

            for (int i = (power + 1) / 2; i < power; i++) {
                inv_exp.push_back(h[i]);
            }
            inv_exp_fft = polynom_t<mint>(inv_exp).resize(power << 1);
            fft(inv_exp_fft);

            h.assign(power, 0);
            std::copy(diff.begin(), diff.begin() + power - 1, h.begin());
            fft(h);
            for (int i = 0; i < power; i++) {
                q[i] = exp_fft[i] * h[i];
            }
            inv_fft(q);

            h.resize(power << 1);
            for (int i = 1; i < power; i++) {
                h[i] = exp[i] * i;
            }
            for (int i = 0; i < power; i++) {
                h[i + 1] -= q[i];
            }
            h[0] = -q[power - 1];
            fft(h);

            q.resize(power << 1);
            for (int i = 0; i < (power << 1); i++) {
                q[i] = inv_exp_fft[i] * h[i];
            }
            inv_fft(q);

            if (static_cast<int>(size()) >= power) {
                h.assign(begin() + power, begin() + std::min<int>(power << 1, size()));
            } else {
                h.clear();
            }
            h.resize(power);

            for (int i = power - 1; i >= 0; i--) {
                h[i] -= q[i] * fft_data.inv(i + power);
            }
            h.resize(power << 1);
            fft(h);
            for (int i = 0; i < (power << 1); i++) {
                q[i] = exp_fft[i] * h[i];
            }
            inv_fft(q);

            exp.resize(power << 1);
            for (int i = 0; i < power; i++) {
                exp[power + i] = q[i];
            }
        }
        return exp.resize(degree);
    }

    // Returns p^{d} modulo x^{degree}.
    // O(n log).
    polynom_t<mint> power(int64_t d, int degree) const {
        if (!d || !degree) {
            return polynom_t<mint>{1}.resize(degree);
        }
        int pos = 0;
        while (pos < static_cast<int>(size()) && (*this)[pos] == mint(0)) {
            pos++;
        }
        if (pos == static_cast<int>(size()) || pos >= (degree + d - 1) / d) {
            return polynom_t<mint>(degree);
        }

        int coeffs_left = degree - d * pos;
        polynom_t<mint> result = ((polynom_t<mint>(begin() + pos, end()) / (*this)[pos]).log(coeffs_left)
                               * mint(d)).exp(coeffs_left) * (*this)[pos].power(d);
        result.resize(degree);
        for (int i = degree - 1; i - (degree - coeffs_left) >= 0; i--) {
            result[i] = result[i - (degree - coeffs_left)];
        }
        std::fill(result.begin(), result.end() - coeffs_left, mint(0));
        return result;
    }

    // Returns f(x + c).
    // O(n log).
    polynom_t<mint> change_of_variable(mint c) const {
        int n = size();
        polynom_t<mint> p(n), q(n);
        mint fact = 1, ifact = 1, power = 1;
        for (int i = 0; i < n; i++) {
            p[n - 1 - i] = (*this)[i] * fact;
            q[i] = power * ifact;
            fact *= i + 1;
            ifact *= fft_data.inv(i + 1);
            power *= c;
        }

        auto result = p * q;
        result.resize(n);
        std::reverse(result.begin(), result.end());
        ifact = 1;
        for (int i = 0; i < n; i++) {
            result[i] *= ifact;
            ifact *= fft_data.inv(i + 1);
        }
        return result;
    }

    template<typename V>
    std::enable_if_t<!std::is_same_v<V, polynom_t<mint>>, polynom_t<mint>&> operator*=(const V &value) {
        for (auto &x : *this) {
            x *= value;
        }
        return *this;
    }

    template<typename V>
    std::enable_if_t<!std::is_same_v<V, polynom_t<mint>>, polynom_t<mint>&> operator/=(const V &value) {
        for (auto &x : *this) {
            x /= value;
        }
        return *this;
    }

    friend std::ostream& operator<<(std::ostream &out, const polynom_t<mint> &p) {
        for (int i = 0; i < static_cast<int>(p.size()); i++) {
            if (i) out << ' ';
            out << p[i];
        }
        return out;
    }

    friend polynom_t<mint> operator+(const polynom_t<mint> &p, const polynom_t<mint> &q) {
        return polynom_t(p) += q;
    }

    friend polynom_t<mint> operator-(const polynom_t<mint> &p, const polynom_t<mint> &q) {
        return polynom_t(p) -= q;
    }

    friend polynom_t<mint> operator*(const polynom_t<mint> &p, const polynom_t<mint> &q) {
        return polynom_t(p) *= q;
    }

    friend polynom_t<mint> operator/(const polynom_t<mint> &p, const polynom_t<mint> &q) {
        return polynom_t(p) /= q;
    }

    friend polynom_t<mint> operator%(const polynom_t<mint> &p, const polynom_t<mint> &q) {
        return polynom_t(p) %= q;
    }

    template<typename V>
    friend std::enable_if_t<!std::is_same_v<V, polynom_t<mint>>, polynom_t<mint>>
        operator*(const V &value, const polynom_t<mint> &p) {
        return polynom_t<mint>(p) *= value;
    }

    template<typename V>
    friend std::enable_if_t<!std::is_same_v<V, polynom_t<mint>>, polynom_t<mint>>
        operator*(const polynom_t<mint> &p, const V &value) {
        return polynom_t<mint>(p) *= value;
    }

    template<typename V>
    friend std::enable_if_t<!std::is_same_v<V, polynom_t<mint>>, polynom_t<mint>>
        operator/(const V &value, const polynom_t<mint> &p) {
        return polynom_t<mint>(p) /= value;
    }

    template<typename V>
    friend std::enable_if_t<!std::is_same_v<V, polynom_t<mint>>, polynom_t<mint>>
        operator/(const polynom_t<mint> &p, const V &value) {
        return polynom_t<mint>(p) /= value;
    }
};

using polynom = polynom_t<mint>;
