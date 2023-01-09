template<typename T>
std::vector<int> prefix_function(const T &str) {
    int n = int(str.size());
    std::vector<int> p(n);
    for (int i = 1, j = 0; i < n; i++) {
        while (j > 0 && str[i] != str[j])
            j = p[j - 1];

        if (str[i] == str[j])
            j++;

        p[i] = j;
    }
    return p;
}
