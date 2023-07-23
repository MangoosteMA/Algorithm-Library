/*
 * Node must have default constructor.
 * Node must have static function merge.
 * Node must have .push and .clear_after_push methods.
*/

template<typename node>
class segtree {
private:
    void build(int v, int vl, int vr) {
        if (vr - vl <= 1)
            return;
 
        int vm = (vl + vr) >> 1;
        build(v << 1, vl, vm);
        build(v << 1 | 1, vm, vr);
        tree[v] = node::merge(tree[v << 1], tree[v << 1 | 1]);
    }

    template<typename T>
    void build(int v, int vl, int vr, const std::vector<T> &arr) {
        if (vl == vr)
            return;
 
        if (vr - vl == 1) {
            tree[v] = node(arr[vl]);
            return;
        }
 
        int vm = (vl + vr) >> 1;
        build(v << 1, vl, vm, arr);
        build(v << 1 | 1, vm, vr, arr);
        tree[v] = node::merge(tree[v << 1], tree[v << 1 | 1]);
    }

    template<typename... Args>
    void _update(int v, int vl, int vr, int l, int r, Args&&... args) {
        if (r <= vl || vr <= l)
            return;
 
        if (l <= vl && vr <= r) {
            tree[v].apply(std::forward<Args>(args)..., vl, vr);
            return;
        }
 
        int vm = (vl + vr) >> 1;
        tree[v].push(tree[v << 1], vl, vm);
        tree[v].push(tree[v << 1 | 1], vm, vr);
        tree[v].clear_after_push();
 
        _update(v << 1, vl, vm, l, r, std::forward<Args>(args)...);
        _update(v << 1 | 1, vm, vr, l, r, std::forward<Args>(args)...);
        tree[v] = node::merge(tree[v << 1], tree[v << 1 | 1]);
    }

    node _query(int v, int vl, int vr, int l, int r) {
        if (l <= vl && vr <= r)
            return tree[v];
 
        int vm = (vl + vr) >> 1;
        tree[v].push(tree[v << 1], vl, vm);
        tree[v].push(tree[v << 1 | 1], vm, vr);
        tree[v].clear_after_push();
 
        if (r <= vm)
            return _query(v << 1, vl, vm, l, r);
 
        if (vm <= l)
            return _query(v << 1 | 1, vm, vr, l, r);
 
        return node::merge(_query(v << 1, vl, vm, l, r), _query(v << 1 | 1, vm, vr, l, r));
    }

    template<typename T>
    int _find_first(int v, int vl, int vr, int from, const T &checker) {
        if (vr <= from)
            return n;

        if (from <= vl && !checker(tree[v], vl, vr))
            return n;

        if (vr - vl == 1)
            return vl;

        int vm = (vl + vr) >> 1;
        tree[v].push(tree[v << 1], vl, vm);
        tree[v].push(tree[v << 1 | 1], vm, vr);
        tree[v].clear_after_push();

        int res = _find_first(v << 1, vl, vm, from, checker);
        return res == n ? _find_first(v << 1 | 1, vm, vr, from, checker) : res;
    }

    template<typename T>
    int _find_last(int v, int vl, int vr, int from, const T &checker) {
        if (from <= vl)
            return -1;

        if (vr <= from && !checker(tree[v], vl, vr))
            return -1;

        if (vr - vl == 1)
            return vl;

        int vm = (vl + vr) >> 1;
        tree[v].push(tree[v << 1], vl, vm);
        tree[v].push(tree[v << 1 | 1], vm, vr);
        tree[v].clear_after_push();

        int res = _find_last(v << 1 | 1, vm, vr, from, checker);
        return res == -1 ? _find_last(v << 1, vl, vm, from, checker) : res;
    }

public:
    int n;
    std::vector<node> tree;

    segtree(int n) : n(n), tree(4 * n, node()) {
        build(1, 0, n);
    }

    template<typename T>
    segtree(const std::vector<T> &arr) : n(arr.size()), tree(4 * n) {
        build(1, 0, n, arr);
    }
 
    int size() const {
        return n;
    }
 
    template<typename... Args>
    void update(int l, int r, Args&&... args) {
        if (r <= l)
            return;

        _update(1, 0, n, l, r, std::forward<Args>(args)...);
    }
 
    node query(int l, int r) {
        assert(std::max(0, l) < std::min(n, r)); // or return node() in this case
        return _query(1, 0, n, l, r);
    }

    // Trying to find first true on the interval [from, n).
    // Returns n if not found.
    template<typename T>
    int find_first(int from, const T &checker) {
        return _find_first(1, 0, n, from, checker);
    }

    // Trying to find last true on the interval [0, from).
    // Returns -1 if not found.
    template<typename T>
    int find_last(int from, const T &checker) {
        return _find_last(1, 0, n, from, checker);
    }
};

// Node template:
// struct node {
//     node() {}
//     void apply(..., int /* vl */, int /* vr */) {}
//     void push(node /* &child */, int /* cl */, int /* cr */) {}
//     void clear_after_push() {}
//     static node merge(const node &left, const node &right) {}
// };

/*
struct node {
    ll mn = 0, mod = 0, sum = 0;

    node() {}

    void apply(ll delta, int vl, int vr) {
        mod += delta;
        sum += 1ll * (vr - vl) * delta;
        mn += delta;
    }

    void push(node &child, int cl, int cr) {
        child.apply(mod, cl, cr);
    }
    
    void clear_after_push() {
        mod = 0;
    }

    static node merge(const node &left, const node &right) {
        node res;
        res.sum = left.sum + right.sum;
        res.mn = min(left.mn, right.mn);
        assert(res.mod == 0);
        return res;
    }
};
*/
