// min= на отрезке, = в точке, sum, max

struct segtree {
    struct node {
        int max, cnt, second, mod;
        ll sum;
 
        node() {}
        node(int x) : max(x), cnt(1), second(INT_MIN), mod(INT_MAX), sum(x) {}
    };
 
    int n;
    vec<node> tree;
 
    segtree(int n, int val) : n(n), tree(4 * n) {
        build(1, 0, n, val);
    }
 
    node merge(node a, node b) const {
        assert(a.mod == INT_MAX && b.mod == INT_MAX);
        if (a.max < b.max)
            swap(a, b);
 
        node res;
        res.mod = INT_MAX;
        res.sum = a.sum + b.sum;
 
        res.max = a.max;
        res.cnt = a.cnt;
        if (b.max == a.max) {
            res.cnt += b.cnt;
            res.second = max(a.second, b.second);
        } else {
            res.second = max(a.second, b.max);
        }
 
        return res;
    }
 
    void build(int v, int vl, int vr, int val) {
        if (vr - vl == 1) {
            tree[v] = node(val);
            return;
        }
 
        int vm = (vl + vr) >> 1;
        build(v << 1, vl, vm, val);
        build(v << 1 | 1, vm, vr, val);
 
        tree[v] = merge(tree[v << 1], tree[v << 1 | 1]);
    }
 
    void push(int v, int vl, int vr) {
        if (tree[v].mod == INT_MAX)
            return;
 
        if (vr - vl > 1) {
            tree[v << 1].mod = min(tree[v << 1].mod, tree[v].mod);
            tree[v << 1 | 1].mod = min(tree[v << 1 | 1].mod, tree[v].mod);
        }
 
        assert(tree[v].mod > tree[v].second);
        if (tree[v].mod < tree[v].max) {
            tree[v].sum -= 1ll * tree[v].cnt * tree[v].max;
            tree[v].max = tree[v].mod;
            tree[v].sum += 1ll * tree[v].cnt * tree[v].max;
        }
 
        tree[v].mod = INT_MAX;
    }
 
    void setmin(int l, int r, int val) {
        setmin(1, 0, n, l, r, val);
    }
 
    void setmin(int v, int vl, int vr, int l, int r, int val) {
        push(v, vl, vr);
        if (r <= vl || vr <= l || val >= tree[v].max)
            return;
 
        if (l <= vl && vr <= r && val > tree[v].second) {
            tree[v].mod = min(tree[v].mod, val);
            push(v, vl, vr);
            return;
        }
 
        int vm = (vl + vr) >> 1;
        setmin(v << 1, vl, vm, l, r, val);
        setmin(v << 1 | 1, vm, vr, l, r, val);
 
        tree[v] = merge(tree[v << 1], tree[v << 1 | 1]);
    }
 
    void set(int pos, int val) {
        set(1, 0, n, pos, val);
    }
 
    void set(int v, int vl, int vr, int pos, int val) {
        push(v, vl, vr);
        if (pos < vl || vr <= pos)
            return;
 
        if (vr - vl == 1) {
            tree[v] = node(val);
            return;
        }
 
        int vm = (vl + vr) >> 1;
        set(v << 1, vl, vm, pos, val);
        set(v << 1 | 1, vm, vr, pos, val);
 
        tree[v] = merge(tree[v << 1], tree[v << 1 | 1]);
    }
 
    ll get(int l, int r) {
        return get(1, 0, n, l, r);
    }
 
    ll get(int v, int vl, int vr, int l, int r) {
        push(v, vl, vr);
        if (r <= vl || vr <= l)
            return 0;
 
        if (l <= vl && vr <= r)
            return tree[v].sum;
 
        int vm = (vl + vr) >> 1;
        return get(v << 1, vl, vm, l, r) + get(v << 1 | 1, vm, vr, l, r);
    }
};

// min= на отрезке, += на отрезке, sum, max

struct segtree {
    struct node {
        ll mx;
        int cnt;
        ll second_mx, sum;
 
        node() {}
        node(int x) : mx(x), cnt(1), second_mx(LLONG_MIN), sum(x) {}
    };
 
    node merge(node a, node b) const {
        if (a.mx < b.mx)
            swap(a, b);
 
        node res;
        res.sum = a.sum + b.sum;
 
        res.mx = a.mx;
        res.cnt = a.cnt;
        if (b.mx == res.mx) {
            res.cnt += b.cnt;
            res.second_mx = max(a.second_mx, b.second_mx);
        } else {
            res.second_mx = max(a.second_mx, b.mx);
        }
 
        return res;
    }
 
    struct modificator {
        ll to_add, to_set;
 
        modificator() : to_add(0), to_set(LLONG_MAX) {}
 
        void setmin(ll val) {
            to_set = min(to_set, val);
        }
 
        void add(ll val) {
            to_add += val;
            if (to_set != LLONG_MAX)
                to_set += val;
        }
    };
 
    int n;
    vec<node> tree;
    vec<modificator> mod;
 
    segtree(vec<int> a) : n(len(a)), tree(4 * n), mod(4 * n) {
        build(1, 0, n, a);
    }
 
    void build(int v, int vl, int vr, const vec<int> &a) {
        if (vr - vl == 1) {
            tree[v] = node(a[vl]);
            return;
        }
 
        int vm = (vl + vr) >> 1;
        build(v << 1, vl, vm, a);
        build(v << 1 | 1, vm, vr, a);
 
        tree[v] = merge(tree[v << 1], tree[v << 1 | 1]);
    }
 
    void push(int v, int vl, int vr) {
        if (vr - vl > 1) {
            mod[v << 1].add(mod[v].to_add);
            mod[v << 1 | 1].add(mod[v].to_add);
 
            mod[v << 1].setmin(mod[v].to_set);
            mod[v << 1 | 1].setmin(mod[v].to_set);
        }
 
        tree[v].sum += (vr - vl) * mod[v].to_add;
        tree[v].mx += mod[v].to_add;
        if (tree[v].second_mx != LLONG_MIN)
            tree[v].second_mx += mod[v].to_add;
 
        if (mod[v].to_set <= tree[v].second_mx) {
            int vm = (vl + vr) >> 1;
            push(v << 1, vl, vm);
            push(v << 1 | 1, vm, vr);
            tree[v] = merge(tree[v << 1], tree[v << 1 | 1]);
        } else if (mod[v].to_set < tree[v].mx) {
            tree[v].sum -= 1ll * tree[v].cnt * tree[v].mx;
            tree[v].mx = mod[v].to_set;
            tree[v].sum += 1ll * tree[v].cnt * tree[v].mx;
        }
 
        mod[v] = modificator();
    }
 
    void setmin(int l, int r, int val) {
        setmin(1, 0, n, l, r, val);
    }
 
    void setmin(int v, int vl, int vr, int l, int r, int val) {
        push(v, vl, vr);
        if (r <= vl || vr <= l)
            return;
 
        if (l <= vl && vr <= r) {
            mod[v].setmin(val);
            push(v, vl, vr);
            return;
        }
 
        int vm = (vl + vr) >> 1;
 
        setmin(v << 1, vl, vm, l, r, val);
        setmin(v << 1 | 1, vm, vr, l, r, val);
        tree[v] = merge(tree[v << 1], tree[v << 1 | 1]);
    }
 
    void add(int l, int r, int val) {
        add(1, 0, n, l, r, val);
    }
 
    void add(int v, int vl, int vr, int l, int r, int val) {
        if (r <= vl || vr <= l)
            return;
 
        if (l <= vl && vr <= r) {
            mod[v].add(val);
            return;
        }
 
        push(v, vl, vr);
        int vm = (vl + vr) / 2;
 
        add(v << 1, vl, vm, l, r, val);
        add(v << 1 | 1, vm, vr, l, r, val);
 
        push(v << 1, vl, vm);
        push(v << 1 | 1, vm, vr);
        tree[v] = merge(tree[v << 1], tree[v << 1 | 1]);
    }
 
    ll get(int l, int r) {
        return get(1, 0, n, l, r);
    }
 
    ll get(int v, int vl, int vr, int l, int r) {
        if (r <= vl || vr <= l)
            return 0;
 
        push(v, vl, vr);
        if (l <= vl && vr <= r)
            return tree[v].sum;
 
        int vm = (vl + vr) >> 1;
        return get(v << 1, vl, vm, l, r) + get(v << 1 | 1, vm, vr, l, r);
    }
};
