// Returns sum_{x = 0}^{n - 1} floor((kx + b) / m)
// Require: k >= 0, b >= 0, m > 0, n >= 0
// WARNING: careful with overflow. Don't forget to specify large enough type T.
template<typename T>
T floor_sum(T k, T b, T m, T n) {
    if (k == 0) {
        return (b / m) * n;
    }
    if (k >= m || b >= m) {
        return n * (n - 1) / 2 * (k / m) + n * (b / m) + floor_sum(k % m, b % m, m, n);
    }
    T ymax = (k * (n - 1) + b) / m;
    return n * ymax - floor_sum(m, m + k - b - 1, k, ymax);
}

// Returns sum_{x = 0}^{n - 1} (kx + b) % m
// Require: m > 0, n >= 0
// WARNING: careful with overflow. Don't forget to specify large enough to fit floor_sum type T.
template<typename T>
T mod_sum(T k, T b, ll m, ll n) {
    k = (k % m + m) % m;
    b = (b % m + m) % m;
    return n * (n - 1) / 2 * k + n * b - m * floor_sum(k, b, m, n);
}

// Returns the number of 0 <= x < n, such that kx mod m <= v
// Require: k >= 0, m > 0, n >= 0
// WARNING: careful with overflow. Don't forget to specify large enough to fit floor_sum type T.
template<typename T>
T count_remainders(T k, T v, T m, T n) {
    if (v < 0) {
        return 0;
    }
    if (v >= m - 1) {
        return n;
    }
    return (mod_sum<T>(k, m - v - 1, m, n) - mod_sum<T>(k, 0, m, n) + n * (v + 1)) / m;
}
