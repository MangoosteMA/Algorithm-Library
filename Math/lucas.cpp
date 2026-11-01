/*
 ! WARNING: MOD must be positive and square-free.
 ! WARNING: Each prime divisor must be small enough (<= 20'000'000 is checked).
 * Require: static_modular_int.
 * Preprocessing: O(sum_{p | MOD, p is prime} p).
 * Query: O(sum_{p | MOD, p is prime} log_p(n)).
 */
template<int MOD>
struct Lucas {
private:
    using mint = static_modular_int<MOD>;

    inline static constexpr int PRIME_DIVS_COUNT = []() {
        int cnt = 0, x = MOD;

        for (int p = 2; 1ll * p * p <= x; p++) {
            if (x % p != 0) {
                continue;
            }

            cnt++;
            x /= p;

            if (x % p == 0) {
                throw "MOD must be square-free";
            }
        }

        return cnt + (x > 1);
    }();

    inline static constexpr auto PRIME_DIVS = []() {
        std::array<int, PRIME_DIVS_COUNT> divs{};
        int pos = 0, x = MOD;

        for (int p = 2; 1ll * p * p <= x; p++) {
            if (x % p != 0) {
                continue;
            }

            divs[pos++] = p;
            x /= p;
        }

        if (x > 1) {
            divs[pos++] = x;
        }
        return divs;
    }();

    static constexpr std::array<int, 3> extgcd(int a, int b) {
        if (a == 0) {
            return {b, 0, 1};
        }
        auto [g, x, y] = extgcd(b % a, a);
        return {g, y - x * (b / a), x};
    }

    template<int MOD1, int MOD2>
    struct CRT {
        static_assert(MOD1 > 0 && MOD2 > 0, "Moduli must be positive");
        inline static constexpr auto EG = extgcd(MOD1, MOD2);
        static_assert(EG[0] == 1, "MOD1 and MOD2 are not coprime");

        inline static constexpr int INV = (EG[1] % MOD2 + MOD2) % MOD2;

        static constexpr int combine(int rem1, int rem2) {
            rem1 %= MOD1;
            if (rem1 < 0) {
                rem1 += MOD1;
            }

            rem2 %= MOD2;
            if (rem2 < 0) {
                rem2 += MOD2;
            }

            auto delta = rem2 - rem1 % MOD2;
            if (delta < 0) {
                delta += MOD2;
            }
            auto t = int64_t(delta) * INV % MOD2;
            return rem1 + MOD1 * t;
        }
    };

    template<int P>
    struct Factorials {
        static_assert(P <= 20'000'000, "Prime factor is too large for factorial tables");

        using mint = static_modular_int<P>;

        std::vector<mint> fact, ifact;

        Factorials() {
            fact.resize(P);
            fact[0] = 1;
            for (int i = 1; i < P; i++) {
                fact[i] = fact[i - 1] * i;
            }

            ifact.resize(P);
            ifact[P - 1] = fact[P - 1].inv();
            for (int i = P - 1; i > 0; i--) {
                ifact[i - 1] = ifact[i] * i;
            }
        }

        mint choose(int64_t n, int64_t k) const {
            if (n < 0 || k < 0 || k > n) {
                return 0;
            }

            mint res = 1;
            while (n != 0) {
                int ni = n % P;
                int ki = k % P;
                if (ki > ni) {
                    return 0;
                }
                res *= fact[ni] * ifact[ki] * ifact[ni - ki];
                n /= P;
                k /= P;
            }
            return res;
        }
    };

    template<int P>
    static const Factorials<P> &factorials() {
        static const Factorials<P> table;
        return table;
    }

    template<size_t i, int CURRENT_MOD>
    static mint choose_impl(int64_t n, int64_t k, int64_t rem) {
        if constexpr (i == PRIME_DIVS_COUNT) {
            return mint(rem);
        } else {
            constexpr int P = PRIME_DIVS[i];
            auto prime_rem = factorials<P>().choose(n, k).get();
            auto next = CRT<CURRENT_MOD, P>::combine(rem, prime_rem);
            return choose_impl<i + 1, CURRENT_MOD * P>(n, k, next);
        }
    }

public:
    static mint choose(int64_t n, int64_t k) {
        if (n < 0 || k < 0 || k > n) {
            return 0;
        }
        return choose_impl<0, 1>(n, k, 0);
    }
};
