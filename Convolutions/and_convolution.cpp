template<typename T>
void and_convolution(std::vector<T> &a, bool inverse = false) {
    int n = a.size();
    assert(!(n & (n - 1)));
    for (int bit = 0; bit < (n == 0 ? -1 : std::__lg(n)); bit++)
        for (int mask = 0; mask < n; mask++)
            if (!(mask >> bit & 1)) {
                assert((mask ^ (1 << bit)) < n);
                a[mask] += inverse ? -a[mask ^ (1 << bit)] : a[mask ^ (1 << bit)];
            }
}

template<typename result_t, typename T1, typename T2>
std::vector<result_t> and_multiply(T1 a_begin, T1 a_end, T2 b_begin, T2 b_end) {
    if (a_begin == a_end || b_begin == b_end)
        return {};

    std::vector<result_t> a(a_begin, a_end);
    std::vector<result_t> b(b_begin, b_end);
    int n = std::max(a.size(), b.size());
    while (n & (n - 1))
        n++;

    a.resize(n);
    b.resize(n);
    and_convolution(a);
    and_convolution(b);
    for (int i = 0; i < n; i++)
        a[i] *= b[i];

    and_convolution(a, true);
    return a;
}