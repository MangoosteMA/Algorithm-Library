std::vector<int> gray_code(int n) {
    const int N = 1 << n;
    std::vector<int> gray(N);
    for (int bit = 0; bit < n; bit++)
        for (int length = 1 << bit, start = 0; start < N; start += length << 2)
            for (int i = start + length; i < N && i < start + 3 * length; i++)
                gray[i] ^= 1 << bit;

    return gray;
}