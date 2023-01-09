template<typename T, typename unite_t, T neutral>
struct SegmentTree {
    unite_t unite;
    int n;
    vector<T> tree;

    inline void build() {
        for (int i = n - 1; i; i--)
            tree[i] = unite(tree[i << 1], tree[(i << 1) ^ 1]);
    }

    SegmentTree() {}

    SegmentTree(vector<T> a) : n(int(a.size())), tree(n << 1, neutral) {
        tree.shrink_to_fit();
        for (int i = 0; i < n; i++)
            tree[i + n] = a[i];

        build();
    }

    SegmentTree(int n, T val = neutral) : n(n), tree(n << 1, val) {
        tree.shrink_to_fit();
        build();
    }

    void rebuild(int pos) {
        for (pos >>= 1; pos; pos >>= 1)
            tree[pos] = unite(tree[pos << 1], tree[(pos << 1) ^ 1]);
    }

    void change(int pos, T val, const function<void(T &value, T x)> relax) {
        pos += n;
        relax(tree[pos], val);
        rebuild(pos);
    }

    void set(int pos, T val) {
        pos += n;
        tree[pos] = val;
        rebuild(pos);
    }

    void add(int pos, T val) {
        pos += n;
        tree[pos] += val;
        rebuild(pos);
    }

    void setmax(int pos, T val) {
        pos += n;
        if (tree[pos] < val) {
            tree[pos] = val;
            rebuild(pos);
        }
    }

    void setmin(int pos, T val) {
        pos += n;
        if (tree[pos] > val) {
            tree[pos] = val;
            rebuild(pos);
        }
    }

    T get(int l, int r) const {
        if (l < 0)
            l = 0;

        if (r > n)
            r = n;

        T left_res = neutral, right_res = neutral;
        for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
            if (l & 1)
                left_res = unite(left_res, tree[l++]);

            if (r & 1)
                right_res = unite(tree[--r], right_res);
        }
        return unite(left_res, right_res);
    }
};


template<typename T>
struct SegmentTreeSumComparator {
    T operator () (const T &a, const T& b) const {
        return a + b;
    }
};

template<typename T>
struct SegmentTreeMinComparator {
    T operator () (const T &a, const T &b) const {
        return a < b ? a : b;
    }
};

template<typename T>
struct SegmentTreeMaxComparator {
    T operator () (const T &a, const T &b) const {
        return a > b ? a : b;
    }
};


template<typename T>
using SegmentTreeSum = SegmentTree<T, SegmentTreeSumComparator<T>, T(0)>;

template<typename T>
using SegmentTreeMin = SegmentTree<T, SegmentTreeMinComparator<T>, numeric_limits<T>::max()>;

template<typename T>
using SegmentTreeMax = SegmentTree<T, SegmentTreeMaxComparator<T>, numeric_limits<T>::min()>;
