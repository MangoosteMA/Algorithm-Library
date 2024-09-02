/*
 ! WARNING: implementation won't work for T = integer type.
 * Operator / must be defined for type T.
 * Given n points (x[i], y[i]). All x[i] must be distinct.
 * Returns a polynomial f of degree (n - 1) such that f(x[i]) = y[i].
 */
template<typename T>
std::vector<T> lagrange_interpolation(const std::vector<T> &x, const std::vector<T> &y) {
    assert(x.size() == y.size());
    int n = x.size();
    std::vector<T> prod(n + 1);
    prod[0] = 1;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j >= 0; j--) {
            prod[j] *= -x[i];
            if (j > 0) {
                prod[j] += prod[j - 1];
            }
        }
    }

    std::vector<T> f(n);
    for (int i = 0; i < n; i++) {
        T denom = 1;
        for (int j = 0; j < n; j++) {
            if (i != j) {
                denom *= x[i] - x[j];
            }
        }
        T coeff = y[i] / denom, val = prod[n];
        for (int j = n - 1; j >= 0; j--) {
            f[j] += val * coeff;
            val = prod[j] + x[i] * val;
        }
    }
    return f;
}
