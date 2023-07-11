#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using ld = long double;

#define all(a) begin(a), end(a)
#define len(a) int((a).size())

/*
 * Type T must have operator += T.
 * Type T must have default constructor, which sets neutral value.
 * Operation += must be commutative.
 * For segment query type T must have operator -= T.
*/

template<typename T>
struct binary_indexed_tree {
    std::vector<T> bit;

    binary_indexed_tree(int n = 0) : bit(n + 1) {}

    // returns size of the initial array
    int size() const {
        return int(bit.size()) - 1;
    }

    // adds delta at the position pos (zero based).
    void add(int pos, T delta) {
        for (pos++; pos < int(bit.size()); pos += pos & -pos)
            bit[pos] += delta;
    }

    // returns sum on the interval [0, pref]
    T query(int pref) {
        T sum();
        for (pref++; pref > 0; pref -= pref & -pref)
            sum += bit[pref];

        return sum;
    }

    // returns sum on the interval [l, r)
    T query(int l, int r) {
        if (r <= l)
            return 0;

        T res = query(r - 1);
        res -= query(l - 1);
        return res;
    }
};

int main() {
    cin.tie(nullptr)->sync_with_stdio(false);


}
