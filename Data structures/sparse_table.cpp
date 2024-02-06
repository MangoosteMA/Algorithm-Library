/*
 * Zero based.
 * Works for operations Op, such that Op(S) = Op(S, x), where x in S (like min, max, gcd...).
 * Operation must be commutative.
*/

template<typename T, typename merge_t>
struct sparse_table {
    std::vector<std::vector<T>> sparse;
    const merge_t merge;

    sparse_table(const merge_t &merge) : merge(merge) {}

    sparse_table(const std::vector<T> &a, const merge_t &merge) : merge(merge) {
        if (a.empty())
            return;

        const int n = int(a.size()), lg = std::__lg(n);
        sparse.reserve(lg + 1);
        sparse.push_back(a);

        for (int level = 1; level <= lg; level++) {
            sparse.push_back(std::vector<T>(n - (1 << level) + 1));
            for (int i = 0; i < int(sparse[level].size()); i++)
                sparse[level][i] = merge(sparse[level - 1][i], sparse[level - 1][i + (1 << (level - 1))]);
        }
    }

    sparse_table(const sparse_table &sparse) : sparse(sparse.sparse), merge(sparse.merge) {}

    sparse_table& operator=(const sparse_table<T, merge_t> &another) {
        sparse = another.sparse;
        return *this;
    }

    // Returns result of merging elements on the interval [l, r).
    T query(int l, int r) const {
        assert(l < r);
        const int level = std::__lg(r - l);
        return merge(sparse[level][l], sparse[level][r - (1 << level)]);
    }
};

/*
Example:
auto merge_min = [](int x, int y) {
    return x < y ? x : y;
};
sparse_table<int, decltype(merge_min)> sparse(a, merge_min);
*/

/*
For finding the index of the minimum element:
std::vector<int> sparse_init(n);
std::iota(sparse_init.begin(), sparse_init.end(), 0);
auto merge_min = [&](int i, int j) {
    return a[i] < a[j] ? i : j;
};
sparse_table<int, decltype(merge_min)> sparse(sparse_init, merge_min);
*/
