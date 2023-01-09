// ! warning: insert this above your template, cause it will get runtime error otherwise (I don't know why ;) )
// source: https://judge.yosupo.jp/submission/92714

#line 1 "test-oj/a.cpp"
// verification-helper: PROBLEM https://judge.yosupo.jp/problem/convolution_mod
#pragma GCC target("avx2")
#include <cstdio>

#line 2 "yosupo/fastio.hpp"

#include <unistd.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <cstring>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#line 2 "yosupo/bit.hpp"

namespace yosupo {

namespace internal {

// @param n `0 <= n`
// @return minimum non-negative `x` s.t. `n <= 2**x`
int ceil_pow2(int n) {
    int x = 0;
    while ((1U << x) < (unsigned int)(n)) x++;
    return x;
}

}  // namespace internal

// @param n `1 <= n`
// @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsf(unsigned int n) { return __builtin_ctz(n); }
// @param n `1 <= n`
// @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsf(unsigned long n) { return __builtin_ctzl(n); }
// @param n `1 <= n`
// @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsf(unsigned long long n) { return __builtin_ctzll(n); }
// @param n `1 <= n`
// @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsf(unsigned __int128 n) {
    unsigned long long low = (unsigned long long)(n);
    unsigned long long high = (unsigned long long)(n >> 64);
    return low ? __builtin_ctzll(low) : 64 + __builtin_ctzll(high);
}

// @param n `1 <= n`
// @return maximum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsr(unsigned int n) {
    return 8 * (int)sizeof(unsigned int) - 1 - __builtin_clz(n);
}
// @param n `1 <= n`
// @return maximum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsr(unsigned long n) {
    return 8 * (int)sizeof(unsigned long) - 1 - __builtin_clzl(n);
}
// @param n `1 <= n`
// @return maximum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsr(unsigned long long n) {
    return 8 * (int)sizeof(unsigned long long) - 1 - __builtin_clzll(n);
}
// @param n `1 <= n`
// @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsr(unsigned __int128 n) {
    unsigned long long low = (unsigned long long)(n);
    unsigned long long high = (unsigned long long)(n >> 64);
    return high ? 127 - __builtin_clzll(high) : 63 - __builtin_ctzll(low);
}

int popcnt(unsigned int n) { return __builtin_popcount(n); }
int popcnt(unsigned long n) { return __builtin_popcountl(n); }
int popcnt(unsigned long long n) { return __builtin_popcountll(n); }

}  // namespace yosupo
#line 2 "yosupo/internal_type_traits.hpp"

#line 4 "yosupo/internal_type_traits.hpp"
#include <numeric>
#line 6 "yosupo/internal_type_traits.hpp"

namespace yosupo {

namespace internal {

template <class T>
using is_signed_int128 =
    typename std::conditional<std::is_same<T, __int128_t>::value ||
                                  std::is_same<T, __int128>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using is_unsigned_int128 =
    typename std::conditional<std::is_same<T, __uint128_t>::value ||
                                  std::is_same<T, unsigned __int128>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using make_unsigned_int128 =
    typename std::conditional<std::is_same<T, __int128_t>::value,
                              __uint128_t,
                              unsigned __int128>;

template <class T>
using is_integral =
    typename std::conditional<std::is_integral<T>::value ||
                                  internal::is_signed_int128<T>::value ||
                                  internal::is_unsigned_int128<T>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using is_signed_int = typename std::conditional<(is_integral<T>::value &&
                                                 std::is_signed<T>::value) ||
                                                    is_signed_int128<T>::value,
                                                std::true_type,
                                                std::false_type>::type;

template <class T>
using is_unsigned_int =
    typename std::conditional<(is_integral<T>::value &&
                               std::is_unsigned<T>::value) ||
                                  is_unsigned_int128<T>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using to_unsigned = typename std::conditional<
    is_signed_int128<T>::value,
    make_unsigned_int128<T>,
    typename std::conditional<std::is_signed<T>::value,
                              std::make_unsigned<T>,
                              std::common_type<T>>::type>::type;

template <class T>
using is_integral_t = std::enable_if_t<is_integral<T>::value>;

template <class T>
using is_signed_int_t = std::enable_if_t<is_signed_int<T>::value>;

template <class T>
using is_unsigned_int_t = std::enable_if_t<is_unsigned_int<T>::value>;

template <class T> using to_unsigned_t = typename to_unsigned<T>::type;

}  // namespace internal

}  // namespace yosupo
#line 16 "yosupo/fastio.hpp"

namespace yosupo {

struct Scanner {
  public:
    Scanner(const Scanner&) = delete;
    Scanner& operator=(const Scanner&) = delete;

    Scanner(FILE* fp) : fd(fileno(fp)) {}

    void read() {}
    template <class H, class... T> void read(H& h, T&... t) {
        bool f = read_single(h);
        assert(f);
        read(t...);
    }

    int read_unsafe() { return 0; }
    template <class H, class... T> int read_unsafe(H& h, T&... t) {
        bool f = read_single(h);
        if (!f) return 0;
        return 1 + read_unsafe(t...);
    }

    int close() { return ::close(fd); }

  private:
    static constexpr int SIZE = 1 << 15;

    int fd = -1;
    std::array<char, SIZE + 1> line;
    int st = 0, ed = 0;
    bool eof = false;

    bool read_single(std::string& ref) {
        if (!skip_space()) return false;
        ref = "";
        while (true) {
            char c = top();
            if (c <= ' ') break;
            ref += c;
            st++;
        }
        return true;
    }
    bool read_single(double& ref) {
        std::string s;
        if (!read_single(s)) return false;
        ref = std::stod(s);
        return true;
    }

    template <class T,
              std::enable_if_t<std::is_same<T, char>::value>* = nullptr>
    bool read_single(T& ref) {
        if (!skip_space<50>()) return false;
        ref = top();
        st++;
        return true;
    }

    template <class T,
              internal::is_signed_int_t<T>* = nullptr,
              std::enable_if_t<!std::is_same<T, char>::value>* = nullptr>
    bool read_single(T& sref) {
        using U = internal::to_unsigned_t<T>;
        if (!skip_space<50>()) return false;
        bool neg = false;
        if (line[st] == '-') {
            neg = true;
            st++;
        }
        U ref = 0;
        do {
            ref = 10 * ref + (line[st++] & 0x0f);
        } while (line[st] >= '0');
        sref = neg ? -ref : ref;
        return true;
    }
    template <class U,
              internal::is_unsigned_int_t<U>* = nullptr,
              std::enable_if_t<!std::is_same<U, char>::value>* = nullptr>
    bool read_single(U& ref) {
        if (!skip_space<50>()) return false;
        ref = 0;
        do {
            ref = 10 * ref + (line[st++] & 0x0f);
        } while (line[st] >= '0');
        return true;
    }

    bool reread() {
        if (ed - st >= 50) return true;
        if (st > SIZE / 2) {
            std::memmove(line.data(), line.data() + st, ed - st);
            ed -= st;
            st = 0;
        }
        if (eof) return false;
        auto u = ::read(fd, line.data() + ed, SIZE - ed);
        if (u == 0) {
            eof = true;
            line[ed] = '\0';
            u = 1;
        }
        ed += int(u);
        line[ed] = char(127);
        return true;
    }

    char top() {
        if (st == ed) {
            bool f = reread();
            assert(f);
        }
        return line[st];
    }

    template <int TOKEN_LEN = 0>
    bool skip_space() {
        while (true) {
            while (line[st] <= ' ') st++;   
            if (ed - st > TOKEN_LEN) return true;
            if (st > ed) st = ed;
            for (auto i = st; i < ed; i++) {
                if (line[i] <= ' ') return true;
            }
            if (!reread()) return false;
        }
    }
};

struct Printer {
  public:
    template <char sep = ' ', bool F = false> void write() {}
    template <char sep = ' ', bool F = false, class H, class... T>
    void write(const H& h, const T&... t) {
        if (F) write_single(sep);
        write_single(h);
        write<true>(t...);
    }
    template <char sep = ' ', class... T> void writeln(const T&... t) {
        write<sep>(t...);
        write_single('\n');
    }

    Printer(FILE* _fp) : fd(fileno(_fp)) {}
    ~Printer() { flush(); }

    int close() {
        flush();
        return ::close(fd);
    }

    void flush() {
        if (pos) {
            auto res = ::write(fd, line.data(), pos);
            assert(res != -1);
            pos = 0;
        }
    }

  private:
    static std::array<std::array<char, 2>, 100> small;
    static std::array<unsigned long long, 20> tens;

    static constexpr size_t SIZE = 1 << 15;
    int fd;
    std::array<char, SIZE> line;
    size_t pos = 0;
    std::stringstream ss;

    template <class T,
              std::enable_if_t<std::is_same<char, T>::value>* = nullptr>
    void write_single(const T& val) {
        if (pos == SIZE) flush();
        line[pos++] = val;
    }

    template <class T,
              internal::is_signed_int_t<T>* = nullptr,
              std::enable_if_t<!std::is_same<char, T>::value>* = nullptr>
    void write_single(const T& val) {
        using U = internal::to_unsigned_t<T>;
        if (val == 0) {
            write_single('0');
            return;
        }
        if (pos > SIZE - 50) flush();
        U uval = val;
        if (val < 0) {
            write_single('-');
            uval = -uval;
        }
        write_unsigned(uval);
    }

    template <class U, internal::is_unsigned_int_t<U>* = nullptr>
    void write_single(U uval) {
        if (uval == 0) {
            write_single('0');
            return;
        }
        if (pos > SIZE - 50) flush();

        write_unsigned(uval);
    }

    template <class U, internal::is_unsigned_int_t<U>* = nullptr>
    static int calc_len(U x) {
        int i = (bsr(x) * 3 + 3) / 10;
        if (x < tens[i])
            return i;
        else
            return i + 1;
    }

    template <class U,
              internal::is_unsigned_int_t<U>* = nullptr,
              std::enable_if_t<2 >= sizeof(U)>* = nullptr>
    void write_unsigned(U uval) {
        size_t len = calc_len(uval);
        pos += len;

        char* ptr = line.data() + pos;
        while (uval >= 100) {
            ptr -= 2;
            memcpy(ptr, small[uval % 100].data(), 2);
            uval /= 100;
        }
        if (uval >= 10) {
            memcpy(ptr - 2, small[uval].data(), 2);
        } else {
            *(ptr - 1) = char('0' + uval);
        }
    }

    template <class U,
              internal::is_unsigned_int_t<U>* = nullptr,
              std::enable_if_t<4 == sizeof(U)>* = nullptr>
    void write_unsigned(U uval) {
        std::array<char, 8> buf;
        memcpy(buf.data() + 6, small[uval % 100].data(), 2);
        memcpy(buf.data() + 4, small[uval / 100 % 100].data(), 2);
        memcpy(buf.data() + 2, small[uval / 10000 % 100].data(), 2);
        memcpy(buf.data() + 0, small[uval / 1000000 % 100].data(), 2);

        if (uval >= 100000000) {
            if (uval >= 1000000000) {
                memcpy(line.data() + pos, small[uval / 100000000 % 100].data(), 2);
                pos += 2;
            } else {
                line[pos] = char('0' + uval / 100000000);
                pos++;
            }
            memcpy(line.data() + pos, buf.data(), 8);
            pos += 8;
        } else {
            size_t len = calc_len(uval);
            memcpy(line.data() + pos, buf.data() + (8 - len), len);
            pos += len;
        }
    }

    template <class U,
              internal::is_unsigned_int_t<U>* = nullptr,
              std::enable_if_t<8 == sizeof(U)>* = nullptr>
    void write_unsigned(U uval) {
        size_t len = calc_len(uval);
        pos += len;

        char* ptr = line.data() + pos;
        while (uval >= 100) {
            ptr -= 2;
            memcpy(ptr, small[uval % 100].data(), 2);
            uval /= 100;
        }
        if (uval >= 10) {
            memcpy(ptr - 2, small[uval].data(), 2);
        } else {
            *(ptr - 1) = char('0' + uval);
        }
    }

    template <
        class U,
        std::enable_if_t<internal::is_unsigned_int128<U>::value>* = nullptr>
    void write_unsigned(U uval) {
        static std::array<char, 50> buf;
        size_t len = 0;
        while (uval > 0) {
            buf[len++] = char((uval % 10) + '0');
            uval /= 10;
        }
        std::reverse(buf.begin(), buf.begin() + len);
        memcpy(line.data() + pos, buf.data(), len);
        pos += len;
    }

    void write_single(const std::string& s) {
        for (char c : s) write_single(c);
    }
    void write_single(const char* s) {
        size_t len = strlen(s);
        for (size_t i = 0; i < len; i++) write_single(s[i]);
    }
    template <class T> void write_single(const std::vector<T>& val) {
        auto n = val.size();
        for (size_t i = 0; i < n; i++) {
            if (i) write_single(' ');
            write_single(val[i]);
        }
    }
};

std::array<std::array<char, 2>, 100> Printer::small = [] {
    std::array<std::array<char, 2>, 100> table;
    for (int i = 0; i <= 99; i++) {
        table[i][1] = char('0' + (i % 10));
        table[i][0] = char('0' + (i / 10 % 10));
    }
    return table;
}();
std::array<unsigned long long, 20> Printer::tens = [] {
    std::array<unsigned long long, 20> table;
    for (int i = 0; i < 20; i++) {
        table[i] = 1;
        for (int j = 0; j < i; j++) {
            table[i] *= 10;
        }
    }
    return table;
}();

}  // namespace yosupo
#line 2 "yosupo/modint.hpp"

#line 1 "/home/yosupo/programs/ac-library/atcoder/modint.hpp"



#line 7 "/home/yosupo/programs/ac-library/atcoder/modint.hpp"

#ifdef _MSC_VER
#include <intrin.h>
#endif

#line 1 "/home/yosupo/programs/ac-library/atcoder/internal_math.hpp"



#include <utility>

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace atcoder {

namespace internal {

// @param m `1 <= m`
// @return x mod m
constexpr long long safe_mod(long long x, long long m) {
    x %= m;
    if (x < 0) x += m;
    return x;
}

// Fast modular multiplication by barrett reduction
// Reference: https://en.wikipedia.org/wiki/Barrett_reduction
// NOTE: reconsider after Ice Lake
struct barrett {
    unsigned int _m;
    unsigned long long im;

    // @param m `1 <= m < 2^31`
    explicit barrett(unsigned int m) : _m(m), im((unsigned long long)(-1) / m + 1) {}

    // @return m
    unsigned int umod() const { return _m; }

    // @param a `0 <= a < m`
    // @param b `0 <= b < m`
    // @return `a * b % m`
    unsigned int mul(unsigned int a, unsigned int b) const {
        // [1] m = 1
        // a = b = im = 0, so okay

        // [2] m >= 2
        // im = ceil(2^64 / m)
        // -> im * m = 2^64 + r (0 <= r < m)
        // let z = a*b = c*m + d (0 <= c, d < m)
        // a*b * im = (c*m + d) * im = c*(im*m) + d*im = c*2^64 + c*r + d*im
        // c*r + d*im < m * m + m * im < m * m + 2^64 + m <= 2^64 + m * (m + 1) < 2^64 * 2
        // ((ab * im) >> 64) == c or c + 1
        unsigned long long z = a;
        z *= b;
#ifdef _MSC_VER
        unsigned long long x;
        _umul128(z, im, &x);
#else
        unsigned long long x =
            (unsigned long long)(((unsigned __int128)(z)*im) >> 64);
#endif
        unsigned int v = (unsigned int)(z - x * _m);
        if (_m <= v) v += _m;
        return v;
    }
};

// @param n `0 <= n`
// @param m `1 <= m`
// @return `(x ** n) % m`
constexpr long long pow_mod_constexpr(long long x, long long n, int m) {
    if (m == 1) return 0;
    unsigned int _m = (unsigned int)(m);
    unsigned long long r = 1;
    unsigned long long y = safe_mod(x, m);
    while (n) {
        if (n & 1) r = (r * y) % _m;
        y = (y * y) % _m;
        n >>= 1;
    }
    return r;
}

// Reference:
// M. Forisek and J. Jancina,
// Fast Primality Testing for Integers That Fit into a Machine Word
// @param n `0 <= n`
constexpr bool is_prime_constexpr(int n) {
    if (n <= 1) return false;
    if (n == 2 || n == 7 || n == 61) return true;
    if (n % 2 == 0) return false;
    long long d = n - 1;
    while (d % 2 == 0) d /= 2;
    constexpr long long bases[3] = {2, 7, 61};
    for (long long a : bases) {
        long long t = d;
        long long y = pow_mod_constexpr(a, t, n);
        while (t != n - 1 && y != 1 && y != n - 1) {
            y = y * y % n;
            t <<= 1;
        }
        if (y != n - 1 && t % 2 == 0) {
            return false;
        }
    }
    return true;
}
template <int n> constexpr bool is_prime = is_prime_constexpr(n);

// @param b `1 <= b`
// @return pair(g, x) s.t. g = gcd(a, b), xa = g (mod b), 0 <= x < b/g
constexpr std::pair<long long, long long> inv_gcd(long long a, long long b) {
    a = safe_mod(a, b);
    if (a == 0) return {b, 0};

    // Contracts:
    // [1] s - m0 * a = 0 (mod b)
    // [2] t - m1 * a = 0 (mod b)
    // [3] s * |m1| + t * |m0| <= b
    long long s = b, t = a;
    long long m0 = 0, m1 = 1;

    while (t) {
        long long u = s / t;
        s -= t * u;
        m0 -= m1 * u;  // |m1 * u| <= |m1| * s <= b

        // [3]:
        // (s - t * u) * |m1| + t * |m0 - m1 * u|
        // <= s * |m1| - t * u * |m1| + t * (|m0| + |m1| * u)
        // = s * |m1| + t * |m0| <= b

        auto tmp = s;
        s = t;
        t = tmp;
        tmp = m0;
        m0 = m1;
        m1 = tmp;
    }
    // by [3]: |m0| <= b/g
    // by g != b: |m0| < b/g
    if (m0 < 0) m0 += b / s;
    return {s, m0};
}

// Compile time primitive root
// @param m must be prime
// @return primitive root (and minimum in now)
constexpr int primitive_root_constexpr(int m) {
    if (m == 2) return 1;
    if (m == 167772161) return 3;
    if (m == 469762049) return 3;
    if (m == 754974721) return 11;
    if (m == 998244353) return 3;
    int divs[20] = {};
    divs[0] = 2;
    int cnt = 1;
    int x = (m - 1) / 2;
    while (x % 2 == 0) x /= 2;
    for (int i = 3; (long long)(i)*i <= x; i += 2) {
        if (x % i == 0) {
            divs[cnt++] = i;
            while (x % i == 0) {
                x /= i;
            }
        }
    }
    if (x > 1) {
        divs[cnt++] = x;
    }
    for (int g = 2;; g++) {
        bool ok = true;
        for (int i = 0; i < cnt; i++) {
            if (pow_mod_constexpr(g, (m - 1) / divs[i], m) == 1) {
                ok = false;
                break;
            }
        }
        if (ok) return g;
    }
}
template <int m> constexpr int primitive_root = primitive_root_constexpr(m);

// @param n `n < 2^32`
// @param m `1 <= m < 2^32`
// @return sum_{i=0}^{n-1} floor((ai + b) / m) (mod 2^64)
unsigned long long floor_sum_unsigned(unsigned long long n,
                                      unsigned long long m,
                                      unsigned long long a,
                                      unsigned long long b) {
    unsigned long long ans = 0;
    while (true) {
        if (a >= m) {
            ans += n * (n - 1) / 2 * (a / m);
            a %= m;
        }
        if (b >= m) {
            ans += n * (b / m);
            b %= m;
        }

        unsigned long long y_max = a * n + b;
        if (y_max < m) break;
        // y_max < m * (n + 1)
        // floor(y_max / m) <= n
        n = (unsigned long long)(y_max / m);
        b = (unsigned long long)(y_max % m);
        std::swap(m, a);
    }
    return ans;
}

}  // namespace internal

}  // namespace atcoder


#line 1 "/home/yosupo/programs/ac-library/atcoder/internal_type_traits.hpp"



#line 7 "/home/yosupo/programs/ac-library/atcoder/internal_type_traits.hpp"

namespace atcoder {

namespace internal {

#ifndef _MSC_VER
template <class T>
using is_signed_int128 =
    typename std::conditional<std::is_same<T, __int128_t>::value ||
                                  std::is_same<T, __int128>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using is_unsigned_int128 =
    typename std::conditional<std::is_same<T, __uint128_t>::value ||
                                  std::is_same<T, unsigned __int128>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using make_unsigned_int128 =
    typename std::conditional<std::is_same<T, __int128_t>::value,
                              __uint128_t,
                              unsigned __int128>;

template <class T>
using is_integral = typename std::conditional<std::is_integral<T>::value ||
                                                  is_signed_int128<T>::value ||
                                                  is_unsigned_int128<T>::value,
                                              std::true_type,
                                              std::false_type>::type;

template <class T>
using is_signed_int = typename std::conditional<(is_integral<T>::value &&
                                                 std::is_signed<T>::value) ||
                                                    is_signed_int128<T>::value,
                                                std::true_type,
                                                std::false_type>::type;

template <class T>
using is_unsigned_int =
    typename std::conditional<(is_integral<T>::value &&
                               std::is_unsigned<T>::value) ||
                                  is_unsigned_int128<T>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using to_unsigned = typename std::conditional<
    is_signed_int128<T>::value,
    make_unsigned_int128<T>,
    typename std::conditional<std::is_signed<T>::value,
                              std::make_unsigned<T>,
                              std::common_type<T>>::type>::type;

#else

template <class T> using is_integral = typename std::is_integral<T>;

template <class T>
using is_signed_int =
    typename std::conditional<is_integral<T>::value && std::is_signed<T>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using is_unsigned_int =
    typename std::conditional<is_integral<T>::value &&
                                  std::is_unsigned<T>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using to_unsigned = typename std::conditional<is_signed_int<T>::value,
                                              std::make_unsigned<T>,
                                              std::common_type<T>>::type;

#endif

template <class T>
using is_signed_int_t = std::enable_if_t<is_signed_int<T>::value>;

template <class T>
using is_unsigned_int_t = std::enable_if_t<is_unsigned_int<T>::value>;

template <class T> using to_unsigned_t = typename to_unsigned<T>::type;

}  // namespace internal

}  // namespace atcoder


#line 14 "/home/yosupo/programs/ac-library/atcoder/modint.hpp"

namespace atcoder {

namespace internal {

struct modint_base {};
struct static_modint_base : modint_base {};

template <class T> using is_modint = std::is_base_of<modint_base, T>;
template <class T> using is_modint_t = std::enable_if_t<is_modint<T>::value>;

}  // namespace internal

template <int m, std::enable_if_t<(1 <= m)>* = nullptr>
struct static_modint : internal::static_modint_base {
    using mint = static_modint;

  public:
    static constexpr int mod() { return m; }
    static mint raw(int v) {
        mint x;
        x._v = v;
        return x;
    }

    static_modint() : _v(0) {}
    template <class T, internal::is_signed_int_t<T>* = nullptr>
    static_modint(T v) {
        long long x = (long long)(v % (long long)(umod()));
        if (x < 0) x += umod();
        _v = (unsigned int)(x);
    }
    template <class T, internal::is_unsigned_int_t<T>* = nullptr>
    static_modint(T v) {
        _v = (unsigned int)(v % umod());
    }

    unsigned int val() const { return _v; }

    mint& operator++() {
        _v++;
        if (_v == umod()) _v = 0;
        return *this;
    }
    mint& operator--() {
        if (_v == 0) _v = umod();
        _v--;
        return *this;
    }
    mint operator++(int) {
        mint result = *this;
        ++*this;
        return result;
    }
    mint operator--(int) {
        mint result = *this;
        --*this;
        return result;
    }

    mint& operator+=(const mint& rhs) {
        _v += rhs._v;
        if (_v >= umod()) _v -= umod();
        return *this;
    }
    mint& operator-=(const mint& rhs) {
        _v -= rhs._v;
        if (_v >= umod()) _v += umod();
        return *this;
    }
    mint& operator*=(const mint& rhs) {
        unsigned long long z = _v;
        z *= rhs._v;
        _v = (unsigned int)(z % umod());
        return *this;
    }
    mint& operator/=(const mint& rhs) { return *this = *this * rhs.inv(); }

    mint operator+() const { return *this; }
    mint operator-() const { return mint() - *this; }

    mint pow(long long n) const {
        assert(0 <= n);
        mint x = *this, r = 1;
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }
    mint inv() const {
        if (prime) {
            assert(_v);
            return pow(umod() - 2);
        } else {
            auto eg = internal::inv_gcd(_v, m);
            assert(eg.first == 1);
            return eg.second;
        }
    }

    friend mint operator+(const mint& lhs, const mint& rhs) {
        return mint(lhs) += rhs;
    }
    friend mint operator-(const mint& lhs, const mint& rhs) {
        return mint(lhs) -= rhs;
    }
    friend mint operator*(const mint& lhs, const mint& rhs) {
        return mint(lhs) *= rhs;
    }
    friend mint operator/(const mint& lhs, const mint& rhs) {
        return mint(lhs) /= rhs;
    }
    friend bool operator==(const mint& lhs, const mint& rhs) {
        return lhs._v == rhs._v;
    }
    friend bool operator!=(const mint& lhs, const mint& rhs) {
        return lhs._v != rhs._v;
    }

  private:
    unsigned int _v;
    static constexpr unsigned int umod() { return m; }
    static constexpr bool prime = internal::is_prime<m>;
};

template <int id> struct dynamic_modint : internal::modint_base {
    using mint = dynamic_modint;

  public:
    static int mod() { return (int)(bt.umod()); }
    static void set_mod(int m) {
        assert(1 <= m);
        bt = internal::barrett(m);
    }
    static mint raw(int v) {
        mint x;
        x._v = v;
        return x;
    }

    dynamic_modint() : _v(0) {}
    template <class T, internal::is_signed_int_t<T>* = nullptr>
    dynamic_modint(T v) {
        long long x = (long long)(v % (long long)(mod()));
        if (x < 0) x += mod();
        _v = (unsigned int)(x);
    }
    template <class T, internal::is_unsigned_int_t<T>* = nullptr>
    dynamic_modint(T v) {
        _v = (unsigned int)(v % mod());
    }

    unsigned int val() const { return _v; }

    mint& operator++() {
        _v++;
        if (_v == umod()) _v = 0;
        return *this;
    }
    mint& operator--() {
        if (_v == 0) _v = umod();
        _v--;
        return *this;
    }
    mint operator++(int) {
        mint result = *this;
        ++*this;
        return result;
    }
    mint operator--(int) {
        mint result = *this;
        --*this;
        return result;
    }

    mint& operator+=(const mint& rhs) {
        _v += rhs._v;
        if (_v >= umod()) _v -= umod();
        return *this;
    }
    mint& operator-=(const mint& rhs) {
        _v += mod() - rhs._v;
        if (_v >= umod()) _v -= umod();
        return *this;
    }
    mint& operator*=(const mint& rhs) {
        _v = bt.mul(_v, rhs._v);
        return *this;
    }
    mint& operator/=(const mint& rhs) { return *this = *this * rhs.inv(); }

    mint operator+() const { return *this; }
    mint operator-() const { return mint() - *this; }

    mint pow(long long n) const {
        assert(0 <= n);
        mint x = *this, r = 1;
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }
    mint inv() const {
        auto eg = internal::inv_gcd(_v, mod());
        assert(eg.first == 1);
        return eg.second;
    }

    friend mint operator+(const mint& lhs, const mint& rhs) {
        return mint(lhs) += rhs;
    }
    friend mint operator-(const mint& lhs, const mint& rhs) {
        return mint(lhs) -= rhs;
    }
    friend mint operator*(const mint& lhs, const mint& rhs) {
        return mint(lhs) *= rhs;
    }
    friend mint operator/(const mint& lhs, const mint& rhs) {
        return mint(lhs) /= rhs;
    }
    friend bool operator==(const mint& lhs, const mint& rhs) {
        return lhs._v == rhs._v;
    }
    friend bool operator!=(const mint& lhs, const mint& rhs) {
        return lhs._v != rhs._v;
    }

  private:
    unsigned int _v;
    static internal::barrett bt;
    static unsigned int umod() { return bt.umod(); }
};
template <int id> internal::barrett dynamic_modint<id>::bt(998244353);

using modint998244353 = static_modint<998244353>;
using modint1000000007 = static_modint<1000000007>;
using modint = dynamic_modint<-1>;

namespace internal {

template <class T>
using is_static_modint = std::is_base_of<internal::static_modint_base, T>;

template <class T>
using is_static_modint_t = std::enable_if_t<is_static_modint<T>::value>;

template <class> struct is_dynamic_modint : public std::false_type {};
template <int id>
struct is_dynamic_modint<dynamic_modint<id>> : public std::true_type {};

template <class T>
using is_dynamic_modint_t = std::enable_if_t<is_dynamic_modint<T>::value>;

}  // namespace internal

}  // namespace atcoder


#line 4 "yosupo/modint.hpp"

#include <iostream>

namespace atcoder {

template <int MOD>
std::ostream& operator<<(std::ostream& os, const static_modint<MOD>& x) {
    return os << x.val();
}

template <int ID>
std::ostream& operator<<(std::ostream& os, const dynamic_modint<ID>& x) {
    return os << x.val();
}

}  // namespace atcoder

namespace yosupo {

template <int MOD> using static_modint = atcoder::static_modint<MOD>;

template <int ID> using dynamic_modint = atcoder::dynamic_modint<ID>;

using modint998244353 = atcoder::modint998244353;
using modint1000000007 = atcoder::modint1000000007;
using modint = atcoder::modint;

}  // namespace yosupo
#line 7 "test-oj/a.cpp"

#include <immintrin.h>
#line 11 "test-oj/a.cpp"

#line 1 "/home/yosupo/programs/ac-library/atcoder/convolution.hpp"



#line 9 "/home/yosupo/programs/ac-library/atcoder/convolution.hpp"

#line 1 "/home/yosupo/programs/ac-library/atcoder/internal_bit.hpp"



#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace atcoder {

namespace internal {

// @param n `0 <= n`
// @return minimum non-negative `x` s.t. `n <= 2**x`
int ceil_pow2(int n) {
    int x = 0;
    while ((1U << x) < (unsigned int)(n)) x++;
    return x;
}

// @param n `1 <= n`
// @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
constexpr int bsf_constexpr(unsigned int n) {
    int x = 0;
    while (!(n & (1 << x))) x++;
    return x;
}

// @param n `1 <= n`
// @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsf(unsigned int n) {
#ifdef _MSC_VER
    unsigned long index;
    _BitScanForward(&index, n);
    return index;
#else
    return __builtin_ctz(n);
#endif
}

}  // namespace internal

}  // namespace atcoder


#line 12 "/home/yosupo/programs/ac-library/atcoder/convolution.hpp"

namespace atcoder {

namespace internal {

template <class mint,
          int g = internal::primitive_root<mint::mod()>,
          internal::is_static_modint_t<mint>* = nullptr>
struct fft_info {
    static constexpr int rank2 = bsf_constexpr(mint::mod() - 1);
    std::array<mint, rank2 + 1> root;   // root[i]^(2^i) == 1
    std::array<mint, rank2 + 1> iroot;  // root[i] * iroot[i] == 1

    std::array<mint, std::max(0, rank2 - 2 + 1)> rate2;
    std::array<mint, std::max(0, rank2 - 2 + 1)> irate2;

    std::array<mint, std::max(0, rank2 - 3 + 1)> rate3;
    std::array<mint, std::max(0, rank2 - 3 + 1)> irate3;

    fft_info() {
        root[rank2] = mint(g).pow((mint::mod() - 1) >> rank2);
        iroot[rank2] = root[rank2].inv();
        for (int i = rank2 - 1; i >= 0; i--) {
            root[i] = root[i + 1] * root[i + 1];
            iroot[i] = iroot[i + 1] * iroot[i + 1];
        }

        {
            mint prod = 1, iprod = 1;
            for (int i = 0; i <= rank2 - 2; i++) {
                rate2[i] = root[i + 2] * prod;
                irate2[i] = iroot[i + 2] * iprod;
                prod *= iroot[i + 2];
                iprod *= root[i + 2];
            }
        }
        {
            mint prod = 1, iprod = 1;
            for (int i = 0; i <= rank2 - 3; i++) {
                rate3[i] = root[i + 3] * prod;
                irate3[i] = iroot[i + 3] * iprod;
                prod *= iroot[i + 3];
                iprod *= root[i + 3];
            }
        }
    }
};

template <class mint, internal::is_static_modint_t<mint>* = nullptr>
void butterfly(std::vector<mint>& a) {
    int n = int(a.size());
    int h = internal::ceil_pow2(n);

    static const fft_info<mint> info;

    int len = 0;  // a[i, i+(n>>len), i+2*(n>>len), ..] is transformed
    while (len < h) {
        if (h - len == 1) {
            int p = 1 << (h - len - 1);
            mint rot = 1;
            for (int s = 0; s < (1 << len); s++) {
                int offset = s << (h - len);
                for (int i = 0; i < p; i++) {
                    auto l = a[i + offset];
                    auto r = a[i + offset + p] * rot;
                    a[i + offset] = l + r;
                    a[i + offset + p] = l - r;
                }
                if (s + 1 != (1 << len))
                    rot *= info.rate2[bsf(~(unsigned int)(s))];
            }
            len++;
        } else {
            // 4-base
            int p = 1 << (h - len - 2);
            mint rot = 1, imag = info.root[2];
            for (int s = 0; s < (1 << len); s++) {
                mint rot2 = rot * rot;
                mint rot3 = rot2 * rot;
                int offset = s << (h - len);
                for (int i = 0; i < p; i++) {
                    auto mod2 = 1ULL * mint::mod() * mint::mod();
                    auto a0 = 1ULL * a[i + offset].val();
                    auto a1 = 1ULL * a[i + offset + p].val() * rot.val();
                    auto a2 = 1ULL * a[i + offset + 2 * p].val() * rot2.val();
                    auto a3 = 1ULL * a[i + offset + 3 * p].val() * rot3.val();
                    auto a1na3imag =
                        1ULL * mint(a1 + mod2 - a3).val() * imag.val();
                    auto na2 = mod2 - a2;
                    a[i + offset] = a0 + a2 + a1 + a3;
                    a[i + offset + 1 * p] = a0 + a2 + (2 * mod2 - (a1 + a3));
                    a[i + offset + 2 * p] = a0 + na2 + a1na3imag;
                    a[i + offset + 3 * p] = a0 + na2 + (mod2 - a1na3imag);
                }
                if (s + 1 != (1 << len))
                    rot *= info.rate3[bsf(~(unsigned int)(s))];
            }
            len += 2;
        }
    }
}

template <class mint, internal::is_static_modint_t<mint>* = nullptr>
void butterfly_inv(std::vector<mint>& a) {
    int n = int(a.size());
    int h = internal::ceil_pow2(n);

    static const fft_info<mint> info;

    int len = h;  // a[i, i+(n>>len), i+2*(n>>len), ..] is transformed
    while (len) {
        if (len == 1) {
            int p = 1 << (h - len);
            mint irot = 1;
            for (int s = 0; s < (1 << (len - 1)); s++) {
                int offset = s << (h - len + 1);
                for (int i = 0; i < p; i++) {
                    auto l = a[i + offset];
                    auto r = a[i + offset + p];
                    a[i + offset] = l + r;
                    a[i + offset + p] =
                        (unsigned long long)(mint::mod() + l.val() - r.val()) *
                        irot.val();
                    ;
                }
                if (s + 1 != (1 << (len - 1)))
                    irot *= info.irate2[bsf(~(unsigned int)(s))];
            }
            len--;
        } else {
            // 4-base
            int p = 1 << (h - len);
            mint irot = 1, iimag = info.iroot[2];
            for (int s = 0; s < (1 << (len - 2)); s++) {
                mint irot2 = irot * irot;
                mint irot3 = irot2 * irot;
                int offset = s << (h - len + 2);
                for (int i = 0; i < p; i++) {
                    auto a0 = 1ULL * a[i + offset + 0 * p].val();
                    auto a1 = 1ULL * a[i + offset + 1 * p].val();
                    auto a2 = 1ULL * a[i + offset + 2 * p].val();
                    auto a3 = 1ULL * a[i + offset + 3 * p].val();

                    auto a2na3iimag =
                        1ULL *
                        mint((mint::mod() + a2 - a3) * iimag.val()).val();

                    a[i + offset] = a0 + a1 + a2 + a3;
                    a[i + offset + 1 * p] =
                        (a0 + (mint::mod() - a1) + a2na3iimag) * irot.val();
                    a[i + offset + 2 * p] =
                        (a0 + a1 + (mint::mod() - a2) + (mint::mod() - a3)) *
                        irot2.val();
                    a[i + offset + 3 * p] =
                        (a0 + (mint::mod() - a1) + (mint::mod() - a2na3iimag)) *
                        irot3.val();
                }
                if (s + 1 != (1 << (len - 2)))
                    irot *= info.irate3[bsf(~(unsigned int)(s))];
            }
            len -= 2;
        }
    }
}

template <class mint, internal::is_static_modint_t<mint>* = nullptr>
std::vector<mint> convolution_naive(const std::vector<mint>& a,
                                    const std::vector<mint>& b) {
    int n = int(a.size()), m = int(b.size());
    std::vector<mint> ans(n + m - 1);
    if (n < m) {
        for (int j = 0; j < m; j++) {
            for (int i = 0; i < n; i++) {
                ans[i + j] += a[i] * b[j];
            }
        }
    } else {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                ans[i + j] += a[i] * b[j];
            }
        }
    }
    return ans;
}

template <class mint, internal::is_static_modint_t<mint>* = nullptr>
std::vector<mint> convolution_fft(std::vector<mint> a, std::vector<mint> b) {
    int n = int(a.size()), m = int(b.size());
    int z = 1 << internal::ceil_pow2(n + m - 1);
    a.resize(z);
    internal::butterfly(a);
    b.resize(z);
    internal::butterfly(b);
    for (int i = 0; i < z; i++) {
        a[i] *= b[i];
    }
    internal::butterfly_inv(a);
    a.resize(n + m - 1);
    mint iz = mint(z).inv();
    for (int i = 0; i < n + m - 1; i++) a[i] *= iz;
    return a;
}

}  // namespace internal

template <class mint, internal::is_static_modint_t<mint>* = nullptr>
std::vector<mint> convolution(std::vector<mint>&& a, std::vector<mint>&& b) {
    int n = int(a.size()), m = int(b.size());
    if (!n || !m) return {};
    if (std::min(n, m) <= 60) return convolution_naive(a, b);
    return internal::convolution_fft(a, b);
}

template <class mint, internal::is_static_modint_t<mint>* = nullptr>
std::vector<mint> convolution(const std::vector<mint>& a,
                              const std::vector<mint>& b) {
    int n = int(a.size()), m = int(b.size());
    if (!n || !m) return {};
    if (std::min(n, m) <= 60) return convolution_naive(a, b);
    return internal::convolution_fft(a, b);
}

template <unsigned int mod = 998244353,
          class T,
          std::enable_if_t<internal::is_integral<T>::value>* = nullptr>
std::vector<T> convolution(const std::vector<T>& a, const std::vector<T>& b) {
    int n = int(a.size()), m = int(b.size());
    if (!n || !m) return {};

    using mint = static_modint<mod>;
    std::vector<mint> a2(n), b2(m);
    for (int i = 0; i < n; i++) {
        a2[i] = mint(a[i]);
    }
    for (int i = 0; i < m; i++) {
        b2[i] = mint(b[i]);
    }
    auto c2 = convolution(move(a2), move(b2));
    std::vector<T> c(n + m - 1);
    for (int i = 0; i < n + m - 1; i++) {
        c[i] = c2[i].val();
    }
    return c;
}

std::vector<long long> convolution_ll(const std::vector<long long>& a,
                                      const std::vector<long long>& b) {
    int n = int(a.size()), m = int(b.size());
    if (!n || !m) return {};

    static constexpr unsigned long long MOD1 = 754974721;  // 2^24
    static constexpr unsigned long long MOD2 = 167772161;  // 2^25
    static constexpr unsigned long long MOD3 = 469762049;  // 2^26
    static constexpr unsigned long long M2M3 = MOD2 * MOD3;
    static constexpr unsigned long long M1M3 = MOD1 * MOD3;
    static constexpr unsigned long long M1M2 = MOD1 * MOD2;
    static constexpr unsigned long long M1M2M3 = MOD1 * MOD2 * MOD3;

    static constexpr unsigned long long i1 =
        internal::inv_gcd(MOD2 * MOD3, MOD1).second;
    static constexpr unsigned long long i2 =
        internal::inv_gcd(MOD1 * MOD3, MOD2).second;
    static constexpr unsigned long long i3 =
        internal::inv_gcd(MOD1 * MOD2, MOD3).second;

    auto c1 = convolution<MOD1>(a, b);
    auto c2 = convolution<MOD2>(a, b);
    auto c3 = convolution<MOD3>(a, b);

    std::vector<long long> c(n + m - 1);
    for (int i = 0; i < n + m - 1; i++) {
        unsigned long long x = 0;
        x += (c1[i] * i1) % MOD1 * M2M3;
        x += (c2[i] * i2) % MOD2 * M1M3;
        x += (c3[i] * i3) % MOD3 * M1M2;
        // B = 2^63, -B <= x, r(real value) < B
        // (x, x - M, x - 2M, or x - 3M) = r (mod 2B)
        // r = c1[i] (mod MOD1)
        // focus on MOD1
        // r = x, x - M', x - 2M', x - 3M' (M' = M % 2^64) (mod 2B)
        // r = x,
        //     x - M' + (0 or 2B),
        //     x - 2M' + (0, 2B or 4B),
        //     x - 3M' + (0, 2B, 4B or 6B) (without mod!)
        // (r - x) = 0, (0)
        //           - M' + (0 or 2B), (1)
        //           -2M' + (0 or 2B or 4B), (2)
        //           -3M' + (0 or 2B or 4B or 6B) (3) (mod MOD1)
        // we checked that
        //   ((1) mod MOD1) mod 5 = 2
        //   ((2) mod MOD1) mod 5 = 3
        //   ((3) mod MOD1) mod 5 = 4
        long long diff =
            c1[i] - internal::safe_mod((long long)(x), (long long)(MOD1));
        if (diff < 0) diff += MOD1;
        static constexpr unsigned long long offset[5] = {
            0, 0, M1M2M3, 2 * M1M2M3, 3 * M1M2M3};
        x -= offset[diff % 5];
        c[i] = x;
    }

    return c;
}

}  // namespace atcoder


#line 1 "/home/yosupo/programs/ac-library/atcoder/math.hpp"



#line 6 "/home/yosupo/programs/ac-library/atcoder/math.hpp"
#include <tuple>
#line 8 "/home/yosupo/programs/ac-library/atcoder/math.hpp"

#line 10 "/home/yosupo/programs/ac-library/atcoder/math.hpp"

namespace atcoder {

long long pow_mod(long long x, long long n, int m) {
    assert(0 <= n && 1 <= m);
    if (m == 1) return 0;
    internal::barrett bt((unsigned int)(m));
    unsigned int r = 1, y = (unsigned int)(internal::safe_mod(x, m));
    while (n) {
        if (n & 1) r = bt.mul(r, y);
        y = bt.mul(y, y);
        n >>= 1;
    }
    return r;
}

long long inv_mod(long long x, long long m) {
    assert(1 <= m);
    auto z = internal::inv_gcd(x, m);
    assert(z.first == 1);
    return z.second;
}

// (rem, mod)
std::pair<long long, long long> crt(const std::vector<long long>& r,
                                    const std::vector<long long>& m) {
    assert(r.size() == m.size());
    int n = int(r.size());
    // Contracts: 0 <= r0 < m0
    long long r0 = 0, m0 = 1;
    for (int i = 0; i < n; i++) {
        assert(1 <= m[i]);
        long long r1 = internal::safe_mod(r[i], m[i]), m1 = m[i];
        if (m0 < m1) {
            std::swap(r0, r1);
            std::swap(m0, m1);
        }
        if (m0 % m1 == 0) {
            if (r0 % m1 != r1) return {0, 0};
            continue;
        }
        // assume: m0 > m1, lcm(m0, m1) >= 2 * max(m0, m1)

        // (r0, m0), (r1, m1) -> (r2, m2 = lcm(m0, m1));
        // r2 % m0 = r0
        // r2 % m1 = r1
        // -> (r0 + x*m0) % m1 = r1
        // -> x*u0*g = r1-r0 (mod u1*g) (u0*g = m0, u1*g = m1)
        // -> x = (r1 - r0) / g * inv(u0) (mod u1)

        // im = inv(u0) (mod u1) (0 <= im < u1)
        long long g, im;
        std::tie(g, im) = internal::inv_gcd(m0, m1);

        long long u1 = (m1 / g);
        // |r1 - r0| < (m0 + m1) <= lcm(m0, m1)
        if ((r1 - r0) % g) return {0, 0};

        // u1 * u1 <= m1 * m1 / g / g <= m0 * m1 / g = lcm(m0, m1)
        long long x = (r1 - r0) / g % u1 * im % u1;

        // |r0| + |m0 * x|
        // < m0 + m0 * (u1 - 1)
        // = m0 + m0 * m1 / g - m0
        // = lcm(m0, m1)
        r0 += x * m0;
        m0 *= u1;  // -> lcm(m0, m1)
        if (r0 < 0) r0 += m0;
    }
    return {r0, m0};
}

long long floor_sum(long long n, long long m, long long a, long long b) {
    assert(0 <= n && n < (1LL << 32));
    assert(1 <= m && m < (1LL << 32));
    unsigned long long ans = 0;
    if (a < 0) {
        unsigned long long a2 = internal::safe_mod(a, m);
        ans -= 1ULL * n * (n - 1) / 2 * ((a2 - a) / m);
        a = a2;
    }
    if (b < 0) {
        unsigned long long b2 = internal::safe_mod(b, m);
        ans -= 1ULL * n * ((b2 - b) / m);
        b = b2;
    }
    return ans + internal::floor_sum_unsigned(n, m, a, b);
}

}  // namespace atcoder


#line 16 "test-oj/a.cpp"

static_assert(__cplusplus >= 201703L, "C++17 or later");

namespace yosupo {

struct llx4 {
    __m256i d;

    llx4() {}
    llx4(long long x) : d(_mm256_set1_epi64x(x)) {}
    llx4(const __m256i& x) : d(x) {}
    llx4(const std::array<long long, 8>& x)
        : d(_mm256_loadu_si256((__m256i*)x.data())) {}

    long long at(int i) const {
        alignas(32) std::array<long long, 4> b;
        _mm256_store_si256((__m256i*)b.data(), d);
        return b[i];
    }
    void set(int i, long long x) {
        alignas(32) std::array<long long, 4> b;
        _mm256_store_si256((__m256i*)b.data(), d);
        b[i] = x;
        d = _mm256_load_si256((__m256i*)b.data());
    }

    llx4& operator+=(const llx4& rhs) {
        d = _mm256_add_epi64(d, rhs.d);
        return *this;
    }
    friend llx4 operator+(const llx4& lhs, const llx4& rhs) {
        return llx4(lhs) += rhs;
    }
    llx4& operator-=(const llx4& rhs) {
        d = _mm256_sub_epi64(d, rhs.d);
        return *this;
    }
    friend llx4 operator-(const llx4& lhs, const llx4& rhs) {
        return llx4(lhs) -= rhs;
    }
};

struct intx8 {
    __m256i d;

    intx8() : d() {}
    intx8(int x) : d(_mm256_set1_epi32(x)) {}
    intx8(const __m256i& x) : d(x) {}
    intx8(const std::array<int, 8>& x)
        : d(_mm256_loadu_si256((__m256i*)x.data())) {}
    intx8(int x0, int x1, int x2, int x3, int x4, int x5, int x6, int x7)
        : d(_mm256_set_epi32(x7, x6, x5, x4, x3, x2, x1, x0)) {}

    std::array<int, 8> to_array() const {
        alignas(32) std::array<int, 8> b;
        _mm256_store_si256((__m256i*)b.data(), d);
        return b;
    }
    int at(int i) const {
        alignas(32) std::array<int, 8> b;
        _mm256_store_si256((__m256i*)b.data(), d);
        return b[i];
    }
    void set(int i, int x) {
        alignas(32) std::array<int, 8> b;
        _mm256_store_si256((__m256i*)b.data(), d);
        b[i] = x;
        d = _mm256_load_si256((__m256i*)b.data());
    }

    intx8& operator+=(const intx8& rhs) {
        d = _mm256_add_epi32(d, rhs.d);
        return *this;
    }
    friend intx8 operator+(const intx8& lhs, const intx8& rhs) {
        return intx8(lhs) += rhs;
    }
    intx8& operator-=(const intx8& rhs) {
        d = _mm256_sub_epi32(d, rhs.d);
        return *this;
    }
    friend intx8 operator-(const intx8& lhs, const intx8& rhs) {
        return intx8(lhs) -= rhs;
    }

    // return (0246, 1357)
    std::pair<llx4, llx4> mul(const intx8 rhs) const {
        __m256i x0246 = _mm256_mul_epi32(d, rhs.d);
        __m256i x1357 = _mm256_mul_epi32(_mm256_shuffle_epi32(d, 0xf5),
                                         _mm256_shuffle_epi32(rhs.d, 0xf5));
        return {x0246, x1357};
    }

    intx8& operator&=(const intx8& rhs) {
        d = _mm256_and_si256(d, rhs.d);
        return *this;
    }
    friend intx8 operator&(const intx8& lhs, const intx8& rhs) {
        return intx8(lhs) &= rhs;
    }

    // (d[i] > rhs[i] ? -1 : 0), -1 means that all bit set
    intx8 operator>(const intx8& rhs) const {
        return _mm256_cmpgt_epi32(d, rhs.d);
    }
    intx8 operator<(const intx8& rhs) const { return rhs > *this; }

    intx8 sign() const { return *this < intx8(_mm256_setzero_si256()); }

    intx8 abs() const { return _mm256_abs_epi32(d); }

    /*    template <int N> modint998244353x8 shuffle() const {
            modint998244353x8 x;
            x.d.d = _mm256_shuffle_epi32(d.d, N);
            return x;
        }
        template <int N> modint998244353x8 shufflex4() const {
            modint998244353x8 x;
            x.d.d = _mm256_permute2x128_si256(d.d, d.d, N);
            return x;
        }*/

    // return (0246, 1357)
    std::pair<llx4, llx4> split() const {
        return {
            llx4(((*this) & intx8(-1, 0, -1, 0, -1, 0, -1, 0)).d),
            llx4(_mm256_srli_epi64(d, 32)),
        };
    }
};

}  // namespace yosupo

namespace yosupo {

namespace internal {

using namespace std;

static constexpr int m_int = 998244353;

// f(x[i]) = (x[i] / (2^32)) (mod m)
// input range: x[i] + (2^32 - 1) * m < 2^63
// output range: (x[i] / 2^32) <= f(x[i]) <= (x[i] / 2^32) + m
template <int m>
intx8 montgomery_reduction(const llx4& x0246, const llx4& x1357) {
    static constexpr int inv_m =
        -(int)atcoder::internal::inv_gcd(m_int, 1LL << 32).second;

    __m256i km0246 =
        _mm256_mul_epu32(_mm256_mul_epu32(x0246.d, _mm256_set1_epi32(inv_m)),
                         _mm256_set1_epi32(m));
    __m256i km1357 =
        _mm256_mul_epu32(_mm256_mul_epu32(x1357.d, _mm256_set1_epi32(inv_m)),
                         _mm256_set1_epi32(m));

    llx4 z0246 = llx4(x0246) + llx4(km0246);
    llx4 z1357 = llx4(x1357) + llx4(km1357);

    return _mm256_blend_epi32(_mm256_shuffle_epi32(z0246.d, 0xf5), z1357.d,
                              0b10101010);
}

template <int val> constexpr __m256i constantx8() {
    return _mm256_set1_epi32(val);
}

static const __m256i m = constantx8<m_int>();  //_mm256_set1_epi32(m_int);
static const __m256i im =
    constantx8<(int)atcoder::internal::inv_gcd(m_int, 1LL << 32).second>();

static const intx8 mod2 = intx8(2 * 998244353);

struct modint998244353x8 {
    using mint = modint998244353;
    static const int B = ((1LL << 32)) % m_int;
    static const int iB = atcoder::internal::inv_gcd(B, m_int).second;

    // 0 <= d && d <= 2 * mod
    intx8 d;

    modint998244353x8() : d(0) {}
    modint998244353x8(const std::array<mint, 8>& _d) {
        d = intx8(_mm256_loadu_si256((__m256i*)_d.data()));
        (*this) *= modint998244353x8(B);
    }

    modint998244353x8(mint x0,
                      mint x1,
                      mint x2,
                      mint x3,
                      mint x4,
                      mint x5,
                      mint x6,
                      mint x7)
        : d(_mm256_set_epi32(x7.val(),
                             x6.val(),
                             x5.val(),
                             x4.val(),
                             x3.val(),
                             x2.val(),
                             x1.val(),
                             x0.val())) {
        (*this) *= modint998244353x8(B);
    }

    modint998244353x8(mint x) : d(int((x * B).val())) {}

    modint998244353x8& operator+=(const modint998244353x8& rhs) {
        d += rhs.d;
        d -= intx8(2 * m_int);
        d += intx8(2 * m_int) & d.sign();
        return *this;
    }
    modint998244353x8& operator-=(const modint998244353x8& rhs) {
        d -= rhs.d;
        d += intx8(2 * m_int) & d.sign();
        return *this;
    }

    modint998244353x8& operator*=(const modint998244353x8& rhs) {
        auto v = d.mul(rhs.d);
        d = montgomery_reduction<mint::mod()>(v.first, v.second);
        return *this;
    }

    template <int N> modint998244353x8 neg() const {
        modint998244353x8 w;
        w.d = (d - intx8(_mm256_blend_epi32(_mm256_setzero_si256(),
                                            _mm256_set1_epi32(2 * m_int), N)))
                  .abs();
        return w;
    }

    friend modint998244353x8 operator+(const modint998244353x8& lhs,
                                       const modint998244353x8& rhs) {
        return modint998244353x8(lhs) += rhs;
    }
    friend modint998244353x8 operator-(const modint998244353x8& lhs,
                                       const modint998244353x8& rhs) {
        return modint998244353x8(lhs) -= rhs;
    }
    friend modint998244353x8 operator*(const modint998244353x8& lhs,
                                       const modint998244353x8& rhs) {
        return modint998244353x8(lhs) *= rhs;
    }

    template <int N> modint998244353x8 shuffle() const {
        modint998244353x8 x;
        x.d.d = _mm256_shuffle_epi32(d.d, N);
        return x;
    }
    template <int N> modint998244353x8 shufflex4() const {
        modint998244353x8 x;
        x.d.d = _mm256_permute2x128_si256(d.d, d.d, N);
        return x;
    }

    std::array<mint, 8> to_array() const {
        auto buf = (*this * modint998244353x8(iB)).d;
        buf -= intx8(m_int) & (buf > intx8(m_int - 1));
        auto v = buf.to_array();
        std::array<mint, 8> x;
        for (int i = 0; i < 8; i++) {
            x[i] = mint::raw(v[i]);
        }
        return x;
    }

    static modint998244353x8 from_raw(const intx8& _d) {
        modint998244353x8 x;
        x.d = _d;
        return x;
    }
};

template <class mint,
          int g = atcoder::internal::primitive_root<mint::mod()>,
          atcoder::internal::is_static_modint_t<mint>* = nullptr>
struct fft_info {
    static constexpr int rank2 = 20;    // bsf_constexpr(mint::mod() - 1);
    std::array<mint, rank2 + 1> root, iroot;   // root[i]^(2^i) == 1, root[i] * iroot[i] == 1

    std::array<mint, std::max(0, rank2 - 1 + 1)> rate2, irate2;
    std::array<modint998244353x8, std::max(0, rank2 - 1 + 1)> rate2x;

    std::array<mint, std::max(0, rank2 - 2 + 1)> rate3, irate3;

    std::array<mint, std::max(0, rank2 - 3 + 1)> rate4, irate4;
    std::array<modint998244353x8, std::max(0, rank2 - 3 + 1)>
        rate4xi, irate4xi;  // rate4xi[i][j] = rate4[i]^j

    fft_info() {
        root[rank2] = mint(g).pow((mint::mod() - 1) >> rank2);
        iroot[rank2] = root[rank2].inv();
        for (int i = rank2 - 1; i >= 0; i--) {
            root[i] = root[i + 1] * root[i + 1];
            iroot[i] = iroot[i + 1] * iroot[i + 1];
        }

        {
            mint prod = 1, iprod = 1;
            for (int i = 0; i <= rank2 - 2; i++) {
                rate2[i] = root[i + 2] * prod;
                irate2[i] = iroot[i + 2] * iprod;
                prod *= iroot[i + 2];
                iprod *= root[i + 2];
            }
            for (int i = 0; i <= rank2 - 2; i++) {
                rate2x[i] = modint998244353x8(rate2[i]);
            }
        }
        {
            mint prod = 1, iprod = 1;
            for (int i = 0; i <= rank2 - 3; i++) {
                rate3[i] = root[i + 3] * prod;
                irate3[i] = iroot[i + 3] * iprod;
                prod *= iroot[i + 3];
                iprod *= root[i + 3];
            }
        }
        {
            mint prod = 1, iprod = 1;
            for (int i = 0; i <= rank2 - 4; i++) {
                rate4[i] = root[i + 4] * prod;
                irate4[i] = iroot[i + 4] * iprod;
                prod *= iroot[i + 4];
                iprod *= root[i + 4];
                std::array<mint, 8> buf, ibuf;
                for (int j = 0; j < 8; j++) {
                    buf[j] = rate4[i].pow(j);
                    ibuf[j] = irate4[i].pow(j);
                }
                rate4xi[i] = buf;
                irate4xi[i] = ibuf;
            }
        }
    }
};
using namespace std;

void butterfly998244353(int n, std::vector<modint998244353x8>& _a) {
    using mint = modint998244353;

    int h = internal::ceil_pow2(n);

    static const fft_info<mint> info;
    const mint imag = info.root[2];

    assert(n >= 8 && n % 8 == 0);
    int n8 = n / 8;

    int len = 0;  // a[i, i+(n>>len), i+2*(n>>len), ..] is transformed


    while (len + 5 <= h) {
        // 4-base
        int p = 1 << (h - len - 5);
        modint998244353x8 rotx(1);
        auto imagx = modint998244353x8(imag);
        imagx.d -= intx8(998244353) & (imagx.d > intx8(998244352));
        for (int s = 0; s < (1 << len); s++) {
            auto rot2x = rotx * rotx;
            auto rot3x = rot2x * rotx;
            int offset = s << (h - len - 3);
            for (int i = 0; i < p; i++) {
                auto a0 = _a[i + offset];
                auto a1 = _a[i + offset + p] * rotx;
                auto a2 = _a[i + offset + 2 * p] * rot2x;
                auto a3 = _a[i + offset + 3 * p] * rot3x;
                auto a1na3imag = (a1 - a3) * imagx;
                auto a0a2 = a0 + a2;
                auto a1a3 = a1 + a3;
                auto a0na2 = a0 - a2;
                _a[i + offset] = a0a2 + a1a3;
                _a[i + offset + 1 * p] = a0a2 - a1a3;
                _a[i + offset + 2 * p] = a0na2 + a1na3imag;
                _a[i + offset + 3 * p] = a0na2 - a1na3imag;
            }
            rotx *= modint998244353x8(info.rate3[bsf(~(unsigned int)(s))]);
        }
        len += 2;
    }

    if (len + 4 == h) {
        int p = 1 << (h - len - 4);
        modint998244353x8 rotx = modint998244353(mint(1));
        for (int s = 0; s < (1 << len); s++) {
            int offset = s << (h - len - 3);
            for (int i = 0; i < p; i++) {
                auto l = _a[i + offset];
                auto r = _a[i + offset + p] * rotx;
                _a[i + offset] = l + r;
                _a[i + offset + p] = l - r;
            }
            rotx *= info.rate2x[bsf(~(unsigned int)(s))];
        }
        len++;
    }

    {
        // 8-base
        mint e8 = info.root[3];
        const modint998244353x8 step1 = modint998244353x8(
            1, 1, 1, 1, 1, e8, e8 * e8, e8 * e8 * e8
        );
        const modint998244353x8 step2 = modint998244353x8(
            1, 1, 1, imag, 1, 1, 1, imag
        );
        assert(len + 3 == h);
        auto rotxi = modint998244353x8(1);
        for (int s = 0; s < n8; s++) {
            auto v = _a[s] * rotxi;
            v = (v.neg<0b11110000>() + v.shufflex4<0b01>()) * step1;
            v = (v.neg<0b11001100>() + v.shuffle<0b01001110>()) * step2;
            v = (v.neg<0b10101010>() + v.shuffle<0b10110001>());
            _a[s] = v;
            rotxi *= info.rate4xi[bsf(~(unsigned int)(s))];
        }
        len += 3;
    }
}

void butterfly998244353_inv(int n, std::vector<modint998244353x8>& _a) {
    using mint = modint998244353;

    int h = internal::ceil_pow2(n);

    static const fft_info<mint> info;

    assert(n >= 8 && n % 8 == 0);
    const mint iimag = info.iroot[2];
    const modint998244353x8 iimagx = iimag;

    int n8 = n / 8;

    int len = h;  // a[i, i+(n>>len), i+2*(n>>len), ..] is transformed

    {
        // 8-base
        mint ie8 = info.iroot[3];
        const modint998244353x8 istep1 =
            modint998244353x8(1, 1, 1, 1, 1, ie8, ie8 * ie8, ie8 * ie8 * ie8);
        const modint998244353x8 istep2 =
            modint998244353x8(1, 1, 1, iimag, 1, 1, 1, iimag);
        auto irotxi = modint998244353x8(1);
        for (int s = 0; s < n8; s++) {
            auto v = _a[s];
            v = (v.neg<0b10101010>() + v.shuffle<0b10110001>()) * istep2;
            v = (v.neg<0b11001100>() + v.shuffle<0b01001110>()) * istep1;
            v = (v.neg<0b11110000>() + v.shufflex4<0b01>()) * irotxi;
            _a[s] = v;
            irotxi *= info.irate4xi[bsf(~(unsigned int)(s))];
        }
        len -= 3;
    }

    while (len >= 2) {
        int p = 1 << (h - len - 3);
        auto irotx = modint998244353x8(1);
        for (int s = 0; s < (1 << (len - 2)); s++) {
            auto irot2x = irotx * irotx;
            auto irot3x = irot2x * irotx;
            int offset = s << (h - len - 1);
            for (int i = 0; i < p; i++) {
                auto a0 = _a[i + offset + 0 * p];
                auto a1 = _a[i + offset + 1 * p];
                auto a2 = _a[i + offset + 2 * p];
                auto a3 = _a[i + offset + 3 * p];
                auto a0a1 = a0 + a1;
                auto a0na1 = a0 - a1;
                auto a2a3 = a2 + a3;
                auto a2na3iimag = (a2 - a3) * iimagx;

                _a[i + offset + 0 * p] = a0a1 + a2a3;
                _a[i + offset + 1 * p] = (a0na1 + a2na3iimag) * irotx;
                _a[i + offset + 2 * p] = (a0a1 - a2a3) * irot2x;
                _a[i + offset + 3 * p] = (a0na1 - a2na3iimag) * irot3x;
            }
            irotx *= info.irate3[bsf(~(unsigned int)(s))];
        }
        len -= 2;
    }

    if (len == 1) {
        int p = 1 << (h - 4);
        for (int i = 0; i < p; i++) {
            auto l = _a[i];
            auto r = _a[i + p];
            _a[i] = l + r;
            _a[i + p] = l - r;
        }
        len--;
    }
}

}  // namespace internal

using namespace yosupo::internal;
std::vector<modint998244353x8> convolution998244353(
    int n,
    std::vector<modint998244353x8> a,
    int m,
    std::vector<modint998244353x8> b) {
    if (!n || !m) return {};
    int z = 1 << internal::ceil_pow2(n + m - 1);
    a.resize(z / 8);
    internal::butterfly998244353(z, a);
    b.resize(z / 8);
    internal::butterfly998244353(z, b);
    for (int i = 0; i < z / 8; i++) {
        a[i] *= b[i];
    }
    internal::butterfly998244353_inv(z, a);
    modint998244353x8 iz = modint998244353(z).inv();
    for (int i = 0; i < z / 8; i++) a[i] *= iz;
    return a;
}

}  // namespace yosupo

using namespace std;
using namespace yosupo;

yosupo::Scanner sc(stdin);
yosupo::Printer pr(stdout);

using mint = modint998244353;

namespace the_fastest_convolution998244353 {
    std::vector<yosupo::internal::modint998244353x8> tox8(const std::vector<mint> &a) {
        int n = (int(a.size()) + 7) / 8;
        std::vector<yosupo::internal::modint998244353x8> pol;
        pol.reserve(2 * n);
        pol.resize(n);
        for (int i = 0; i < n; i++) {
            std::array<mint, 8> arr;
            arr.fill(mint::raw(0));
            for (int j = 0; j < 8 && i * 8 + j < int(a.size()); j++)
                arr[j] = mint::raw(a[i * 8 + j].val());

            pol[i] = arr;
        }
        return pol;
    };

    std::vector<mint> multiply(const std::vector<mint> &a, const std::vector<mint> &b) {
        int n = a.size(), m = b.size();
        if (!n || !m)
            return {};

        if (std::min(n, m) <= 20) {
            std::vector<mint> c(n + m - 1);
            for (int i = 0; i < n; i++)
                for (int j = 0; j < m; j++)
                    c[i + j] += a[i] * b[j];

            return c;
        }

        auto c = yosupo::convolution998244353(n, tox8(a), m, tox8(b));
        std::vector<mint> res;
        res.resize(n + m - 1);
        for (int i = 0; i < int(c.size()); i++) {
            auto cc = c[i].to_array();
            for (int j = 0; j < 8 && i * 8 + j < int(res.size()); j++)
                res[i * 8 + j] = cc[j];
        }
        return res;
    };
} // namespace the_fastest_convolution998244353
