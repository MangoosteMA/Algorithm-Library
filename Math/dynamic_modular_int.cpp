template<typename T>
int normalize(T value, int mod) {
    if (value < -mod || value >= 2 * mod)
        value %= mod;

    if (value < 0)
        value += mod;

    if (value >= mod)
        value -= mod;

    return value;
}

template<typename T>
struct dynamic_modular_int {
    using mint = dynamic_modular_int<T>;

    int value;

    dynamic_modular_int() : value(0) {}
    dynamic_modular_int(const mint &x) : value(x.value) {}

    template<typename U, typename V = std::enable_if_t<std::is_integral<U>::value>>
    dynamic_modular_int(U value) : value(normalize(value, T::mod)) {}

    template<typename U>
    mint power(U degree) const {
        mint prod = 1;
        mint a = *this;

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
        value += x.value;
        if (value >= T::mod)
            value -= T::mod;

        return *this;
    }

    mint& operator-=(const mint &x) {
        value -= x.value;
        if (value < 0)
            value += T::mod;

        return *this;
    }

    mint& operator*=(const mint &x) {
        value = (long long) value * x.value % T::mod;
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
        ++value;
        if (value == T::mod)
            value = 0;

        return *this;
    }

    mint& operator--() {
        --value;
        if (value == -1)
            value = T::mod - 1;

        return *this;
    }

    mint operator++(int) {
        mint prev = *this;
        value++;
        if (value == T::mod)
            value = 0;

        return prev;
    }

    mint operator--(int) {
        mint prev = *this;
        value--;
        if (value == -1)
            value = T::mod - 1;

        return prev;
    }

    mint operator-() const {
        return mint(0) - *this;
    }

    bool operator==(const mint &x) const {
        return value == x.value;
    }

    bool operator!=(const mint &x) const {
        return value != x.value;
    }

    template<typename U>
    explicit operator U() {
        return value;
    }

    friend std::istream& operator>>(std::istream &in, mint &x) {
        std::string s;
        in >> s;
        x = 0;
        for (const auto c : s)
            x = x * 10 + (c - '0');

        return in;
    }

    friend std::ostream& operator<<(std::ostream &out, const mint &x) {
        return out << x.value;
    }

    static int primitive_root() {
        if constexpr (T::mod == 1'000'000'007)
            return 5;

        if constexpr (T::mod == 998'244'353)
            return 3;

        if constexpr (T::mod == 786433)
            return 10;

        static int root = -1;
        if (root != -1)
            return root;

        std::vector<int> primes;
        int value = T::mod - 1;
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
                if ((mint(r).power((T::mod - 1) / p)).value == 1) {
                    ok = false;
                    break;
                }

            if (ok)
                return root = r;
        }
    }
};

struct dynamic_modular_int_mod {
    static int mod;
};

int dynamic_modular_int_mod::mod = 0;
int &MOD = dynamic_modular_int_mod::mod;

using mint = dynamic_modular_int<dynamic_modular_int_mod>;
