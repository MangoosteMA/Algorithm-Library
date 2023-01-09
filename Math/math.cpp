namespace math {
    template<typename T>
    std::vector<std::pair<T, int>> get_factors(T n) {
        std::vector<std::pair<T, int>> factors;
        for (T i = 2; i * i <= n; i++) {
            if (n % i != 0)
                continue;

            factors.emplace_back(i, 0);
            while (n % i == 0) {
                factors.back().second++;
                n /= i;
            }
        }
        if (n != 1)
            factors.emplace_back(n, 1);

        return factors;
    }

    template<typename A, typename B>
    bool miller_rabin(A n, A b) {
        A t = n - 1;
        int s = 0;
        while (t % 2 == 0)
            s++, t >>= 1;

        auto power = [&](A a, A b) {
            A prod = 1;
            while (b > 0) {
                if (b & 1)
                    prod = B(prod) * a % n;

                a = B(a) * a % n;
                b >>= 1;
            }
            return prod;
        };

        A x = power(b, t);
        if (x == 1 || x == n - 1)
            return true;

        for (int i = 0; i < s - 1; i++) {
            x = B(x) * x % n;
            if (x == 1)
                return false;

            if (x == n - 1)
                return true;
        }
        return false;
    }

    template<typename A, typename B = __int128>
    bool is_prime(A x, const int tests = 20) {
        static std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());

        if (x == 1)
            return false;

        if (x == 2 || x == 3)
            return true;

        if (x % 2 == 0 || x % 3 == 0)
            return false;

        for (int i = 0; i < tests; i++)
            if (!miller_rabin<A, B>(x, rng() % (x - 3) + 2))
                return false;

        return true;
    }

    template<typename A, typename B = __int128>
    std::vector<A> get_prime_divisors(A n) {
        static std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());

        if (n <= 1)
            return {};

        if (is_prime(n))
            return {n};

        while (true) {
            A delta = rng() % n;

            auto f = [&](A x) {
                return (B(x) * x + delta) % n;
            };

            A a = rng() % n;
            A b = f(a);
            while (true) {
                A div = gcd(n, a > b ? a - b : b - a);
                if (div == n)
                    break;

                if (div != 1) {
                    auto left = get_prime_divisors(div);
                    auto right = get_prime_divisors(n / div);
                    left.reserve(left.size() + right.size());

                    for (const A &x : right)
                        left.push_back(x);

                    return left;
                }

                a = f(a);
                b = f(f(b));
            }
        }
    }

    template<typename T>
    T phi(T n) {
        T res = n;
        for (T i = 2; i * i <= n; i++) {
            if (n % i != 0)
                continue;

            res /= i;
            res *= i - 1;
            while (n % i == 0)
                n /= i;
        }
        if (n != 1)
            res /= n, res *= n - 1;

        return res;
    }

    template<typename A, typename B = A>
    std::array<B, 3> extgcd(A a, A b) {
        if (b == 0)
            return {a, 1, 0};

        auto [g, x, y] = extgcd(b, a % b);
        return {g, y, x - y * (a / b)};
    }
} // namespace math

using math::get_factors;
using math::is_prime;
using math::get_prime_divisors;
using math::phi;
using math::extgcd;
