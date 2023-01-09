template<typename T, int SZ>
struct container_hash {
    int add(int a, int b, int j) const {
        a += b;
        if (a >= mods[j])
            return a - mods[j];

        return a;
    }

    int mult(int a, int b, int j) const {
        return (long long) a * b % mods[j];
    }

    int sub(int a, int b, int j) const {
        a -= b;
        if (a < 0)
            return a + mods[j];

        return a;
    }

    std::array<std::vector<int>, SZ> pref_hash, suf_hash, base_degree;
    std::array<int, SZ> mods, bases;

    container_hash() {}
    container_hash(const T &str, const std::array<int, SZ> &mods, const std::array<int, SZ> &bases)
        : mods(mods), bases(bases)
    {
        int n = str.size();
        for (int j = 0; j < SZ; j++) {
            base_degree[j].resize(n + 1, 1 % mods[j]);
            for (int i = 0; i < n; i++)
                base_degree[j][i + 1] = mult(base_degree[j][i], bases[j], j);

            pref_hash[j].resize(n + 1);
            for (int i = 0; i < n; i++)
                pref_hash[j][i + 1] = add(mult(pref_hash[j][i], bases[j], j), str[i], j);

            suf_hash[j].resize(n + 1);
            for (int i = n - 1; i >= 0; i--)
                suf_hash[j][i] = add(mult(suf_hash[j][i + 1], bases[j], j), str[i], j);
        }
    }

    std::array<int, SZ> hash(int l, int r) const {
        std::array<int, SZ> current_hash;
        for (int j = 0; j < SZ; j++)
            current_hash[j] = sub(pref_hash[j][r], mult(pref_hash[j][l], base_degree[j][r - l], j), j);

        return current_hash;
    }

    std::array<int, SZ> reversed_hash(int l, int r) const {
        std::array<int, SZ> current_hash;
        for (int j = 0; j < SZ; j++)
            current_hash[j] = sub(suf_hash[j][l], mult(suf_hash[j][r], base_degree[j][r - l], j), j);

        return current_hash;
    }

    bool palindrom(int l, int r) const {
        return hash(l, r) == reversed_hash(l, r);
    }
};

// constexpr int SZ = 3;
// constexpr array<int, SZ> mods = array<int, SZ>{int(1e9) + 7, int(1e9) + 9, int(1e9) + 21};
// constexpr array<int, SZ> bases = array<int, SZ>{239, 241, 251};

// constexpr int SZ = 2;
// constexpr array<int, SZ> mods = array<int, SZ>{int(1e9) + 7, int(1e9) + 9};
// constexpr array<int, SZ> bases = array<int, SZ>{239, 241};
