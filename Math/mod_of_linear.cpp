/*
 ! WARNING: careful with overflow. Don't forget to specify large enough type T.
 * Returns sum_{x = 0}^{n - 1} floor((kx + b) / m).
 * Require: k >= 0, b >= 0, m > 0, n >= 0.
 */
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

/*
 ! WARNING: careful with overflow. Don't forget to specify large enough to fit floor_sum type T.
 * Returns sum_{x = 0}^{n - 1} (kx + b) % m.
 * Require: m > 0, n >= 0.
 */
template<typename T>
T mod_sum(T k, T b, T m, T n) {
    k = (k % m + m) % m;
    b = (b % m + m) % m;
    return n * (n - 1) / 2 * k + n * b - m * floor_sum(k, b, m, n);
}

/*
 ! WARNING: WARNING: careful with overflow. Don't forget to specify large enough to fit floor_sum type T.
 * Returns the number of 0 <= x < n, such that (kx + b) mod m <= v.
 * Require: m > 0, n >= 0.
 */
template<typename T>
T count_remainders(T k, T b, T v, T m, T n) {
    if (v < 0) {
        return 0;
    }
    if (v >= m - 1) {
        return n;
    }
    return (mod_sum<T>(k, b - v - 1, m, n) - mod_sum<T>(k, b, m, n) + n * (v + 1)) / m;
}

/*
 * Returns min_{x=0}^{n - 1} (kx + b) mod m
 * Require: n, m > 0, 0 <= b, k < m
 */
template<typename T>
T min_of_mod_of_linear(T n, T m, T k, T b, T step_cost = 1, T overflow_cost = 0);

/*
 * Returns max_{x=0}^{n - 1} (kx + b) mod m
 * Require: n, m > 0, 0 <= b, k < m
 */
template<typename T>
T max_of_mod_of_linear(T n, T m, T k, T b, T step_cost = 1, T overflow_cost = 0);

template<typename T>
T max_of_mod_of_linear(T n, T m, T k, T b, T step_cost, T overflow_cost) {
    if (k == 0) {
        return b;
    }
    if (b < m - k) {
        T steps = (m - b - 1) / k;
        T cost = step_cost * steps;
        if (cost >= n) {
            return k * ((n - 1) / step_cost) + b;
        }
        n -= cost;
        b += steps * k;
    }
    return m - 1 - min_of_mod_of_linear(n, k, m % k, m - 1 - b, (m / k) * step_cost + overflow_cost, step_cost);
}

template<typename T>
T min_of_mod_of_linear(T n, T m, T k, T b, T step_cost, T overflow_cost) {
    if (k == 0) {
        return b;
    }
    if (b >= k) {
        T steps = (m - b + k - 1) / k;
        T cost = step_cost * steps + overflow_cost;
        if (cost >= n) {
            return b;
        }
        n -= cost;
        b += steps * k - m;
    }
    return k - 1 - max_of_mod_of_linear(n, k, m % k, k - 1 - b, (m / k) * step_cost + overflow_cost, step_cost);
}
