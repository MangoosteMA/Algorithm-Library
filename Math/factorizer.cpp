/*
 * Include dynamic_montgomery to use it.
*/

namespace factorizer {
    using ull = unsigned long long;

    bool is_prime(ull value) {
        if (value < 2)
            return false;

        for (ull x : {2, 3, 5, 7, 11}) {
            if (value == x)
                return true;

            if (value % x == 0)
                return false;
        }

        if (mint::get_mod() != value)
            mint::set_mod(value);

        ull d = value - 1;
        int s = __builtin_ctzll(d);
        d >>= s;

        const mint ONE = 1;
        const mint NEG_ONE = -1;

        auto miller_rabin = [&](ull base) {
            if (base == 0)
                return true;

            mint y = mint(base).power(d);
            if (y == ONE)
                return true;

            for (int i = 0; i < s; i++) {
                if (y == NEG_ONE)
                    return true;

                y *= y;
            }
            return false;
        };

        if (value < 4759123141ull) {
            for (ull base : {2, 7, 61})
                if (!miller_rabin(base % value))
                    return false;
        } else {
            for (ull base : {2, 325, 9375, 28178, 450775, 9780504, 1795265022})
                if (!miller_rabin(base % value))
                    return false;
        }
        return true;
    }

    // If there is no non-trivial divisor, returns value.
    ull find_any_nontrivial_divisor(ull value) {
        for (ull x : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29})
            if (value % x == 0)
                return x;

        if (is_prime(value) || value == 1)
            return value;

        if (mint::get_mod() != value)
            mint::set_mod(value);

        static std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        const mint ONE = 1;
        while (true) {
            mint random_factor = rng() % (value - 1) + 1;

            auto f = [&](mint x) {
                return x * x + random_factor;
            };

            mint x = rng() % (value - 2) + 2;
            mint y = x;
            ull g = 1;
            while (g == 1) {
                mint prod = ONE;
                mint save_x = x;
                mint save_y = y;

                static constexpr int STEP = 128;
                for (int i = 0; i < STEP; i++) {
                    x = f(x);
                    y = f(f(y));
                    prod *= x - y;
                }

                g = std::gcd(prod.get(), value);
                if (g == 1)
                    continue;

                x = save_x;
                y = save_y;
                for (int i = 0; i < STEP; i++) {
                    x = f(x);
                    y = f(f(y));
                    g = std::gcd((x - y).get(), value);
                    if (g != 1)
                        break;
                }

                if (g != 1 && g != value)
                    return g;
            }
        }
    }

    // If n is divisible by p^d and not divisible by p^{d + 1} then p will occur d times.
    template<typename T>
    std::vector<T> get_all_prime_factors_with_duplicates(T value) {
        std::vector<T> res;

        auto dfs = [&](auto self, ull v) -> void {
            if (v == 1)
                return;

            ull x = find_any_nontrivial_divisor(v);
            if (x == v) {
                res.push_back(x);
                return;
            }
            self(self, x);
            self(self, v / x);
        };

        dfs(dfs, value);
        std::sort(res.begin(), res.end());
        return res;
    }

    // Return all prime factors in the format (prime, degree) sorted by prime.
    template<typename T>
    std::vector<std::pair<T, int>> get_all_prime_factors(T value) {
        auto prime_factors = get_all_prime_factors_with_duplicates(value);
        std::vector<std::pair<T, int>> factors;
        for (int i = 0, j = 0; i < int(prime_factors.size()); i = j) {
            while (j < int(prime_factors.size()) && prime_factors[i] == prime_factors[j])
                j++;

            factors.emplace_back(prime_factors[i], j - i);
        }
        return factors;
    }

    // Returns all factors of the number sorted in increasing order.
    template<typename T>
    std::vector<T> get_all_factors(T value) {
        std::vector<T> divs{1};
        for (auto [p, d] : get_all_prime_factors(value)) {
            int prev_size = divs.size();
            for (int i = 0; i < prev_size; i++) {
                T coeff = 1;
                for (int j = 0; j < d; j++) {
                    coeff *= p;
                    divs.push_back(divs[i] * coeff);
                }
            }
        }

        std::sort(divs.begin(), divs.end());
        return divs;
    }
} // namespace factorizer

using factorizer::is_prime;
using factorizer::get_all_prime_factors;
using factorizer::get_all_factors;
