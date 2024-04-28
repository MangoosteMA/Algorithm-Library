struct line {
    ll k;
    ll b;
    ll temp;

    ll eval() const {
        return k * temp + b;
    }

    ll melting_point(const line& other) const {
        ll val1 = eval();
        ll val2 = other.eval();
        assert(val1 <= val2);
        if (other.k >= k) {
            return LLONG_MAX;
        }

        ll delta_val = val2 - val1;
        ll delta_k = k - other.k;
        assert(delta_val >= 0 && delta_k > 0);
        return (delta_val + delta_k - 1) / delta_k;
    }
};

class kinetic_segtree {
private:
    struct node {
        ll lazy_b = 0;
        ll lazy_temp = 0;
        ll melt = LLONG_MAX;
        line best;

        node(line best = line()) : best(best) {}
    };

    int n;
    vector<node> tree;

    void update(int v) {
        if (make_pair(tree[v << 1].best.eval(), tree[v << 1].best.k) < make_pair(tree[v << 1 | 1].best.eval(), tree[v << 1 | 1].best.k)) {
            tree[v].best = tree[v << 1].best;
            tree[v].melt = tree[v].best.melting_point(tree[v << 1 | 1].best);
        } else {
            tree[v].best = tree[v << 1 | 1].best;
            tree[v].melt = tree[v].best.melting_point(tree[v << 1].best);
        }

        tree[v].melt = min({tree[v].melt, tree[v << 1].melt, tree[v << 1 | 1].melt});
        assert(tree[v].melt > 0);
    }

    void apply(int v, int vl, int vr, ll delta_b, ll delta_temp) {
        tree[v].lazy_b += delta_b;
        tree[v].lazy_temp += delta_temp;

        tree[v].best.b += delta_b;
        tree[v].best.temp += delta_temp;

        tree[v].melt -= delta_temp;
        if (tree[v].melt <= 0) {
            push(v, vl, vr);
            update(v);
        }
    }

    void push(int v, int vl, int vr) {
        int vm = (vl + vr) / 2;
        apply(v << 1, vl, vm, tree[v].lazy_b, tree[v].lazy_temp);
        apply(v << 1 | 1, vm, vr, tree[v].lazy_b, tree[v].lazy_temp);
        tree[v].lazy_b = 0;
        tree[v].lazy_temp = 0;
    }

    void build(int v, int vl, int vr, const vector<line> &lines) {
        if (vr - vl == 1) {
            tree[v] = node(lines[vl]);
            return;
        }

        int vm = (vl + vr) / 2;
        build(v << 1, vl, vm, lines);
        build(v << 1 | 1, vm, vr, lines);
        update(v);
    }

    void add(int v, int vl, int vr, int l, int r, ll delta_b, ll delta_temp) {
        if (r <= vl || vr <= l) {
            return;
        }
        if (l <= vl && vr <= r) {
            apply(v, vl, vr, delta_b, delta_temp);
            return;
        }

        push(v, vl, vr);
        int vm = (vl + vr) / 2;
        add(v << 1, vl, vm, l, r, delta_b, delta_temp);
        add(v << 1 | 1, vm, vr, l, r, delta_b, delta_temp);
        update(v);
    }

    void change_line(int v, int vl, int vr, int pos, const line &new_line) {
        if (vr - vl == 1) {
            tree[v].best = new_line;
            return;
        }

        push(v, vl, vr);
        int vm = (vl + vr) / 2;
        if (pos < vm) {
            change_line(v << 1, vl, vm, pos, new_line);
        } else {
            change_line(v << 1 | 1, vm, vr, pos, new_line);
        }
        update(v);
    }

    ll query(int v, int vl, int vr, int l, int r) {
        if (r <= vl || vr <= l) {
            return LLONG_MAX;
        }
        if (l <= vl && vr <= r) {
            return tree[v].best.eval();
        }

        push(v, vl, vr);
        int vm = (vl + vr) / 2;
        return min(query(v << 1, vl, vm, l, r), query(v << 1 | 1, vm, vr, l, r));
    }

public:
    kinetic_segtree(const vector<line> &lines) : n(lines.size()), tree(4 * n) {
        build(1, 0, n, lines);
    }

    void add(int l, int r, ll delta_b, ll delta_temp) {
        assert(delta_temp >= 0);
        add(1, 0, n, l, r, delta_b, delta_temp);
    }

    void change_line(int pos, const line &new_line) {
        assert(0 <= pos && pos < n);
        change_line(1, 0, n, pos, new_line);
    }

    ll query(int l, int r) {
        return query(1, 0, n, l, r);
    }
};
