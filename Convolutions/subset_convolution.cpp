// (i & j) == 0 => res[i + j] += a[i] * b[j]

template<typename result_t, typename T1, typename T2>
std::vector<result_t> subset_multiply(T1 a_begin, T1 a_end, T2 b_begin, T2 b_end) {
    if (a_begin == a_end || b_begin == b_end)
        return {};

    std::vector<result_t> a(a_begin, a_end);
    std::vector<result_t> b(b_begin, b_end);
    int n = std::max(a.size(), b.size());
    while (n & (n - 1))
        n++;

    std::vector<int> popcount(n);
    for (int mask = 1; mask < n; mask++)
        popcount[mask] = popcount[mask >> 1] + (mask & 1);

    int lg = std::__lg(n);
    std::vector<std::vector<result_t>> at(lg + 1, std::vector<result_t>(n));
    for (int i = 0; i < int(a.size()); i++)
        at[popcount[i]][i] = a[i];

    std::vector<std::vector<result_t>> bt(lg + 1, std::vector<result_t>(n));
    for (int i = 0; i < int(b.size()); i++)
        bt[popcount[i]][i] = b[i];
    
    for (int level = 0; level <= lg; level++) {
        or_convolution(at[level], false);
        or_convolution(bt[level], false);
    }

    std::vector<std::vector<result_t>> prod(lg + 1, std::vector<result_t>(n));
    for (int level1 = 0; level1 <= lg; level1++)
        for (int level2 = 0; level1 + level2 <= lg; level2++)
            for (int i = 0; i < n; i++)
                prod[level1 + level2][i] += at[level1][i] * bt[level2][i];
    
    for (int level = 0; level <= lg; level++)
        or_convolution(prod[level], true);

    std::vector<result_t> result_prod(n);
    for (int i = 0; i < n; i++)
        result_prod[i] += prod[popcount[i]][i];
    
    return result_prod;
}
