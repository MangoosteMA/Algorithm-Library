/*
 * Don't forget to run dynamic_montgomery::set_mod method, before using it.
 * Mod must be prime.
 * Mod must be less than 2^62
*/

class dynamic_montgomery {
private:
    int64_t value;
    inline static uint64_t mod = 0;
    inline static uint64_t inv_neg_mod = 0;
    inline static uint64_t neg_mod = 0;

    static uint64_t reduce(const __uint128_t &value) {
        return (value + __uint128_t(uint64_t(value) * inv_neg_mod) * mod) >> 64;
    }

public:
    static void set_mod(uint64_t new_mod) {
        assert(new_mod <= uint64_t(1) << 62);
        mod = new_mod;

        inv_neg_mod = mod;
        for (int i = 0; i < 6; ++i) {
            inv_neg_mod *= uint64_t(2) - mod * inv_neg_mod;
        }
        inv_neg_mod *= -1;
        assert(mod * inv_neg_mod == uint64_t(-1));

        neg_mod = (-__uint128_t(mod)) % mod;
    }

    static uint64_t get_mod() {
        return mod;
    }

    dynamic_montgomery() : value(0) {}
    dynamic_montgomery(const dynamic_montgomery &x) : value(x.value) {}

    template<typename T, typename U = std::enable_if_t<std::is_integral<T>::value>>
    dynamic_montgomery(const T &x) : value(!x ? 0 : reduce(__uint128_t(x % int64_t(mod) + int64_t(mod)) * neg_mod)) {}

    uint64_t get() const {
        auto real_value = reduce(value);
        return real_value < mod ? real_value : real_value - mod;
    }

    template<typename T>
    dynamic_montgomery power(T degree) const {
        degree = (degree % int64_t(mod - 1) + int64_t(mod - 1)) % int64_t(mod - 1);
        dynamic_montgomery prod = 1, a = *this;
        for (; degree > 0; degree >>= 1, a *= a)
            if (degree & 1)
                prod *= a;

        return prod;
    }

    dynamic_montgomery inv() const {
        return power(-1);
    }

    dynamic_montgomery& operator=(const dynamic_montgomery &x) {
        value = x.value;
        return *this;
    }

    dynamic_montgomery& operator+=(const dynamic_montgomery &x) {
        if (int64_t(value += x.value - (mod << 1)) < 0) {
            value += mod << 1;
        }
        return *this;
    }

    dynamic_montgomery& operator-=(const dynamic_montgomery &x) {
        if (int64_t(value -= x.value) < 0) {
            value += mod << 1;
        }
        return *this;
    }

    dynamic_montgomery& operator*=(const dynamic_montgomery &x) {
        value = reduce(__uint128_t(value) * x.value);
        return *this;
    }

    dynamic_montgomery& operator/=(const dynamic_montgomery &x) {
        return *this *= x.inv();
    }

    friend dynamic_montgomery operator+(const dynamic_montgomery &x, const dynamic_montgomery &y) {
        return dynamic_montgomery(x) += y;
    }

    friend dynamic_montgomery operator-(const dynamic_montgomery &x, const dynamic_montgomery &y) {
        return dynamic_montgomery(x) -= y;
    }

    friend dynamic_montgomery operator*(const dynamic_montgomery &x, const dynamic_montgomery &y) {
        return dynamic_montgomery(x) *= y;
    }

    friend dynamic_montgomery operator/(const dynamic_montgomery &x, const dynamic_montgomery &y) {
        return dynamic_montgomery(x) /= y;
    }

    dynamic_montgomery operator-() const {
        return dynamic_montgomery(0) - *this;
    }
};

using mint = dynamic_montgomery;
