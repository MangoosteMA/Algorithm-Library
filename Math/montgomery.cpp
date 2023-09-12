/*
 * MOD must be prime.
 * MOD should be less than 2^30.
*/

template<uint32_t mod>
class montgomery {
    static_assert(mod < uint32_t(1) << 30, "mod < 2^30");
    using mint = montgomery<mod>;

private:
    uint32_t value;

    static constexpr uint32_t inv_neg_mod = []() {
        uint32_t x = mod;
        for (int i = 0; i < 4; ++i) {
            x *= uint32_t(2) - mod * x;
        }
        return -x;
    }();
    static_assert(mod * inv_neg_mod == -1);

    static constexpr uint32_t neg_mod = (-uint64_t(mod)) % mod;

    static uint32_t reduce(const uint64_t &value) {
        return (value + uint64_t(uint32_t(value) * inv_neg_mod) * mod) >> 32;
    }

    inline static const mint ONE = mint(1);

public:
    montgomery() : value(0) {}
    montgomery(const mint &x) : value(x.value) {}

    template<typename T, typename U = std::enable_if_t<std::is_integral<T>::value>>
    montgomery(const T &x) : value(!x ? 0 : reduce(int64_t(x % int32_t(mod) + int32_t(mod)) * neg_mod)) {}

    uint32_t get() const {
        auto real_value = reduce(value);
        return real_value < mod ? real_value : real_value - mod;
    }

    template<typename T>
    mint power(T degree) const {
        degree = (degree % int32_t(mod - 1) + int32_t(mod - 1)) % int32_t(mod - 1);
        mint prod = 1, a = *this;
        for (; degree > 0; degree >>= 1, a *= a)
            if (degree & 1)
                prod *= a;

        return prod;
    }

    mint inv() const {
        return power(-1);
    }

    mint& operator=(const mint &x) {
        value = x.value;
        return *this;
    }

    mint& operator+=(const mint &x) {
        if (int32_t(value += x.value - (mod << 1)) < 0) {
            value += mod << 1;
        }
        return *this;
    }

    mint& operator-=(const mint &x) {
        if (int32_t(value -= x.value) < 0) {
            value += mod << 1;
        }
        return *this;
    }

    mint& operator*=(const mint &x) {
        value = reduce(uint64_t(value) * x.value);
        return *this;
    }

    mint& operator/=(const mint &x) {
        return *this *= x.inv();
    }

    friend mint operator+(const mint &x, const mint &y) {
        return mint(x) += y;
    }

    friend mint operator-(const mint &x, const mint &y) {
        return mint(x) -= y;
    }

    friend mint operator*(const mint &x, const mint &y) {
        return mint(x) *= y;
    }

    friend mint operator/(const mint &x, const mint &y) {
        return mint(x) /= y;
    }

    mint& operator++() {
        return *this += ONE;
    }

    mint& operator--() {
        return *this -= ONE;
    }

    mint operator++(int) {
        mint prev = *this;
        *this += ONE;
        return prev;
    }

    mint operator--(int) {
        mint prev = *this;
        *this -= ONE;
        return prev;
    }

    mint operator-() const {
        return mint(0) - *this;
    }

    bool operator==(const mint &x) const {
        return get() == x.get();
    }

    bool operator!=(const mint &x) const {
        return get() != x.get();
    }

    bool operator<(const mint &x) const {
        return get() < x.get();
    }

    template<typename T>
    explicit operator T() {
        return get();
    }

    friend std::istream& operator>>(std::istream &in, mint &x) {
        std::string s;
        in >> s;
        x = 0;
        bool neg = s[0] == '-';
        for (const auto c : s)
            if (c != '-')
                x = x * 10 + (c - '0');

        if (neg)
            x *= -1;

        return in;
    }

    friend std::ostream& operator<<(std::ostream &out, const mint &x) {
        return out << x.get();
    }

    static int32_t primitive_root() {
        if constexpr (mod == 1'000'000'007)
            return 5;
        if constexpr (mod == 998'244'353)
            return 3;
        if constexpr (mod == 786433)
            return 10;

        static int root = -1;
        if (root != -1)
            return root;

        std::vector<int> primes;
        int value = mod - 1;
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
                if ((mint(r).power((mod - 1) / p)).get() == 1) {
                    ok = false;
                    break;
                }

            if (ok)
                return root = r;
        }
    }
};

// constexpr uint32_t MOD = 1'000'000'007;
// constexpr uint32_t MOD = 998'244'353;
using mint = montgomery<MOD>;
