template<typename T>
std::vector<int> z_function(const T &str) {
    int n = int(str.size());
    std::vector<int> z(n);
    for (int i = 1, l = 0, r = 0; i < n; i++) {
        if (i < r)
            z[i] = std::min(r - i, z[i - l]);

        while (i + z[i] < n && str[z[i]] == str[i + z[i]])
            z[i]++;

        if (i + z[i] > r)
            l = i, r = i + z[i];
    }
    return z;
}
