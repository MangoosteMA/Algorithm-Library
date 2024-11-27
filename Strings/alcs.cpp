/*
 * T is an array-like type.
 * Returns an array lcs of size |t| times |t| where lcs[l][r] = LCS(s, t[l..r]).
 */
template<typename T>
std::vector<std::vector<int>> alcs(const T &s, const T &t) {
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
    for (int i = 0; i < m; i++) {
        for (int j = i; j < m; j++) {
            lcs[i][j] = (i == j ? 0 : lcs[i][j - 1]) + (i >= horizontal[n][j]);
        }
    }
    return lcs;
}

/*
 * T is an array-like type.
 * Zero based.
 * Requires O(|s| * |t|) memory.
 * This implementation is not very fast and can be improved if queries are offline.
 */
template<typename T>
class prefix_lcs {
private:
    class merge_sort_tree {
    private:
        int n;
        std::vector<std::vector<int>> tree;

        int find(const std::vector<int> &a, int val) {
            return upper_bound(a.begin(), a.end(), val) - a.begin();
        }

    public:
        merge_sort_tree(const std::vector<int> &a = {}) : n(a.size()), tree(2 * n) {
            for (int i = 0; i < n; i++) {
                tree[n + i] = {a[i]};
            }
            for (int i = n - 1; i > 0; i--) {
                tree[i].resize(tree[i << 1].size() + tree[i << 1 | 1].size());
                std::merge(tree[i << 1].begin(), tree[i << 1].end(),
                           tree[i << 1 | 1].begin(), tree[i << 1 | 1].end(), tree[i].begin());
            }
        }

        // Returns the number of elements in [l, r) that are <= val.
        int query(int l, int r, int val) {
            int res = 0;
            for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
                if (l & 1) {
                    res += find(tree[l++], val);
                }
                if (r & 1) {
                    res += find(tree[--r], val);
                }
            }
            return res;
        }
    };

    int n, m;
    std::vector<merge_sort_tree> tree;

public:
    prefix_lcs() {}

    prefix_lcs(const T &s, const T &t) : n(s.size()), m(t.size()), tree(n + 1) {
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

        for (int i = 0; i <= n; i++) {
            tree[i] = merge_sort_tree(horizontal[i]);
        }
    }

    /*
     * Returns LCS(s[0 : pref], t[l : r]).
     * Complexity: O(log^2 (r - l)) with a small constant.
     */
    int query(int pref, int l, int r) {
        return tree[pref].query(l, r, l);
    }
};
