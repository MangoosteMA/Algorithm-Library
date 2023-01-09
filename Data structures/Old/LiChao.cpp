template<class T>
struct LiChao {
    struct Line {
        T k, b;
        bool empty;

        Line() : empty(true) {}
        Line(T k, T b) : k(k), b(b), empty(false) {}

        inline T getVal(T x) {
            return k * x + b;
        }
    };

    int n;
    vector<Line> tree;

    LiChao() {}
    LiChao(int n) : n(n), tree(4 * n) {}

    inline void addLine(Line ln) {
        addLine(1, 0, n, ln);
    }

    void addLine(int v, int vl, int vr, Line &ln) {
        if (tree[v].empty) {
            tree[v] = ln;
            return;
        }

        T a = tree[v].getVal(vl);
        T b = tree[v].getVal(vr - 1);
        T c = ln.getVal(vl);
        T d = ln.getVal(vr - 1);

        if (a >= c && b >= d)
            return;

        if (c >= a && d >= b) {
            tree[v] = ln;
            return;
        }

        int vm = (vl + vr) / 2;
        addLine(2 * v, vl, vm, ln);
        addLine(2 * v + 1, vm, vr, ln);
    }

    inline T get(int pos) {
        return get(1, 0, n, pos, numeric_limits<T>::min());
    }

    T get(int v, int vl, int vr, int pos, T mx) {
        if (!tree[v].empty)
            mx = max(mx, tree[v].getVal(pos));

        if (vr - vl == 1)
            return mx;

        int vm = (vl + vr) / 2;
        if (pos < vm)
            return get(2 * v, vl, vm, pos, mx);

        return get(2 * v + 1, vm, vr, pos, mx);
    }
};

// using Line = LiChao<ll>::Line;
