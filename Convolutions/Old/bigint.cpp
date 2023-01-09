struct bigint {
    std::vector<int> digits;
    bool positive;

    bigint(long long value = 0) {
        *this = bigint(std::to_string(value));
    }

    template<typename T>
    bigint(const std::vector<T> &v) : digits(v.begin(), v.end()), positive(true) {
        normalize();
    }

    bigint(std::string s) : positive(true) {
        assert(!s.empty());
        if (s[0] == '-') {
            positive = false;
            s = s.substr(1);
        }

        for (auto c : s)
            assert(std::isdigit(c));

        std::reverse(s.begin(), s.end());
        digits.resize(s.size());
        for (int i = 0; i < int(s.size()); i++)
            digits[i] = s[i] - '0';
    }

    explicit operator std::string() const {
        std::string res;
        if (!positive)
            res += '-';

        for (int i = int(digits.size()) - 1; i >= 0; i--)
            res += '0' + digits[i];

        return res;
    }

    template<typename T>
    explicit operator T() const {
        static_assert(std::is_integral<T>::value);
        T coeff = 1, value = 0;
        for (auto &x : digits) {
            value += x * coeff;
            coeff *= 10;
        }
        return value;
    }

    void normalize() {
        for (int i = 0; i < int(digits.size()); i++) {
            if (i == int(digits.size()) - 1 && digits[i] < 0) {
                positive = !positive;
                for (auto &x : digits)
                    x *= -1;

                normalize();
                return;
            }

            int delta = digits[i] < 0 ? -((-digits[i] + 9) / 10) : digits[i] / 10;
            digits[i] -= delta * 10;

            if (delta) {
                if (i == int(digits.size()) - 1)
                    digits.push_back(delta);
                else
                    digits[i + 1] += delta;
            }
        }

        while (digits.size() > 1 && digits.back() == 0)
            digits.pop_back();

        if (digits.size() == 1 && digits[0] == 0)
            positive = true;

        if (digits.empty()) {
            digits = {0};
            positive = true;
        }
    }

    bool operator==(const bigint&) const = default;

    std::strong_ordering operator<=>(const bigint &x) const {
        if (!positive && x.positive)
            return std::strong_ordering::less;

        if (positive && !x.positive)
            return std::strong_ordering::greater;

        if (digits.size() != x.digits.size())
            return ((digits.size() < x.digits.size()) ^ (!positive))
                ? std::strong_ordering::less : std::strong_ordering::greater;

        for (int i = int(digits.size()) - 1; i >= 0; i--)
            if (digits[i] != x.digits[i])
                return (digits[i] < x.digits[i]) ^ (!positive)
                    ? std::strong_ordering::less : std::strong_ordering::greater;

        return std::strong_ordering::equal;
    }

    void to_positive() {
        if (!positive) {
            positive = true;
            for (auto &x : digits)
                x *= -1;
        }
    }

    bigint& operator+=(const bigint &x) {
        to_positive();
        if (digits.size() < x.digits.size())
            digits.resize(x.digits.size());

        for (int i = 0; i < int(x.digits.size()); i++)
            digits[i] += x.positive ? x.digits[i] : -x.digits[i];

        normalize();
        return *this;
    }

    bigint& operator-=(const bigint &x) {
        to_positive();
        if (digits.size() < x.digits.size())
            digits.resize(x.digits.size());

        for (int i = 0; i < int(x.digits.size()); i++)
            digits[i] -= x.positive ? x.digits[i] : -x.digits[i];

        normalize();
        return *this;
    }

    bigint& operator*=(const bigint &x) {
        positive = !(positive ^ x.positive);
        digits = FFT::multiply<int, double>(digits.begin(), digits.end(),
                                    x.digits.begin(), x.digits.end());
        normalize();
        return *this;
    }

    bigint shift_right(int shift) const {
        bigint res;
        if (shift >= int(digits.size())) {
            res.digits = {0};
            res.positive = true;
            return res;
        }

        res.digits = std::vector<int>(digits.begin() + shift, digits.end());
        return res;
    }

    bigint shift_left(int shift) const {
        bigint res;
        res.digits = std::vector<int>(shift, 0);
        res.digits.insert(res.digits.end(), digits.begin(), digits.end());
        return res;
    }

    std::pair<bigint, bigint> divide21(const bigint &a, const bigint &b, int n) const {
        if (a < b)
            return {bigint(0), a};

        if (n <= 9) {
            long long first = (long long) a, second = (long long) b;
            return {bigint(first / second), bigint(first % second)};
        }

        auto c = a.shift_right(n / 2);
        auto [coeff, rem] = divide32(c, b, n / 2);

        auto [coeff2, rem2] = divide32(rem.shift_left(n / 2)
            + bigint(std::vector<int>(a.digits.begin(), a.digits.begin() + std::min<int>(n / 2, a.digits.size()))),
        b, n / 2);

        return {coeff.shift_left(n / 2) + coeff2, rem2};
    }

    std::pair<bigint, bigint> divide32(const bigint &a, const bigint &b, int n) const {
        if (a < b)
            return {bigint(0), a};

        if (int(b.digits.size()) <= n)
            return divide21(a, b, n);

        if (n <= 6) {
            long long first = (long long) a, second = (long long) b;
            return {bigint(first / second), bigint(first % second)};
        }

        bigint coeff;
        int k = int(b.digits.size()) - n;
        auto a1 = a.shift_right(k), b1 = b.shift_right(k);

        if (b1.shift_left(n) < a1)
            coeff = bigint(1).shift_left(n);
        else
            coeff = divide21(a1, b1, n).first;

        auto current = a - b * coeff;
        while (current < 0) {
            current += b;
            coeff--;
        }

        return {coeff, current};
    }

    std::pair<bigint, bigint> divide(bigint a, bigint b) const {
        if (a < b)
            return {bigint(0), b};

        int size = 1;
        while (size < int(std::max(a.digits.size(), b.digits.size())))
            size <<= 1;

        auto [coeff, rem] = divide32(a, b, size);
        coeff.normalize();
        rem.normalize();
        return {coeff, rem};
    }

    bigint& operator/=(const bigint &x) {
        *this = divide(*this, x).first;
        positive = !(positive ^ x.positive);
        return *this;
    }

    bigint& operator%=(const bigint &x) {
        *this = divide(*this, x).second;
        positive = true;
        return*this;
    }

    friend bigint operator+(const bigint &a, const bigint &b) {
        return bigint(a) += b;
    }

    friend bigint operator-(const bigint &a, const bigint &b) {
        return bigint(a) -= b;
    }

    friend bigint operator*(const bigint &a, const bigint &b) {
        return bigint(a) *= b;
    }

    friend bigint operator/(const bigint &a, const bigint &b) {
        return bigint(a) /= b;
    }

    friend bigint operator%(const bigint &a, const bigint &b) {
        return bigint(a) %= b;
    }

    bigint& operator++() {
        return *this += 1;
    }

    bigint& operator--() {
        return *this -= 1;
    }

    bigint operator++(int) {
        return *this += 1;
    }

    bigint operator--(int) {
        return *this -= 1;
    }

    friend std::istream& operator>>(std::istream &in, bigint &x) {
        std::string s;
        in >> s;
        x = bigint(s);
        return in;
    }

    friend std::ostream& operator<<(std::ostream &out, const bigint &x) {
        return out << std::string(x);
    }
};
