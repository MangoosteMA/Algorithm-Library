/*
 * 0-indexed.
 * T is the data type with:
     - constructor()
     - constructor(container::value_type)
 * merge_t must be commutative.
 */
template<typename T, typename merge_t = std::function<T(T, T)>>
class disjoint_sparse_table {
private:
    merge_t merge;
    std::vector<T> single;
    std::vector<std::vector<T>> sparse;

public:
    disjoint_sparse_table(const merge_t &merge) : merge(merge) {}

    template<typename U>
    disjoint_sparse_table(const U &container, const merge_t &merge)
        : merge(merge), single(container.begin(), container.end())
    {
        int n = container.size();
        int h = std::__lg(std::max(2, n) - 1) + 1;
        sparse.resize(h, std::vector<T>(n));

        for (int l = 0; l < h; l++) {
            int len = (1 << l);
            for (int m = len; m < n; m += (len << 1)) {
                sparse[l][m] = T(container[m]);
                sparse[l][m - 1] = T(container[m - 1]);
                for (int i = 1; i < len; i++) {
                    if (m + i < n) {
                        sparse[l][m + i] = merge(sparse[l][m + i - 1], T(container[m + i]));
                    }
                    sparse[l][m - i - 1] = merge(sparse[l][m - i], T(container[m - i - 1]));
                }
            }
        }
    }

    // Returns the result of merging elements on the interval [l, r).
    T query(int l, int r) {
        assert(l < r);
        if (r - l == 1) {
            return single[l];
        }
        r--;
        int level = std::__lg(l ^ r);
        return merge(sparse[level][l], sparse[level][r]);
    }
};
