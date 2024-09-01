// For each 1 <= k <= n returns the number of connected graphs on k vertices with no self-loops.
std::vector<mint> connected_graphs(int n) {
    polynom f(n + 1);
    for (int k = 0; k <= n; k++) {
        f[k] = mint(2).power(1ll * k * (k - 1) / 2) * ifact[k];
    }
    f = f.log(n + 1);
    for (int k = 0; k <= n; k++) {
        f[k] *= fact[k];
    }
    return f;
}
