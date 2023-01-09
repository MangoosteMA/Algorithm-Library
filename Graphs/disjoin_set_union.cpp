struct disjoin_set_union {
    int components;
    std::vector<int> f;

    disjoin_set_union(int n = 0) : components(n), f(n, -1) {}

    int size() const {
        return f.size();
    }

    void set_default() {
        components = size();
        std::fill(f.begin(), f.end(), -1);
    }

    int root(int v) {
        return f[v] < 0 ? v : f[v] = root(f[v]);
    }

    bool unite(int v, int u) {
        v = root(v), u = root(u);
        if (v == u)
            return false;

        if (f[v] > f[u])
            std::swap(v, u);

        f[v] += f[u];
        f[u] = v;
        components--;
        return true;
    }

    bool connected(int v, int u) {
        return root(v) == root(u);
    }
};
