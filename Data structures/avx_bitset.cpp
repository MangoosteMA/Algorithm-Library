#include <iostream>
#include <cstring>
#include <immintrin.h>

#pragma GCC target("avx,avx2,popcnt")
#pragma GCC optimize("O3,unroll-loops")

class avx_bitset final {
private:
    int n;
    uint64_t* values;

    int get_size() const {
        return (n + 255) >> 8 << 2;
    }

    void extand(int new_n) {
        int prev_size = get_size();
        n = new_n;
        int new_size = get_size();

        if (new_size > prev_size) {
            uint64_t* new_values = new uint64_t[new_size]{};
            memcpy(new_values, values, sizeof(uint64_t) * prev_size);
            delete[] values;
            values = new_values;
        }
    }

    void remove_trash() {
        int sz = get_size();
        if ((n >> 6) < sz) {
            for (int i = (n >> 6) + 1; i < sz; i++)
                values[i] = 0;

            if (n & 63) {
                values[n >> 6] <<= 64 - (n & 63);
                values[n >> 6] >>= 64 - (n & 63);
            } else {
                values[n >> 6] = 0;
            }
        }
    }

public:
    avx_bitset(int n = 0, unsigned long long value = 0) : n(n) {
        values = new uint64_t[get_size()]{};
        for (int i = 0; i < n && i < 64; i++)
            if (value >> i & 1)
                set_bit(i, true);

    }

    avx_bitset(const avx_bitset &bs) : n(bs.n) {
        int sz = get_size();
        values = new uint64_t[sz];
        memcpy(values, bs.values, sizeof(uint64_t) * sz);
    }

    int size() const {
        return n;
    }

    void set_bit(int pos, bool bit) {
        int id = pos >> 6;
        pos &= 63;
        if (bit)
            values[id] |= uint64_t(1) << pos;
        else
            values[id] &= ~(uint64_t(1) << pos);
    }

    bool get_bit(int pos) const {
        int id = pos >> 6;
        pos &= 63;
        return values[id] >> pos & 1;
    }

    avx_bitset& operator=(const avx_bitset &bs) {
        delete[] values;
        n = bs.n;
        int sz = get_size();
        values = new uint64_t[sz];
        memcpy(values, bs.values, sizeof(uint64_t) * sz);
        return *this;
    }

    avx_bitset& operator|=(const avx_bitset &bs) {
        if (size() < bs.size())
            extand(bs.size());

        for (int i = 0; (i << 6) < bs.size(); i += 4)
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(values + i),
                _mm256_or_si256(_mm256_lddqu_si256(reinterpret_cast<const __m256i*>(values + i)),
                    _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(bs.data() + i))));

        return *this;
    }

    avx_bitset& operator&=(const avx_bitset &bs) {
        if (size() < bs.size())
            extand(bs.size());

        for (int i = 0; (i << 6) < bs.size(); i += 4)
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(values + i),
                _mm256_and_si256(_mm256_lddqu_si256(reinterpret_cast<const __m256i*>(values + i)),
                    _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(bs.data() + i))));

        return *this;
    }

    avx_bitset& operator^=(const avx_bitset &bs) {
        if (size() < bs.size())
            extand(bs.size());

        for (int i = 0; (i << 6) < bs.size(); i += 4)
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(values + i),
                _mm256_xor_si256(_mm256_lddqu_si256(reinterpret_cast<const __m256i*>(values + i)),
                    _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(bs.data() + i))));

        return *this;
    }

    avx_bitset& operator>>=(int shift) {
        if (shift == 0 || n == 0)
            return *this;

        if (shift < 0)
            return *this <<= -shift;

        int sz = get_size();
        if (!(shift & 255)) {
            shift >>= 6;
            if (shift > sz)
                shift = sz;

            int id = sz - shift;
            for (int i = 0; i < id; i += 4)
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(values + i),
                    _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(values + i + shift)));

            memset(values + id, 0, sizeof(uint64_t) * shift);
        } else if (!(shift & 63)) {
            shift >>= 6;
            if (shift > sz)
                shift = sz;

            int id = sz - shift, pos = 0;
            for (; pos + 4 < id; pos += 4)
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(values + pos),
                    _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(values + pos + shift)));

            for (; pos < id; pos++)
                values[pos] = values[pos + shift];

            memset(values + id, 0, sizeof(uint64_t) * shift);
        } else {
            int d = shift >> 6, cnt1 = shift & 63, cnt2 = 64 - cnt1;
            if (d >= sz) {
                memset(values, 0, sizeof(uint64_t) * sz);
                return *this;
            }

            int id = sz - 1 - d, pos = 0;
            __m256i to_shift_right = _mm256_set1_epi64x(cnt1), to_shift_left = _mm256_set1_epi64x(cnt2);
            for (; pos + 4 < id; pos += 4)
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(values + pos),
                    _mm256_or_si256(_mm256_srlv_epi64(
                        _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(values + pos + d)),
                        to_shift_right),
                        _mm256_sllv_epi64(_mm256_lddqu_si256(reinterpret_cast<const __m256i*>(values + pos + d + 1)),
                            to_shift_left)));

            for (; pos < id; pos++)
                values[pos] = (values[pos + d] >> cnt1) | (values[pos + d + 1] << cnt2);

            values[id] = values[sz - 1] >> cnt1;
            memset(values + id + 1, 0, sizeof(uint64_t) * d);
        }
        return *this;
    }

    avx_bitset& operator<<=(int shift) {
        if (shift == 0 || n == 0)
            return *this;

        if (shift < 0)
            return *this >>= -shift;

        int sz = get_size();
        if (!(shift & 255)) {
            shift >>= 6;
            if (shift > sz)
                shift = sz;

            for (int i = sz - 4; i >= shift; i -= 4)
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(values + i),
                    _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(values + i - shift)));

            memset(values, 0, sizeof(uint64_t) * shift);
        } else if (!(shift & 63)) {
            shift >>= 6;
            if (shift > sz)
                shift = sz;

            int pos = sz - 1;
            for (; pos >= shift + 3; pos -= 4)
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(values + pos - 3),
                    _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(values + pos - 3 - shift)));

            for (; pos >= shift; pos--)
                values[pos] = values[pos - shift];

            memset(values, 0, sizeof(uint64_t) * shift);
        } else {
            int d = shift >> 6, cnt1 = shift & 63, cnt2 = 64 - cnt1;
            if (d >= sz) {
                memset(values, 0, sizeof(uint64_t) * sz);
                return *this;
            }

            int pos = sz - 1;
            __m256i to_shift_left = _mm256_set1_epi64x(cnt1), to_shift_right = _mm256_set1_epi64x(cnt2);
            for (; pos > d + 3; pos -= 4)
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(values + pos - 3),
                    _mm256_or_si256(_mm256_sllv_epi64(
                        _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(values + pos - 3 - d)),
                        to_shift_left),
                        _mm256_srlv_epi64(_mm256_lddqu_si256(reinterpret_cast<const __m256i*>(values + pos - 4 - d)),
                            to_shift_right)));

            for (; pos > d; pos--)
                values[pos] = (values[pos - d] << cnt1) | (values[pos - d - 1] >> cnt2);

            values[d] = values[0] << cnt1;
            memset(values, 0, sizeof(uint64_t) * d);
        }

        remove_trash();
        return *this;
    }

    friend avx_bitset operator|(const avx_bitset &a, const avx_bitset &b) {
        return avx_bitset(a) |= b;
    }

    friend avx_bitset operator&(const avx_bitset &a, const avx_bitset &b) {
        return avx_bitset(a) &= b;
    }

    friend avx_bitset operator^(const avx_bitset &a, const avx_bitset &b) {
        return avx_bitset(a) ^= b;
    }

    friend avx_bitset operator>>(const avx_bitset &a, int shift) {
        return avx_bitset(a) >>= shift;
    }

    friend avx_bitset operator<<(const avx_bitset &a, int shift) {
        return avx_bitset(a) <<= shift;
    }

    bool operator==(const avx_bitset &bs) const {
        if (n != bs.size())
            return false;

        int sz = get_size();
        for (int pos = 0; pos < sz; pos += 4)
            if (!_mm256_testz_si256(_mm256_set1_epi32(-1),
                    _mm256_xor_si256(_mm256_lddqu_si256(reinterpret_cast<const __m256i*>(values + pos)),
                        _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(bs.data() + pos)))))
                return false;

        return true;
    }

    bool operator!=(const avx_bitset &bs) const {
        return !(*this == bs);
    }

    int find_next_one(int pos) const {
        if (pos >= n)
            return n;

        int sz = get_size(), id = pos >> 6;
        for (uint64_t value; (id & 3) || id == (pos >> 6); id++)
            if (value = id == (pos >> 6) ? (values[id] >> (pos & 63) << (pos & 63)) : values[id])
                return (id << 6) + __builtin_ctzll(value);

        for (; id < sz; id += 4)
            if (!_mm256_testz_si256(_mm256_set1_epi32(-1),
                    _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(values + id))))
                break;

        if (id == sz)
            return n;

        for (;; id++)
            if (values[id])
                return (id << 6) + __builtin_ctzll(values[id]);
    }

    int find_next_zero(int pos) const {
        if (pos >= n)
            return n;

        int sz = get_size(), id = pos >> 6;
        for (uint64_t value; (id & 3) || id == (pos >> 6); id++)
            if (value = ~(id == (pos >> 6)
                ? (values[id] >> (pos & 63) << (pos & 63) | (uint64_t(1) << (pos & 63)) - 1) : values[id]))
                return (id << 6) + __builtin_ctzll(value);

        for (; id < sz; id += 4)
            if (!_mm256_testz_si256(_mm256_set1_epi32(-1),
                    _mm256_xor_si256(_mm256_lddqu_si256(reinterpret_cast<const __m256i*>(values + id)),
                        _mm256_set1_epi32(-1))))
                break;

        if (id == sz)
            return n;

        for (;; id++)
            if (~values[id])
                return (id << 6) + __builtin_ctzll(~values[id]);
    }

    int find_prev_one(int pos) const {
        if (pos < 0)
            return -1;

        int id = pos >> 6;
        for (uint64_t value; id >= 0 && ((id & 3) != 3 || id == (pos >> 6)); id--)
            if (value = id == (pos >> 6) ? (values[id] << (63 - (pos & 63)) >> (63 - (pos & 63))) : values[id])
                return (id << 6) + 63 - __builtin_clzll(value);

        for (; id >= 0; id -= 4)
            if (!_mm256_testz_si256(_mm256_set1_epi32(-1),
                    _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(values + id - 3))))
                break;

        if (id == -1)
            return -1;

        for (;; id--)
            if (values[id])
                return (id << 6) + 63 - __builtin_clzll(values[id]);
    }

    int find_prev_zero(int pos) const {
        if (pos < 0)
            return -1;

        int id = pos >> 6;
        for (uint64_t value; id >= 0 && ((id & 3) != 3 || id == (pos >> 6)); id--)
            if (value = ~(id == (pos >> 6)
                ? ((values[id] << (63 - (pos & 63)) >> (63 - (pos & 63))) |
                    ((pos & 63) == 63 ? 0 : (uint64_t(-1) - (uint64_t(1) << ((pos & 63) + 1)) + 1)))
                        : values[id]))
                return (id << 6) + 63 - __builtin_clzll(value);

        for (; id >= 0; id -= 4)
            if (!_mm256_testz_si256(_mm256_set1_epi32(-1),
                    _mm256_xor_si256(_mm256_lddqu_si256(reinterpret_cast<const __m256i*>(values + id - 3)),
                        _mm256_set1_epi32(-1))))
                break;

        if (id == -1)
            return -1;

        for (;; id--)
            if (~values[id])
                return (id << 6) + 63 - __builtin_clzll(~values[id]);
    }

    bool any() const {
        int sz = get_size();
        for (int id = 0; id < sz; id += 4)
            if (!_mm256_testz_si256(_mm256_set1_epi32(-1),
                    _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(values + id))))
                return true;

        return false;
    }

    void flip() {
        int sz = get_size();
        for (int id = 0; id < sz; id += 4)
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(values + id),
                _mm256_xor_si256(_mm256_lddqu_si256(reinterpret_cast<const __m256i*>(values + id)),
                    _mm256_set1_epi32(-1)));

        remove_trash();
    }

    int popcount() const {
        int sz = get_size(), tot = 0;
        for (int i = 0; i < sz; i++)
            tot += __builtin_popcountll(values[i]);

        return tot;
    }

    void set() {
        memset(values, -1, sizeof(uint64_t) * get_size());
        remove_trash();
    }

    void reset() {
        memset(values, 0, sizeof(uint64_t) * get_size());
    }

    void set_segment(int l, int r) {
        while (l < r && (l & 255))
            set_bit(l++, true);

        while (r > l && (r & 255))
            set_bit(--r, true);

        l >>= 6;
        r >>= 6;
        for (int id = l; id < r; id += 4)
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(values + id),
                _mm256_set1_epi64x(-1));
    }

    void reset_segment(int l, int r) {
        while (l < r && (l & 255))
            set_bit(l++, false);

        while (r > l && (r & 255))
            set_bit(--r, false);

        l >>= 6;
        r >>= 6;
        for (int id = l; id < r; id += 4)
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(values + id),
                _mm256_setzero_si256());
    }

    explicit operator std::string() const {
        std::string res(n, '0');
        for (int i = 0; i < n; i++)
            res[i] = '0' + get_bit(i);

        return res;
    }

    explicit operator unsigned long long() const {
        unsigned long long res = 0;
        for (int i = n - 1; i >= 0; i--)
            res = (res << 1) ^ get_bit(i);

        return res;
    }

    friend std::ostream& operator<<(std::ostream &out, const avx_bitset &bs) {
        out << std::string(bs);
        return out;
    }

    uint64_t* data() const {
        return values;
    }

    ~avx_bitset() {
        delete[] values;
    }
};

// TODO: stress set_segment, reset_segment, find_next_zero, find_prev_zero
// stress test
#include <vector>
#include <random>
#include <cassert>

int main() {
    using namespace std;
    ios::sync_with_stdio(false), cin.tie(nullptr);

    mt19937 rng(228228);
    const int MAX_SIZE = 100000;

    for (int test_num = 0;; test_num++) {
        cout << "testing on test " << test_num << '\r' << flush;

        const int n = rng() % MAX_SIZE + 1;
        vector<int> cur_vector(n);
        for (int i = 0; i < n; i++)
            cur_vector[i] = rng() % 2;

        avx_bitset bs(n);
        for (int i = 0; i < n; i++)
            bs.set_bit(i, cur_vector[i]);

        { // testing set
            auto clone = bs;
            clone.set();

            bool correct = true;
            for (int i = 0; i < n; i++)
                correct &= clone.get_bit(i) == 1;

            if (!correct) {
                cout << "set is wrong\n";
                return 0;
            }

            assert(clone.popcount() == n);
        }

        { // testing reset
            auto clone = bs;
            clone.reset();

            bool correct = true;
            for (int i = 0; i < n; i++)
                correct &= clone.get_bit(i) == 0;

            if (!correct) {
                cout << "reset is wrong\n";
                return 0;
            }

            assert(clone.popcount() == 0);
        }

        { // testing left shift
            int to_shift = rng() % MAX_SIZE;
            vector<int> res(n);
            for (int i = to_shift; i < n; i++)
                res[i] = cur_vector[i - to_shift];

            auto clone = bs;
            clone <<= to_shift;

            bool correct = true;
            for (int i = 0; i < n; i++)
                correct &= clone.get_bit(i) == res[i];

            if (!correct) {
                cout << "left shift is wrong\n";
                for (auto x : cur_vector)
                    cout << x;

                cout << endl;
                cout << "shift = " << to_shift << endl;
                cout << "correct: ";
                for (auto x : res)
                    cout << x;

                cout << endl;
                cout << "carrect: ";
                for (int i = 0; i < n; i++)
                    cout << clone.get_bit(i);

                cout << endl;
                return 0;
            }

            if (n % 256 != 0)
                assert(clone.find_next_one(n) == n);
                // assert(!clone.get_bit(n));
        }

        { // testing right shift
            int to_shift = rng() % MAX_SIZE;
            vector<int> res(n);
            for (int i = 0; i + to_shift < n; i++)
                res[i] = cur_vector[i + to_shift];

            auto clone = bs;
            clone >>= to_shift;

            bool correct = true;
            for (int i = 0; i < n; i++)
                correct &= clone.get_bit(i) == res[i];

            if (!correct) {
                cout << "right shift is wrong\n";
                for (auto x : cur_vector)
                    cout << x;

                cout << endl;
                cout << "shift = " << to_shift << endl;
                cout << "correct: ";
                for (auto x : res)
                    cout << x;

                cout << endl;
                cout << "carrect: ";
                for (int i = 0; i < n; i++)
                    cout << clone.get_bit(i);

                cout << endl;
                return 0;
            }
        }

        { // testing xor
            vector<int> vector2(n);
            for (int i = 0; i < n; i++)
                vector2[i] = rng() % 2;

            vector<int> res(n);
            for (int i = 0; i < n; i++)
                res[i] = (cur_vector[i] ^ vector2[i]);

            avx_bitset bs2(n);
            for (int i = 0; i < n; i++)
                bs2.set_bit(i, vector2[i]);

            auto clone = bs ^ bs2;
            bool correct = true;
            for (int i = 0; i < n; i++)
                correct &= clone.get_bit(i) == res[i];

            if (!correct) {
                cout << "xor is wrong\n";
                for (auto x : cur_vector)
                    cout << x;

                cout << endl;
                for (auto x : vector2)
                    cout << x;

                cout << endl;
                cout << "correct: ";
                for (auto x : res)
                    cout << x;

                cout << endl;
                cout << "carrect: ";
                for (int i = 0; i < n; i++)
                    cout << clone.get_bit(i);

                cout << endl;
                return 0;
            }
        }

        { // testing and
            vector<int> vector2(n);
            for (int i = 0; i < n; i++)
                vector2[i] = rng() % 2;

            vector<int> res(n);
            for (int i = 0; i < n; i++)
                res[i] = (cur_vector[i] & vector2[i]);

            avx_bitset bs2(n);
            for (int i = 0; i < n; i++)
                bs2.set_bit(i, vector2[i]);

            auto clone = bs & bs2;
            bool correct = true;
            for (int i = 0; i < n; i++)
                correct &= clone.get_bit(i) == res[i];

            if (!correct) {
                cout << "and is wrong\n";
                for (auto x : cur_vector)
                    cout << x;

                cout << endl;
                for (auto x : vector2)
                    cout << x;

                cout << endl;
                cout << "correct: ";
                for (auto x : res)
                    cout << x;

                cout << endl;
                cout << "carrect: ";
                for (int i = 0; i < n; i++)
                    cout << clone.get_bit(i);

                cout << endl;
                return 0;
            }
        }

        { // testing or
            vector<int> vector2(n);
            for (int i = 0; i < n; i++)
                vector2[i] = rng() % 2;

            vector<int> res(n);
            for (int i = 0; i < n; i++)
                res[i] = (cur_vector[i] | vector2[i]);

            avx_bitset bs2(n);
            for (int i = 0; i < n; i++)
                bs2.set_bit(i, vector2[i]);

            auto clone = bs | bs2;
            bool correct = true;
            for (int i = 0; i < n; i++)
                correct &= clone.get_bit(i) == res[i];

            if (!correct) {
                cout << "or is wrong\n";
                for (auto x : cur_vector)
                    cout << x;

                cout << endl;
                for (auto x : vector2)
                    cout << x;

                cout << endl;
                cout << "correct: ";
                for (auto x : res)
                    cout << x;

                cout << endl;
                cout << "carrect: ";
                for (int i = 0; i < n; i++)
                    cout << clone.get_bit(i);

                cout << endl;
                return 0;
            }
        }

        { // testing find_next_one
            int pos = rng() % n;

            int correct = pos;
            while (correct < n && cur_vector[correct] == 0)
                correct++;

            int where = bs.find_next_one(pos);
            if (where != correct) {
                 cout << "find_next_one is wrong\n";
                for (auto x : cur_vector)
                    cout << x;

                cout << endl;
                cout << "pos = " << pos << endl;
                cout << "correct: " << correct << endl;
                cout << "carrect: " << where << endl;
                return 0;
            }
        }

        { // testing find_prev_one
            int pos = rng() % n;

            int correct = pos;
            while (correct >= 0 && cur_vector[correct] == 0)
                correct--;

            int where = bs.find_prev_one(pos);
            if (where != correct) {
                 cout << "find_prev_one is wrong\n";
                for (auto x : cur_vector)
                    cout << x;

                cout << endl;
                cout << "pos = " << pos << endl;
                cout << "correct: " << correct << endl;
                cout << "carrect: " << where << endl;
                return 0;
            }
        }

        { // testing any
            auto vec = cur_vector;
            if (rng() % 2 == 0)
                fill(vec.begin(), vec.end(), 0);

            avx_bitset clone(n);
            for (int i = 0; i < n; i++)
                clone.set_bit(i, vec[i]);

            bool any = find(vec.begin(), vec.end(), 1) != vec.end();
            if (clone.any() != any) {
                cout << "any is wrong\n";
                for (auto x : vec)
                    cout << x;

                cout << endl;
                return 0;
            }
        }

        { // testing flip
            auto clone = bs;
            clone.flip();

            bool correct = true;
            for (int i = 0; i < n; i++)
                correct &= clone.get_bit(i) != cur_vector[i];

            if (!correct) {
                cout << "flip is wrong\n";
                for (auto x : cur_vector)
                    cout << x;

                cout << endl;
                return 0;
            }
        }

        { // testing popcount
            int cnt = count(cur_vector.begin(), cur_vector.end(), 1);
            if (cnt != bs.popcount()) {
                cout << "popcount is wrong\n";
                for (auto x : cur_vector)
                    cout << x;

                cout << endl;
                cout << "cnt = " << cnt << endl;
                cout << "popcount = " << bs.popcount() << endl;
                return 0;
            }
        }

        { // testing ==
            vector<int> vector2(n);
            for (auto &x : vector2)
                x = rng() % 2;

            if (rng() % 2)
                vector2 = cur_vector;

            avx_bitset bs2(n);
            for (int i = 0; i < n; i++)
                bs2.set_bit(i, vector2[i]);

            if ((bs == bs2) != (cur_vector == vector2)) {
                cout << "== is wrong\n";
                return 0;
            }
        }

        { // testing !=
            vector<int> vector2(n);
            for (auto &x : vector2)
                x = rng() % 2;

            if (rng() % 2)
                vector2 = cur_vector;

            avx_bitset bs2(n);
            for (int i = 0; i < n; i++)
                bs2.set_bit(i, vector2[i]);

            if ((bs != bs2) != (cur_vector != vector2)) {
                cout << "!= is wrong\n";
                return 0;
            }
        }
    }
}
