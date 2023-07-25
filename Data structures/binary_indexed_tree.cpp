/*
 * Zero based.
 * Type T must have operator += T.
 * Type T must have default constructor, which sets neutral value.
 * Operation += must be commutative.
 * For segment query type T must have operator -= T.
*/

template<typename T>
struct binary_indexed_tree {
    std::vector<T> bit;

    // Fills the array with default values.
    binary_indexed_tree(int n = 0) : bit(n + 1) {}

    // Returns size of the initial array.
    int size() const {
        return int(bit.size()) - 1;
    }

    // Adds delta at the position pos.
    void add(int pos, T delta) {
        for (pos++; pos < int(bit.size()); pos += pos & -pos)
            bit[pos] += delta;
    }

    // Returns sum on the interval [0, pref].
    T query(int pref) {
        T sum = T();
        for (pref++; pref > 0; pref -= pref & -pref)
            sum += bit[pref];

        return sum;
    }

    // Returns sum on the interval [l, r).
    T query(int l, int r) {
        if (r <= l)
            return T();

        T res = query(r - 1);
        res -= query(l - 1);
        return res;
    }
};

/*
 * Zero based.
 * Type T must have same restriction as for binary_indexed_tree.
 * Type T must have operator -T.
*/

template<typename T>
struct segment_adding_binary_indexed_tree {
    binary_indexed_tree<T> values;

    segment_adding_binary_indexed_tree(int n = 0) : values(n) {}

    int size() const {
        return values.size();
    }

    // Adding value on the interval [l, r).
    void add(int l, int r, T value) {
        values.add(l, value);
        values.add(r, -value);
    }

    // Returns the element on the position pos.
    T at(int pos) {
        return values.query(pos);
    }
};

/*
 * Zero based.
 * Works for signed integer types.
*/

template<typename T>
struct segment_adding_segment_query_binary_indexed_tree {
    segment_adding_binary_indexed_tree<T> k, b;

    segment_adding_segment_query_binary_indexed_tree(int n = 0) : k(n), b(n) {}

    int size() const {
        return k.size();
    }

    // Adding val on the interval [l, r).
    void add(int l, int r, T val) {
        if (r <= l)
            return;

        b.add(l, r, -(l - 1) * val);
        b.add(r, size(), (r - l) * val);
        k.add(l, r, val);
    }

    // Returns the sum on the interval [0, pref].
    T query(int pref) {
        return b.at(pref) + pref * k.at(pref);
    }

    // Returns the sum on the interval [l, r).
    T query(int l, int r) {
        if (r <= l)
            return T();

        return query(r - 1) - query(l - 1);
    }
};
