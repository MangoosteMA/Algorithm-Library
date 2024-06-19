template<typename T>
void hadamard_convolution(std::vector<T> &a) {
    int n = a.size();
    assert(!(n & (n - 1)));
    for (int length = 1; length < n; length <<= 1)
        for (int i = 0; i < n; i += length << 1)
            for (int j = 0; j < length; j++) {
                T v = a[i + j], u = a[i + length + j];
                a[i + j] = v + u;
                a[i + length + j] = v - u;
            }
}

template<typename result_t, typename T1, typename T2>
std::vector<result_t> hadamard(T1 a_begin, T1 a_end, T2 b_begin, T2 b_end) {
    if (a_begin == a_end || b_begin == b_end)
        return {};

    std::vector<result_t> a(a_begin, a_end);
    std::vector<result_t> b(b_begin, b_end);
    int n = std::max(a.size(), b.size());
    while (n & (n - 1))
        n++;

    a.resize(n);
    b.resize(n);
    hadamard_convolution(a);
    hadamard_convolution(b);
    for (int i = 0; i < n; i++)
        a[i] *= b[i];

    hadamard_convolution(a);
    for (auto &x : a)
        x /= n;

    return a;
}
