// for every substring of t returns its LCS with s in O(|t| * (|s| + |t|))
template<typename T>
std::vector<std::vector<int>> find_all_lcs(const T &s, const T &t) {
    int n = s.size(), m = t.size();
    std::vector<std::vector<int>> vertical(n, std::vector<int>(m + 1));
    std::vector<std::vector<int>> horizontal(n + 1, std::vector<int>(m));
    for (int j = 0; j < m; j++) {
        horizontal[0][j] = j + 1;
        for (int i = 0; i < n; i++) {
            int v = vertical[i][j], h = horizontal[i][j];
            if (v > h || s[i] == t[j]) {
                horizontal[i + 1][j] = v;
                vertical[i][j + 1] = h;
            } else {
                horizontal[i + 1][j] = h;
                vertical[i][j + 1] = v;
            }
        }
    }

    std::vector<std::vector<int>> lcs(m, std::vector<int>(m));
    for (int len = 0; len < m; len++)
        for (int i = 0; i + len < m; i++) {
            int j = i + len;
            lcs[i][j] = (i == j ? 0 : lcs[i][j - 1]) + (i >= horizontal[n][j]);
        }
    
    return lcs;
}
