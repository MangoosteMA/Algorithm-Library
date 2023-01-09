int find_square_root(int a, int p) {
    auto power = [&](int a, int b) {
        int res = 1;
        for (; b; b >>= 1, a = 1ll * a * a % p)
            if (b & 1)
                res = 1ll * res * a % p;

        return res;
    };

    auto power_poly = [&](std::pair<int, int> base, int b, int aa) {
        auto mult = [&](std::pair<int, int> a, std::pair<int, int> b) {
            int k2 = 1ll * a.second * b.second % p;
            int k1 = (1ll * a.second * b.first + 1ll * a.first * b.second) % p;
            int k0 = 1ll * a.first * b.first % p;
            k0 += 1ll * k2 * aa % p;
            if (k0 >= p)
                k0 -= p;

            return std::pair{k0, k1};
        };

        std::pair<int, int> res{1, 0};
        for (; b; b >>= 1, base = mult(base, base))
            if (b & 1)
                res = mult(res, base);

        return res;
    };

    if (a == 0)
        return 0;

    if (p == 2)
        return 1;

    if (power(a, (p - 1) / 2) != 1)
        return -1;

    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    while (true) {
        int i = rng() % (p - 1) + 1;
        // (x + i) ** (p - 1)/2 mod (x**2 - a)
        auto [c, b] = power_poly({i, 1}, (p - 1) / 2, a);
        if (b == 0)
            continue;

        c--;
        if (c == -1)
            c += p;

        int ans = 1ll * c * power(b, p - 2) % p;
        if (1ll * ans * ans % p == a)
            return ans;
    }
}
