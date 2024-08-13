/*
 ! WARNING: hash works only for 32-bit types. If 64-bit is required, reduce it to two 32-bits values.
 ! WARNING: addition and subtraction of hashes work in O(64).
 * DIMS is the number of bases for hash to use.
 * Bases are generated automatically, there's no need to run any init function.
 * Addition:
     H([a_1,..., a_n]) + H([b_1,..., b_m]) =  H([a_1,..., a_n, b_1,..., b])
 * Subtraction:
     H([b_1,..., b_m, a_1,..., a_n]) - H([b_1,..., b_m]) = H([a_1,..., a_n])
 * Merge:
     Merge(H([a_1,..., a_n]), H([b_1,..., b_n])) = H([a_1 ^ b_1,..., a_n ^ b_n])
 */

template<int DIMS>
class xor_friendly_hash {
public:
    static_assert(DIMS > 0 && "DIMS must be positive");
    using T = uint32_t;
    using Tl = uint64_t;

    inline static constexpr int LG = 31;
    inline static constexpr Tl BASE_MASK = (static_cast<Tl>(1) << (LG + 1)) + 141;

private:
    inline static bool initialized = false;
    inline static T base[DIMS] = {};
    inline static std::vector<T> powers[DIMS];

    static void initialize() {
        initialized = true;
        std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        for (int d = 0; d < DIMS; d++) {
            base[d] = rng();
            powers[d].push_back(1);
        }
    }

    static T multiply(T x, T y) {
        if (x == 0 || y == 0) {
            return 0;
        }
        Tl product = 0;
        for (int i = 0; i <= LG; i++) {
            if (x >> i & 1) {
                product ^= (static_cast<Tl>(y) << i);
            }
        }
        for (int i = (LG << 1); i > LG; i--) {
            if (product >> i & 1) {
                product ^= (BASE_MASK << (i - LG));
            }
        }
        return static_cast<T>(product);
    }

    static void resize(int n) {
        if (static_cast<int>(powers[0].size()) >= n) {
            return;
        }
        for (int d = 0; d < DIMS; d++) {
            for (int p = static_cast<int>(powers[d].size()); p < n; p++) {
                powers[d].push_back(multiply(powers[d][p - 1], base[d]));
            }
        }
    }

    int length;
    std::array<T, DIMS> data;

public:
    xor_friendly_hash() : length(0) {
        data.fill(0);
        if (!initialized) {
            initialize();
        }
    }

    xor_friendly_hash(T value, int length = 1) : length(length) {
        data.fill(0);
        if (!initialized) {
            initialize();
        }
        resize(length);
        for (int p = 0; p < length; p++) {
            for (int d = 0; d < DIMS; d++) {
                data[d] ^= multiply(value, powers[d][p]);
            }
        }
    }

    xor_friendly_hash& operator+=(const xor_friendly_hash &x) {
        resize(x.length + 1);
        length += x.length;
        for (int d = 0; d < DIMS; d++) {
            data[d] = (multiply(data[d], powers[d][x.length]) ^ x.data[d]);
        }
        return *this;
    }

    xor_friendly_hash& operator-=(const xor_friendly_hash &x) {
        length -= x.length;
        resize(length + 1);
        for (int d = 0; d < DIMS; d++) {
            data[d] = (multiply(x.data[d], powers[d][length]) ^ data[d]);
        }
        return *this;
    }

    friend xor_friendly_hash operator+(const xor_friendly_hash &x, const xor_friendly_hash &y) {
        return xor_friendly_hash(x) += y;
    }

    friend xor_friendly_hash operator-(const xor_friendly_hash &x, const xor_friendly_hash &y) {
        return xor_friendly_hash(x) -= y;
    }

    bool operator==(const xor_friendly_hash &x) const = default;

    bool operator<(const xor_friendly_hash &x) const {
        return std::tie(length, data) < std::tie(x.length, x.data);
    }

    int size() const {
        return length;
    }

    // Require x.size() == y.size()
    static xor_friendly_hash merge(const xor_friendly_hash &x, const xor_friendly_hash &y) {
        assert(x.size() == y.size());
        xor_friendly_hash merged;
        merged.length = x.length;
        for (int d = 0; d < DIMS; d++) {
            merged.data[d] = (x.data[d] ^ y.data[d]);
        }
        return merged;
    }
};
